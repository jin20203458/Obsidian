
참조:
[[ArrayBoundChecker.cpp]]

이 체커는 아래 3가지 타입의 AST 노드가 **실행된 뒤**(PostStmt)마다 분석을 수행

- **ArraySubscriptExpr**: 배열 인덱스(예: arr[x])
- **UnaryOperator**: 포인터 deref(예: *p), (단, UO_Deref만 분석)
- **MemberExpr**: 구조체/클래스 포인터 멤버 접근(예: p->field)

```cpp
//== ArrayBoundCheckerV2.cpp ------------------------------------*- C++ -*--==//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file defines ArrayBoundCheckerV2, which is a path-sensitive check
// which looks for an out-of-bound array element access.
//
//===----------------------------------------------------------------------===//

#include "clang/AST/CharUnits.h"
#include "clang/AST/ParentMapContext.h"
#include "clang/StaticAnalyzer/Checkers/BuiltinCheckerRegistration.h"
#include "clang/StaticAnalyzer/Checkers/Taint.h"
#include "clang/StaticAnalyzer/Core/BugReporter/BugType.h"
#include "clang/StaticAnalyzer/Core/Checker.h"
#include "clang/StaticAnalyzer/Core/CheckerManager.h"
#include "clang/StaticAnalyzer/Core/PathSensitive/APSIntType.h"
#include "clang/StaticAnalyzer/Core/PathSensitive/CheckerContext.h"
#include "clang/StaticAnalyzer/Core/PathSensitive/DynamicExtent.h"
#include "clang/StaticAnalyzer/Core/PathSensitive/ExprEngine.h"
#include "llvm/ADT/SmallString.h"
#include "llvm/Support/FormatVariadic.h"
#include "llvm/Support/raw_ostream.h"
#include <optional>

using namespace clang;
using namespace ento;
using namespace taint;
using llvm::formatv;

namespace {
/// 만약 E가 "깨끗한(clean)" 배열 인덱스 표현식이라면,
/// 접근하는 요소의 타입을 반환한다.
/// 만약 배열의 베이스(시작점)가 포인터 연산(예: arr + x)에 의해 바뀌었다면
/// (즉, 전체 메모리 region의 시작이 아니라면),
/// 이 함수는 항상 nullopt(값 없음) 를 반환한다.
/// 왜냐하면 이런 경우, 진단 메시지를 위한 최선의(혹은 그나마 덜 나쁜) 선택이기 때문이다.
static std::optional<QualType> determineElementType(const Expr *E,
                                                    const CheckerContext &C) 
{
  // [ ] 배열 인덱스 표현식이 아니면 nullopt 반환
  const auto *ASE = dyn_cast<ArraySubscriptExpr>(E);
  if (!ASE)
    return std::nullopt;

  // [ ] 배열 인덱스 표현식의 베이스가 MemRegion이 아니면 nullopt 반환
  const MemRegion *SubscriptBaseReg = C.getSVal(ASE->getBase()).getAsRegion();
  if (!SubscriptBaseReg)
    return std::nullopt;

  // 첨자 표현식의 밑변은 포인터 산술의 영향을 받습니다,
  // 그래서 우리는 인덱스 대신 바이트 오프셋을 보고하고자 합니다.
  // StricpCasts()는 MemRegion의 상위 레벨을 반환합니다.
  // ElementRegion 이면 nullopt 반환(포인터 산술 영향)
  if (isa<ElementRegion>(SubscriptBaseReg->StripCasts()))
    return std::nullopt;

  // 최종적으로 [ ] 의 타입 반환
  return ASE->getType();
}

// 배열요소의 사이즈 반환(바이트 단위)
static std::optional<int64_t>
determineElementSize(const std::optional<QualType> T, const CheckerContext &C) 
{
  if (!T)
    return std::nullopt;
  return C.getASTContext().getTypeSizeInChars(*T).getQuantity();
}

// 배열 범위 체크 중, 
// "가정(assumption)"에 대한 설명/메시지를 만들어주는 진단 보조용 클래스
class StateUpdateReporter 
{
  const SubRegion *Reg;
  const NonLoc ByteOffsetVal;
  const std::optional<QualType> ElementType;
  const std::optional<int64_t> ElementSize;

  bool AssumedNonNegative = false;
  std::optional<NonLoc> AssumedUpperBound = std::nullopt;

public:
  StateUpdateReporter(const SubRegion *R, NonLoc ByteOffsVal, const Expr *E,
                      CheckerContext &C)
      : Reg(R), ByteOffsetVal(ByteOffsVal),
        ElementType(determineElementType(E, C)),
        ElementSize(determineElementSize(ElementType, C)) {}

  void recordNonNegativeAssumption() { AssumedNonNegative = true; }
  void recordUpperBoundAssumption(NonLoc UpperBoundVal) 
  {
    AssumedUpperBound = UpperBoundVal;
  }

  bool assumedNonNegative() { return AssumedNonNegative; }

  const NoteTag *createNoteTag(CheckerContext &C) const;

private:
  std::string getMessage(PathSensitiveBugReport &BR) const;

  /// Return true if information about the value of `Sym` can put constraints
  /// on some symbol which is interesting within the bug report `BR`.
  /// In particular, this returns true when `Sym` is interesting within `BR`;
  /// but it also returns true if `Sym` is an expression that contains integer
  /// constants and a single symbolic operand which is interesting (in `BR`).
  /// We need to use this instead of plain `BR.isInteresting()` because if we
  /// are analyzing code like
  ///   int array[10];
  ///   int f(int arg) {
  ///     return array[arg] && array[arg + 10];
  ///   }
  /// then the byte offsets are `arg * 4` and `(arg + 10) * 4`, which are not
  /// sub-expressions of each other (but `getSimplifiedOffsets` is smart enough
  /// to detect this out of bounds access).
  static bool providesInformationAboutInteresting(SymbolRef Sym,
                                                  PathSensitiveBugReport &BR);

  static bool providesInformationAboutInteresting(SVal SV,
                                                  PathSensitiveBugReport &BR) {
    return providesInformationAboutInteresting(SV.getAsSymbol(), BR);
  }
};

struct Messages 
{
  std::string Short, Full;
};

// NOTE: The `ArraySubscriptExpr` and `UnaryOperator` callbacks are `PostStmt`
// instead of `PreStmt` because the current implementation passes the whole
// expression to `CheckerContext::getSVal()` which only works after the
// symbolic evaluation of the expression. (To turn them into `PreStmt`
// callbacks, we'd need to duplicate the logic that evaluates these
// expressions.) The `MemberExpr` callback would work as `PreStmt` but it's
// defined as `PostStmt` for the sake of consistency with the other callbacks.
class ArrayBoundCheckerV2 : public Checker<check::PostStmt<ArraySubscriptExpr>,
                                           check::PostStmt<UnaryOperator>,
                                           check::PostStmt<MemberExpr>> {

  BugType BT{this, "Out-of-bound access"};
  BugType TaintBT{this, "Out-of-bound access", categories::TaintedData};

  void performCheck(const Expr *E, CheckerContext &C) const;

  void reportOOB(CheckerContext &C, ProgramStateRef ErrorState, Messages Msgs,
                 NonLoc Offset, std::optional<NonLoc> Extent,
                 bool IsTaintBug = false) const;

  static void markPartsInteresting(PathSensitiveBugReport &BR,
                                   ProgramStateRef ErrorState, NonLoc Val,
                                   bool MarkTaint);

  static bool isFromCtypeMacro(const Stmt *S, ASTContext &AC);

  static bool isIdiomaticPastTheEndPtr(const Expr *E, ProgramStateRef State,
                                       NonLoc Offset, NonLoc Limit,
                                       CheckerContext &C);

  static bool isInAddressOf(const Stmt *S, ASTContext &AC);

public:
  void checkPostStmt(const ArraySubscriptExpr *E, CheckerContext &C) const
  {
    performCheck(E, C);
  }
  void checkPostStmt(const UnaryOperator *E, CheckerContext &C) const 
  {
    if (E->getOpcode() == UO_Deref)
      performCheck(E, C);
  }
  void checkPostStmt(const MemberExpr *E, CheckerContext &C) const 
  {
    if (E->isArrow())
      performCheck(E->getBase(), C);
  }
};

} // anonymous namespace

/// For a given Location that can be represented as a symbolic expression
/// Arr[Idx] (or perhaps Arr[Idx1][Idx2] etc.), return the parent memory block
/// Arr and the distance of Location from the beginning of Arr (expressed in a
/// NonLoc that specifies the number of CharUnits). Returns nullopt when these
/// cannot be determined.
/// 
/// 이게 어떤 배열(arr)에서, 시작점으로부터 몇 바이트 떨어진 곳이냐?
static std::optional<std::pair<const SubRegion *, NonLoc>>
computeOffset(ProgramStateRef State, SValBuilder &SVB, SVal Location) 
{
  QualType T = SVB.getArrayIndexType();

  auto EvalBinOp = [&SVB, State, T](BinaryOperatorKind Op, NonLoc L, NonLoc R) 
      {
    // 이 유틸리티를 사용하여 값을 더하고 곱할 것입니다.
    // (evaluate binary operation on NonLoc, NonLoc)
    return SVB.evalBinOpNN(State, Op, L, R, T).getAs<NonLoc>();};

  // 배열의 시작점이 될 메모리 영역
  const SubRegion *OwnerRegion = nullptr;
  // 배열의 시작점으로부터 몇 바이트 떨어져 있는지 계산할 NonLoc
  std::optional<NonLoc> Offset = SVB.makeZeroArrayIndex();


  // 현재 영역
  const ElementRegion *CurRegion =
      dyn_cast_or_null<ElementRegion>(Location.getAsRegion());

  // 본인이 최상위라면 종료, 본인 또하 종속된 배열이라면 상위로 가서 계산
  while (CurRegion) 
  {
      // 인덱스 확인
    const auto Index = CurRegion->getIndex().getAs<NonLoc>();

    if (!Index)
      return std::nullopt;

    // 배열요소의 타입을 가져옴
    QualType ElemType = CurRegion->getElementType();

    // "이 early return은 getTypeSizeInChars()가 불완전 타입을 받으면 안 되기 때문에 추가된 것 같다.
    // 하지만 실제로 이 시점에서 ElemType이 불완전할 가능성은 없어 보인다."
    if (ElemType->isIncompleteType())
      return std::nullopt;

    // 현재 요소가 얼마나 떨어져 있는지 계산
    NonLoc Size = SVB.makeArrayIndex(
        SVB.getContext().getTypeSizeInChars(ElemType).getQuantity());
    auto Delta = EvalBinOp(BO_Mul, *Index, Size);
   
    if (!Delta)
      return std::nullopt;

    // Perform Offset += Delta.
    Offset = EvalBinOp(BO_Add, *Offset, *Delta);
    if (!Offset)
      return std::nullopt;

    OwnerRegion = CurRegion->getSuperRegion()->getAs<SubRegion>();
    // 이것이 단지 또 다른 ElementRegion 레이어일 때, 우리는 다음을 계속해야
    // 합니다
    // 오프셋 계산:(OwnerRegion이 ElementRegion이 아닌 경우 종료)
    CurRegion = dyn_cast_or_null<ElementRegion>(OwnerRegion);
  }

  // 배열시작점과 offset이 모두 계산되었으면, 반환
  if (OwnerRegion)
    return std::make_pair(OwnerRegion, *Offset);

  return std::nullopt;
}

// NOTE: 이 함수는 이 체커의 "핵심" 함수입니다.
// - 수학적으로는 불등식(예: X+1 < 10)을 단순히 X < 9처럼 쉽게 변형할 수 있지만,
// - C++에서는 자료형 오버플로우나 자동 타입 변환(size_t 등) 때문에
//   항상 이런 변형이 안전하게 동작하지 않을 수 있습니다.
//
// - 이런 오버플로우 문제 때문에, 모든 불등식 비교(evalBinOp 등)에
//   이 단순화 기법을 통합하는 건 비현실적(혹은 불가능)입니다.
// - 하지만 "메모리 오프셋 계산"처럼 값이 SIZE_MAX(최대값)보다 한참 작은
// 상황에서는
//   이 함수가 항상 올바른 결과를 제공합니다.
//
// TODO: 이 알고리즘은 메모리 오프셋 비교가 필요한 다른 체커들도
//       사용할 수 있도록 공통 함수로 분리하는 것이 바람직합니다.
//
// 참고: 이 함수의 수학적 단순화는 좌변/우변(피연산자)의 순서는 유지하지만,
//       C++의 비교 연산자/자동 타입 변환 결과는 달라질 수 있습니다.
// 예를 들어, "X+1 < 0"에서 X가 size_t(부호 없는 정수), 0이 int(부호 있는
// 정수)라면
//   - C++에서는 X+1은 음수가 될 수 없으니 항상 false입니다.
//   - 그런데 이 조건을 단순화해서 "X < -1"로 바꾸면,
//     - -1이 size_t로 자동 변환되어 엄청 큰 값이 되고,
//     - "X < -1"은 C++에서 사실상 항상 true로 평가될 수 있습니다.
// 즉, 수학적으로는 변형해도 의미가 같지만, C++의 실제 동작에서는 그 결과가 다를
// 수 있습니다.

// getSimplifiedOffsets는 심볼릭 수식(예: x+3, 2*y 등)에서
// 경계 비교를 좀 더 단순하게(수학적으로 쉽게) 바꿔주는 함수입니다

// 예를 들어 idx*4 < 40 → idx < 10, x+2 < 7 → x < 5
static std::pair<NonLoc, nonloc::ConcreteInt>
getSimplifiedOffsets(NonLoc offset, nonloc::ConcreteInt extent,
                     SValBuilder &svalBuilder) 
{
    // NonLoc을 SymbolVal로 다운캐스팅 시도
  std::optional<nonloc::SymbolVal> SymVal = offset.getAs<nonloc::SymbolVal>();
  
  // 만약 NonLoc이 SymbolVal이고, 표현식일 경우
  if (SymVal && SymVal->isExpression()) 
  {
    // 심볼릭 표현식이 SymIntExpr인지 확인
    if (const SymIntExpr *SIE = dyn_cast<SymIntExpr>(SymVal->getSymbol())) 
    {
    
       // SIE의 오른쪽 피연산자를 extent(배열 허용값)와 동일한 타입으로 변환
      llvm::APSInt constant =
          APSIntType(extent.getValue()).convert(SIE->getRHS());

      // 연산자 종류확인후 처리
      switch (SIE->getOpcode()) {
      case BO_Mul:
        //이 함수가 실행되는 시점에는 곱하는 값이 0인 경우는 이미
        //  처리되었으므로, 0으로 나누는 경우는 없습니다.

      // 나누어 떨어지지 않는 경우 e.g x*4 < 5 (단순화 불가)
        if ((extent.getValue() % constant) != 0)
          return std::pair<NonLoc, nonloc::ConcreteInt>(offset, extent);
        else // 나누어 떨어지는 경우 e.g x*4 < 8 → x < 2 (단순화, 재귀호출)
          return getSimplifiedOffsets(
              nonloc::SymbolVal(SIE->getLHS()),
              svalBuilder.makeIntVal(extent.getValue() / constant),
              svalBuilder);
      case BO_Add: // 덧셈의 경우, 우변 상수를 extent에서 빼고 재귀호출
        return getSimplifiedOffsets(
            nonloc::SymbolVal(SIE->getLHS()),
            svalBuilder.makeIntVal(extent.getValue() - constant), svalBuilder);
      default:
        break;
      }
    }
  }
  // 더 이상 단순화 불가시 현재 값 반환
  return std::pair<NonLoc, nonloc::ConcreteInt>(offset, extent);
}

// 주어진 Value(NonLoc 타입, 즉 심볼릭 정수 값 등)가
// 음수일 가능성이 있는지를 판단하는 함수
static bool isNegative(SValBuilder &SVB, ProgramStateRef State, NonLoc Value) 
{
    // 범위를 벗어난 값인지 확인
  const llvm::APSInt *MaxV = SVB.getMaxValue(State, Value);
  // 최대값이 존재하고, 그값이 음수이면 반환
  return MaxV && MaxV->isNegative();
}

// 주어진 Value가 부호 없는 정수 타입인지 확인하는 함수
static bool isUnsigned(SValBuilder &SVB, NonLoc Value) 
{
  QualType T = Value.getType(SVB.getContext());
  return T->isUnsignedIntegerType();
}


// Value < Threshold 비교를 이 체커에 정의된 커스텀 단순화 알고리즘을 이용해
// 평가한다. 반환값은 상태 쌍(pair)로, 첫 번째는 "value가 threshold 미만"인
// 상태(True), 두 번째는 "value가 threshold 이상"인 상태(false)를 의미한다. 평가에 실패하면
// {nullptr, nullptr}을 반환한다. 선택적 인자 CheckEquality가 true면, 기본값인
// BO_LT(<) 대신 BO_EQ(==)로 비교를 수행한다. 이때도 동일한 단순화 과정을
// 거친다.
// 항상참, 또는 거짓일 경우는 분기를 만들지 않는다.(확정된 미래이기 때문에)
static std::pair<ProgramStateRef, ProgramStateRef>
compareValueToThreshold(ProgramStateRef State, NonLoc Value, NonLoc Threshold,
                        SValBuilder &SVB, bool CheckEquality = false) {

  // Threshold가 상수(ConcreteInt)라면, Value와 Threshold를 단순화한다.
  if (auto ConcreteThreshold = Threshold.getAs<nonloc::ConcreteInt>()) {
    std::tie(Value, Threshold) =
        getSimplifiedOffsets(Value, *ConcreteThreshold, SVB);
  }

  // 우리는 Value와 Threshold의 "수학적" 비교를 원한다.
  // 하지만 evalBinOpNN은 C/C++ 연산자를 평가하는데, 이는 자동 타입 변환이
  // 일어날 수 있다. 예를 들어, -1은 1000보다 작지만, -1 < 1000ull(C++에서)
  // 비교는 false가 된다. (int -1이 ULONGLONG_MAX로 변환되기 때문) 이런 자동
  // 변환을 피하기 위해, 명백한 경우는 evalBinOpNN을 호출하지 않고 직접
  // 처리한다:

  // 만약 Value가 음수이고 Threshold가 부호 없는 정수라면,
  if (isNegative(SVB, State, Value) && isUnsigned(SVB, Threshold))
  {
    if (CheckEquality) 
    {
      // 음수 값 == unsigned threshold는 항상 false
      return {nullptr, State};
    }
    // 음수 값 < unsigned threshold는 항상 true
    return {State, nullptr};
  }

  // 만약 Value가 부호 없는 정수이고 Threshold가 음수라면,
  if (isUnsigned(SVB, Value) && isNegative(SVB, State, Threshold)) 
  {
    // unsigned 값 == 음수 threshold, unsigned 값 < 음수 threshold 모두 항상
    // false
    return {nullptr, State};
  }
  // FIXME: 이 특수 케이스들은 실제 코드에서 충분히 커버되지만,
  // 이론적으로는 심볼릭 값의 자동 변환 때문에 잘못된 결과가 나올 수도 있다.
  // NOTE: "일반" 케이스에서는 반드시 evalBinOpNN을 써야 한다.
  // 그래야 이 전제조건에서 파생된 제약과 C/C++ 연산자 호출에서 파생된 제약이
  // 동일한 심볼릭 식에 적용된다.
  // 만약 수학적 비교를 별도 경로로 평가하면 오히려 분석의 일관성이 떨어진다.

  // 따라서, 일반적인 경우에는 evalBinOpNN을 사용한다.
  const BinaryOperatorKind OpKind = CheckEquality ? BO_EQ : BO_LT;

  auto BelowThreshold =
      SVB.evalBinOpNN(State, OpKind, Value, Threshold, SVB.getConditionType())
          .getAs<NonLoc>();

  if (BelowThreshold)
    return State->assume(*BelowThreshold);

  return {nullptr, nullptr};
}

// Region에 대해 읽을수 있는 설명을 반환
static std::string getRegionName(const SubRegion *Region)
{
  // 1. 설명적 이름 반환 (예: 변수명)
  if (std::string RegName = Region->getDescriptiveName(); !RegName.empty())
    return RegName;

  // 설명적 이름이 없는 경우
  // 부모가 명확한 이름을 가지지 않는 경우는 getDescriptiveName() 함수가 반환하지 
  // 않기 때문에, 우리가 그것을 지정해줄 필요가 있다.
  // 
 
  // 2. 필드 리젼이라면
  if (const auto *FR = Region->getAs<FieldRegion>())
  {
    if (StringRef Name = FR->getDecl()->getName(); !Name.empty())
      return formatv("the field '{0}'", Name);

    return "the unnamed field";
  }

  // 3. alloca 리젼이라면
  if (isa<AllocaRegion>(Region))
    return "the memory returned by 'alloca'";

  // 4. 심볼릭,힙
  if (isa<SymbolicRegion>(Region) &&
      isa<HeapSpaceRegion>(Region->getMemorySpace()))
    return "the heap area";

  // 5. 문자열 상수
  if (isa<StringRegion>(Region))
    return "the string literal";

  // 그외 
  return "the region";
}

// NonLoc 타입의 SVal에서 구체적인 정수 값을 추출하는 함수
static std::optional<int64_t> getConcreteValue(NonLoc SV)
{
  if (auto ConcreteVal = SV.getAs<nonloc::ConcreteInt>()) 
  {
    return ConcreteVal->getValue().tryExtValue();
  }
  return std::nullopt;
}

static std::optional<int64_t> getConcreteValue(std::optional<NonLoc> SV) 
{
  return SV ? getConcreteValue(*SV) : std::nullopt;
}

/// 이 함수는 Region에 대한 메모리 접근이 범위를 벗어났을 때의
/// 진단 메시지를 생성합니다.
static Messages getPrecedesMsgs(const SubRegion *Region, NonLoc Offset) 
{
  std::string RegName = getRegionName(Region), OffsetStr = "";

  // Offset가 구체적인 정수 값이면, 그 값을 문자열로 변환
  if (auto ConcreteOffset = getConcreteValue(Offset))
    OffsetStr = formatv(" {0}", ConcreteOffset);


  // 숏폼, 롱폼으로 매세지 string 생성
  return {
      formatv("Out of bound access to memory preceding {0}", RegName),
      formatv("Access of {0} at negative byte offset{1}", RegName, OffsetStr)};

}

/// `Val1`과 `Val2`를(제자리에서) `Divisor`로 나눌 수 있는지 시도하며,
/// 나눌 수 있다면 true를 반환한다.
/// (`Divisor`가 0이 아니고 나머지가 없는 경우에만 성공)
/// `Val1`과 `Val2`는 std::nullopt일 수 있으며,
/// 이 경우 해당 값은 항상 나누기에 성공한 것으로 간주한다.
static bool tryDividePair(std::optional<int64_t> &Val1,
                          std::optional<int64_t> &Val2, int64_t Divisor) {
  // Divisor가 0이면 나눌 수 없으므로 false 반환
    if (!Divisor)
    return false;

   // Val1, Val2가 값이 있는 경우, 나머지가 있는지 확인
  const bool Val1HasRemainder = Val1 && *Val1 % Divisor;
  const bool Val2HasRemainder = Val2 && *Val2 % Divisor;

  // 나머지가 없는 경우에만 나누기 수행
  if (!Val1HasRemainder && !Val2HasRemainder)
  {
    if (Val1)
      *Val1 /= Divisor;
    if (Val2)
      *Val2 /= Divisor;
    return true;
  }
  return false;
}

// 배열, 포인터, 구조체 등 메모리 영역에서 경계초과 접근이 발생했을때 
// 진단 메시지를 생성하는 함수입니다.
static Messages getExceedsMsgs(ASTContext &ACtx, const SubRegion *Region,
                               NonLoc Offset, NonLoc Extent, SVal Location,
                               bool AlsoMentionUnderflow) {

 // Region의 (구조적 설명 x) 이름을 가져옵니다.
  std::string RegName = getRegionName(Region);

 // 실제접근 요소로부터 Region의 시작점까지의 오프셋을 계산합니다.
  const auto *EReg = Location.getAsRegion()->getAs<ElementRegion>();
  assert(EReg && "this checker only handles element access");

  // EReg의 요소 타입을 가져옵니다.
  QualType ElemType = EReg->getElementType();

  // Offset와 Extent를 NonLoc에서 구체적인 정수 값으로 변환합니다.
  std::optional<int64_t> OffsetN = getConcreteValue(Offset);
  std::optional<int64_t> ExtentN = getConcreteValue(Extent);

  // 만약 OffsetN, ExtentN가 없다면, 이 함수는 실패할 수 있습니다.
  int64_t ElemSize = ACtx.getTypeSizeInChars(ElemType).getQuantity();


  // OffsetN, ExtentN을 ElemSize로 나누기 시도 (요소 정사이즈 확인)
  // 요소 사이즈에 맞지 않으면 byte offset으로 처리
  bool UseByteOffsets = !tryDividePair(OffsetN, ExtentN, ElemSize);
  const char *OffsetOrIndex = UseByteOffsets ? "byte offset" : "index";

  SmallString<256> Buf;
  llvm::raw_svector_ostream Out(Buf);
  Out << "Access of ";

  // 전체크기가 없고, byte offset이 아닌 경우 (예: 배열 요소 접근)
  if (!ExtentN && !UseByteOffsets)
    Out << "'" << ElemType.getAsString() << "' element in ";

  Out << RegName << " at ";

  // 언더 플로우 가능성이 존재시
  if (AlsoMentionUnderflow) 
  {
    Out << "a negative or overflowing " << OffsetOrIndex;
  }
  else if (OffsetN)// 구체적인 정수값이 존재시
  {
    Out << OffsetOrIndex << " " << *OffsetN;
  } 
  else  // 존재하지 않을시 뭔진 몰라도 경계를 넘었다 표시
  {
    Out << "an overflowing " << OffsetOrIndex;
  }

  // 만약 요소 사이즈가 있다면
  if (ExtentN)
  {
    Out << ", while it holds only ";

    // ExtentN이 1이 아니면, ExtentN을 출력
    if (*ExtentN != 1)
      Out << *ExtentN;
    else // 1이면 요소는 1개 
      Out << "a single";

    // ByteOffset을 쓴다면 
    if (UseByteOffsets)
      Out << " byte";
    else
      Out << " '" << ElemType.getAsString() << "' element";

    // 만약 요소가 여러개라면
    if (*ExtentN > 1)
      Out << "s";
  }

  //e.g Access of {RegName} at a negative or overflowing {OffsetOrIndex},
  //    while it holds only {개수}{단위}
  return {formatv("Out of bound access to memory {0} {1}",
                  AlsoMentionUnderflow ? "around" : "after the end of",
                  RegName),
          std::string(Buf)};
}

// 오염된 접근이 있을때 반환
static Messages getTaintMsgs(const SubRegion *Region, const char *OffsetName,
                             bool AlsoMentionUnderflow) {

  std::string RegName = getRegionName(Region);
  return {formatv("Potential out of bound access to {0} with tainted {1}",
                  RegName, OffsetName),
          formatv("Access of {0} with a tainted {1} that may be {2}too large",
                  RegName, OffsetName,
                  AlsoMentionUnderflow ? "negative or " : "")};
}

//
const NoteTag *StateUpdateReporter::createNoteTag(CheckerContext &C) const {
  // 가정이 없다면 굳이 노트 태그를 만들 필요가 없습니다.
  if (!AssumedNonNegative && !AssumedUpperBound)
    return nullptr;

  // 가정이 있다면
  return C.getNoteTag([*this](PathSensitiveBugReport &BR) -> std::string 
      {
    return getMessage(BR);
  });
}
// 이 함수는 PathSensitiveBugReport BR에 대한 상태 업데이트 가정의 메시지를 생성
// e.g : 가정: 인덱스 '3'는 0 이상이고 5 미만의 범위 내에 있으며,
//       'int' 타입의 요소 개수로 이루어진 arr 영역입니다.
std::string StateUpdateReporter::getMessage(PathSensitiveBugReport &BR) const {

    // 비음수 가정을 쓸지 결정
  bool ShouldReportNonNegative = AssumedNonNegative;

  // 만약 ByteOffsetVal가 흥미로운 심볼에 대한 정보를 제공하지 않는다면
  if (!providesInformationAboutInteresting(ByteOffsetVal, BR))
  {
      // 상한이 존재하고, 상한이 흥미로운 심볼이면
    if (AssumedUpperBound &&
        providesInformationAboutInteresting(*AssumedUpperBound, BR)) 
    {
        // 바이트 오프셋이 흥미롭지 않더라도(예: 일정한 값입니다),
        // 그 가정이 정보를 제공한다면 여전히 흥미로울 수 있습니다
        // 흥미로운 상징적 상한선에 대해.
      ShouldReportNonNegative = false;
    } 
    else
    {
      // We don't have anything interesting, don't report the assumption.
      return "";
    }
  }

  std::optional<int64_t> OffsetN = getConcreteValue(ByteOffsetVal);
  std::optional<int64_t> ExtentN = getConcreteValue(AssumedUpperBound);

  // 요소크기가 존재하고, 나누어 떨어지면(해석가능하면) 인덱스 사용
  const bool UseIndex =
      ElementSize && tryDividePair(OffsetN, ExtentN, *ElementSize);

  SmallString<256> Buf;
  llvm::raw_svector_ostream Out(Buf);
  Out << "Assuming ";

  if (UseIndex) // 인덱스 해석이 가능할때 
  {
    Out << "index ";
    if (OffsetN)
      Out << "'" << OffsetN << "' ";
  } 
  else if (AssumedUpperBound) // 상한만 있을때 (인덱스 x) 
  {
    Out << "byte offset ";
    if (OffsetN)
      Out << "'" << OffsetN << "' ";
  } 
  else 
  {
    Out << "offset ";
  }

  Out << "is";
  if (ShouldReportNonNegative) 
  {
    Out << " non-negative";
  }

  if (AssumedUpperBound) 
  {
    if (ShouldReportNonNegative)
      Out << " and";

    Out << " less than ";

    if (ExtentN)
      Out << *ExtentN << ", ";

    if (UseIndex && ElementType)
      Out << "the number of '" << ElementType->getAsString()
          << "' elements in ";
    else
      Out << "the extent of ";
    Out << getRegionName(Reg);
  }
  return std::string(Out.str());
}

/// 이 함수는 Sym이 PathSensitiveBugReport BR에서 흥미로운 심볼에 대한
/// 제약을 제공할 수 있는지 여부를 판단합니다.
bool StateUpdateReporter::providesInformationAboutInteresting(
    SymbolRef Sym, PathSensitiveBugReport &BR) {

// Sym이 nullptr인 경우, 흥미로운 심볼이 없으므로 false 반환
  if (!Sym)
    return false;

  // Sym이 나타내는 심볼 표현식의 모든 부분 심볼(PartSym)들을 순회합니다.
  // 예를 들어, Sym이 'a + b'와 같은 복합 표현식이라면, 'a'와 'b'가 PartSym이 됩니다
  for (SymbolRef PartSym : Sym->symbols())
  {
    // 상위 심볼이 아니라 부분심볼에서 흥미로움 마크가 붙을수 있음
    if (BR.isInteresting(PartSym))
      return true;

    // 만약 현재 부분 심볼(PartSym)이 SymSymExpr 타입이라면 (예: 'a +
    // b'에서 'a'와 'b'가 모두 심볼인 경우), 이는 두 개의 심볼이 결합된
    // 표현식입니다. 이런 경우, 결합된 결과(a+b)로부터 개별적인 심볼(a, b)에
    // 대한 유의미한 제약 조건을 추출하는 것은 현재 분석 알고리즘으로는 어렵거나
    // 불가능합니다. 따라서 이런 복잡한 심볼에 대해서는 유용한 정보를 제공할 수
    // 없다고 판단하여 false를 반환합니다.
      return false;
  }
  return false;
}

void ArrayBoundCheckerV2::performCheck(const Expr *E, CheckerContext &C) const {

  // 1. 분석 대상 위치(Location) 계산
  const SVal Location = C.getSVal(E);
  // ctype.h 헤더 파일(예: glibc)은 isXXXXX() 매크로들을 다음과 같이 구현합니다:
  //    #define isXXXXX(arg) (LOOKUP_TABLE[arg] & BITMASK_FOR_XXXXX)
  //
  // 이러한 매크로들에 대한 분석이 불완전하면 오탐(false positives)으로
  // 이어집니다. 또한, 설령 정확한 보고라 할지라도 사용자에게 혼란을 줄 수
  // 있으므로, 이 매크로들로부터 발생하는 보고는 비활성화합니다.
  if (isFromCtypeMacro(E, C.getASTContext()))
    return;

  // 3. 현재 분석 상태, SValBuilder 준비
  ProgramStateRef State = C.getState();
  SValBuilder &SVB = C.getSValBuilder();

  // 4. 메모리 영역(Region)과 오프셋(ByteOffset) 계산
  const std::optional<std::pair<const SubRegion *, NonLoc>> &RawOffset =
      computeOffset(State, SVB, Location);

  if (!RawOffset)
    return;

  auto [Reg, ByteOffset] = *RawOffset;


  // 상태 업데이트는 단일 노트 태그로 보고되며
  // 이 도우미 클래스에 의해 구성되었습니다.
  StateUpdateReporter SUR(Reg, ByteOffset, E, C);


  // 6. 하한(Underflow) 검사
  const MemSpaceRegion *Space = Reg->getMemorySpace();
  if (!(isa<SymbolicRegion>(Reg) && isa<UnknownSpaceRegion>(Space))) 
  {
    // 알려지지 않은 공간(unknown space)에 있는 심볼릭 영역(symbolic region)은
    // 배열의 중간을 가리킬 수 있는 알 수 없는 포인터를 나타내므로, 언더플로우를
    // 검사하지 않습니다.
    //
    // 이 두 가지 조건(알려지지 않은 공간 + 심볼릭 영역)은 모두 중요합니다.
    // 왜냐하면 우리는 힙(heap)에 있는 심볼릭 영역(MallocChecker와 같이
    // SValBuilder::getConjuredHeapSymbolVal()을 호출하는 체커가 도입할 수
    // 있는)과 알려지지 않은 공간에 있는 비심볼릭 영역(예: 심볼릭 영역 내의 필드
    // 서브리전)에서는 언더플로우를 검사하고 싶기 때문입니다
    // ByteOffset이 음수인지 확인합니다.
    auto [PrecedesLowerBound, WithinLowerBound] = compareValueToThreshold(
        State, ByteOffset, SVB.makeZeroArrayIndex(), SVB);

    if (PrecedesLowerBound) // 음수 오프셋 가능성이 존재하고
    {
      // 오프셋이 유효하지 않을 수 있습니다(음수)...
      if (!WithinLowerBound) // 양수일 가능성이 없다면
      {
        // 해당 값이 (>= 0)이 될 수 없기 때문에, 오류
        Messages Msgs = getPrecedesMsgs(Reg, ByteOffset);
        reportOOB(C, PrecedesLowerBound, Msgs, ByteOffset, std::nullopt);
        return;
      }
      // ...하지만 유효한 경우도 있을 수 있으므로,
      // 체커는 (낙관적으로) 유효하다고 가정하고 분석을 진행하며,
      // 이 가정을 노트(note) 태그에 언급할 것입니다.
      SUR.recordNonNegativeAssumption();
    }

    // 실제로 상태를 업데이트합니다. 이 'if' 조건문이 실패하는 경우는
    // compareValueToThreshold 함수가 {nullptr, nullptr}를 반환하는
    // 극히 드문 경우뿐입니다. 이는 evalBinOpNN 함수가 '보다 작음' 연산자를
    // 평가하는 데 실패했기 때문입니다.
    if (WithinLowerBound)
      State = WithinLowerBound;
  }

  // 7. 상한(Upper Bound) 검사
  // Reg의 사이즈 검사
  DefinedOrUnknownSVal Size = getDynamicExtent(State, Reg, SVB);
  
  if (auto KnownSize = Size.getAs<NonLoc>()) {
    // 언더플로우와 오버플로우가 모두 가능한 상황(단, 인덱스가 오염되었거나
    // 유효하지 않은 것으로 알려진 경우)에서, 이 체커의 로직은 먼저 오프셋이
    // 음수가 아니라고 가정하고(즉, 0 이상이라고 가정하고) (이 추가적인
    // 가정하에) 오버플로우 오류를 탐지할 것입니다. 이런 상황에서는 경고
    // 메시지가 두 가지 가능성(언더플로우와 오버플로우)을 모두 언급해야 합니다.

      // 인덱스가 0이상이라 가정한적이 있는지 확인
    bool AlsoMentionUnderflow = SUR.assumedNonNegative();

    // ByteOffset과 KnownSize를 비교합니다.
    auto [WithinUpperBound, ExceedsUpperBound] =
        compareValueToThreshold(State, ByteOffset, *KnownSize, SVB);

    if (ExceedsUpperBound) // 상한 초과 가능성 존재하고
    {
      if (!WithinUpperBound) // 상한 미만 가능성이 없다면 
      {
        // ...그리고 범위 내에 있을 수 없으므로(즉, 유효하지 않으므로) 오류를
        // 보고해야 합니다. 다만, 이 표현식이 배열의 끝 바로 다음을 가리키는
        // 포인터(past-the-end pointer)를 계산하는 관용적인 `&array[size]`
        // 표현식임이 확실하게 판단될 경우에는 오류를 보고하지 않습니다.
        if (isIdiomaticPastTheEndPtr(E, ExceedsUpperBound, ByteOffset,
                                     *KnownSize, C)) 
        {
          C.addTransition(ExceedsUpperBound, SUR.createNoteTag(C));
          return;
        }
        // 일반적인 상한 초과시는 오류리포트
        Messages Msgs =
            getExceedsMsgs(C.getASTContext(), Reg, ByteOffset, *KnownSize,
                           Location, AlsoMentionUnderflow);
        reportOOB(C, ExceedsUpperBound, Msgs, ByteOffset, KnownSize);
        return;
      }
      
     // 상한 초과 가능성은 있지만, 범위 내에 있을 수 있는 경우입니다.
      if (isTainted(State, ByteOffset)) 
      {
        // ...하지만 해당 값이 오염되었으므로(tainted), 오류를 보고합니다.

        // 진단 상세: "오염된 오프셋(tainted offset)"이라고 표현하는 것이 항상
        // 정확하지만, 일반적인 경우 `arr[idx]`와 같은 표현식에서 'idx'가 오염된
        // 것이므로, 이때는 "오염된 인덱스(tainted index)"라고 표현하는 것이
        // 사용자에게 더 좋습니다.
        const char *OffsetName = "offset";
        if (const auto *ASE = dyn_cast<ArraySubscriptExpr>(E))
          if (isTainted(State, ASE->getIdx(), C.getLocationContext()))
            OffsetName = "index";

        Messages Msgs = getTaintMsgs(Reg, OffsetName, AlsoMentionUnderflow);
        reportOOB(C, ExceedsUpperBound, Msgs, ByteOffset, KnownSize,
                  /*IsTaintBug=*/true);
        return;
      }
      //"...그리고(또는) 이 값이 오염되지 않았으므로, 분석기는 (낙관적으로)
      // 이 오프셋이 정상 범위 내에 있다고 가정하며,
      // 이 가정 내용을 노트(설명 메시지)로 함께 남깁니다."
      SUR.recordUpperBoundAssumption(*KnownSize);
    }

    // 상한 초과 가능성이 존재하지 않고, 범위 내에 있을 수 있는 경우입니다.
    // 실제로 프로그램 상태를 업데이트합니다. 이 'if' 문이 실패하는 경우는
    // compareValueToThreshold 함수가 {nullptr, nullptr}을 반환하는
    // 극히 드문 상황뿐입니다. 이는 evalBinOpNN 함수가 '보다 작음' 연산자 평가에
    // 실패했기 때문입니다.
    if (WithinUpperBound)
      State = WithinUpperBound;
  }

  // Add a transition, reporting the state updates that we accumulated.
  C.addTransition(State, SUR.createNoteTag(C));
}

//  "interesting" 심볼(분석상 주목해야 할 변수/식)을 표시
void ArrayBoundCheckerV2::markPartsInteresting(PathSensitiveBugReport &BR, // 버그 보고서 객체
                                                ProgramStateRef ErrorState, // 오류가 발생한 프로그램 상태
                                                NonLoc Val,               // 오류와 관련된 심볼릭 값 (오프셋 등)
                                                bool MarkTaint) {         // 오염된 심볼도 표시할지 여부

  // Val이 심볼릭 값인 경우, 그 심볼의 '부분들'을 흥미로운 것으로 표시합니다.
  if (SymbolRef Sym = Val.getAsSymbol())
  {
    // 오프셋이 심볼릭 값이라면, SymExpr::symbols()를 사용하여 그 '부분들'을 반복하고
    // 각 부분을 흥미로운 것으로 표시합니다.
    // 예를 들어, 오프셋이 'x*4 + y'라면, SymSymExpr 'x*4 + y', SymIntExpr 'x*4'와
    // 두 데이터 심볼 'x'와 'y'에 흥미로움 표시를 합니다.
    for (SymbolRef PartSym : Sym->symbols())
      BR.markInteresting(PartSym);
  }

  // MarkTaint가 true인 경우 (오염된 심볼도 표시해야 하는 경우)
  if (MarkTaint) 
  {
    // 보고하려는 문제가 오프셋의 오염 상태에 의존한다면,
    // 오염의 원인이 될 수 있는 심볼들도 흥미로운 것으로 표시합니다.
    // 참고: 이 과정에서 Sym->symbols()에 나타나지 않은 심볼(Val과 느슨하게 연결된 심볼)도
    // 찾을 수 있습니다.
    for (SymbolRef Sym : getTaintedSymbols(ErrorState, Val))
      BR.markInteresting(Sym);
  }
}

// 배열/포인터 경계 초과(Out-of-Bounds, OOB) 접근이 탐지됐을 때 
// 실제로 버그 리포트(PathSensitiveBugReport)를 생성하고,
// 관련 정보를 마킹한 뒤, 분석기(CheckerContext)에 리포트를 제출
void ArrayBoundCheckerV2::reportOOB(CheckerContext &C,
                                    ProgramStateRef ErrorState, Messages Msgs,
                                    NonLoc Offset, std::optional<NonLoc> Extent,
                                    bool IsTaintBug /*=false*/) const {

    // 에러 노드 생성
  ExplodedNode *ErrorNode = C.generateErrorNode(ErrorState);
  if (!ErrorNode)
    return;

  // 버그 보고서 객체를 생성합니다.
  // IsTaintBug 값에 따라 'TaintBT'(오염 버그 타입) 또는 'BT'(기본 버그 타입) 중
  // 하나를 사용합니다. Msgs.Short는 짧은 요약 메시지, Msgs.Full은 자세한 설명
  // 메시지입니다.
  auto BR = std::make_unique<PathSensitiveBugReport>(
      IsTaintBug ? TaintBT : BT, Msgs.Short, Msgs.Full, ErrorNode);

  // FIXME: 이상적으로는 trackExpressionValue() 함수를 호출하는 것만으로
  // 충분해야 합니다.
  //        이 함수가 '올바른 일'을 처리해야 합니다. 즉, 관련 심볼을 흥미로운
  //        것으로 표시하고, 제어 의존성을 추적하며, 관련 값을 저장하는 구문들을
  //        추적하고, 유용한 진단 정보를 추가해야 합니다. 하지만 현재
  //        trackExpressionValue()는 신뢰할 수 없는 휴리스틱(경험적 방법)들의
  //        덩어리여서 여러 가지 문제를 야기합니다:
  //        - '흥미로움' 표시가 일관성 있게 적용되지 않습니다.
  //          예를 들어, 'array[x+10]'이 오버플로우를 일으켜도 'x'는 흥미로운
  //          것으로 표시되지 않습니다.
  //        - 관련 없는 진단 정보가 추가됩니다.
  //          예를 들어, 'int *p = (int*)malloc(2*sizeof(int)); p[3] = 0;'
  //          코드에서 malloc 호출에 "초기화되지 않은 값 저장"이라는 노트가
  //          붙습니다. (기술적으로는 맞지만, 현재 버그와는 관련이 없습니다.)
  // trackExpressionValue()가 신뢰할 수 있게 된다면, 이 커스텀
  // markPartsInteresting() 대신 해당 함수가 적용되어야 합니다.
  markPartsInteresting(*BR, ErrorState, Offset, IsTaintBug);
  if (Extent)
    markPartsInteresting(*BR, ErrorState, *Extent, IsTaintBug);

  C.emitReport(std::move(BR));
}

bool ArrayBoundCheckerV2::isFromCtypeMacro(const Stmt *S, ASTContext &ACtx) {
  SourceLocation Loc = S->getBeginLoc();
  if (!Loc.isMacroID())
    return false;

  StringRef MacroName = Lexer::getImmediateMacroName(
      Loc, ACtx.getSourceManager(), ACtx.getLangOpts());

  if (MacroName.size() < 7 || MacroName[0] != 'i' || MacroName[1] != 's')
    return false;

  return ((MacroName == "isalnum") || (MacroName == "isalpha") ||
          (MacroName == "isblank") || (MacroName == "isdigit") ||
          (MacroName == "isgraph") || (MacroName == "islower") ||
          (MacroName == "isnctrl") || (MacroName == "isprint") ||
          (MacroName == "ispunct") || (MacroName == "isspace") ||
          (MacroName == "isupper") || (MacroName == "isxdigit"));
}

bool ArrayBoundCheckerV2::isInAddressOf(const Stmt *S, ASTContext &ACtx) {
  
    ParentMapContext &ParentCtx = ACtx.getParentMapContext();
 
    do 
    {
        const DynTypedNodeList Parents = ParentCtx.getParents(*S);

    if (Parents.empty()) // 현재 노드의 부모가 없다면 false
      return false;

    S = Parents[0].get<Stmt>();

  } while (isa_and_nonnull<ParenExpr, ImplicitCastExpr>(S));

    // 이 시점에서 괄호나 암시적 형변환이  제거됨
    // 단항 연산자로 변환시도
  const auto *UnaryOp = dyn_cast_or_null<UnaryOperator>(S);
  // 만약 단항 연산자이고, 주소 연산자라면 true
  return UnaryOp && UnaryOp->getOpcode() == UO_AddrOf;
}

bool ArrayBoundCheckerV2::isIdiomaticPastTheEndPtr(const Expr *E,
                                                   ProgramStateRef State,
                                                   NonLoc Offset, NonLoc Limit,
                                                   CheckerContext &C) {
    // 배열첨자식이고, 주소연산자면
  if (isa<ArraySubscriptExpr>(E) && isInAddressOf(E, C.getASTContext())) 
  {

    auto [EqualsToThreshold, NotEqualToThreshold] = compareValueToThreshold(
        State, Offset, Limit, C.getSValBuilder(), /*CheckEquality=*/true);
    return EqualsToThreshold && !NotEqualToThreshold;
  }
  return false;
}

void ento::registerArrayBoundCheckerV2(CheckerManager &mgr) {
  mgr.registerChecker<ArrayBoundCheckerV2>();
}

bool ento::shouldRegisterArrayBoundCheckerV2(const CheckerManager &mgr) {
  return true;
}

```