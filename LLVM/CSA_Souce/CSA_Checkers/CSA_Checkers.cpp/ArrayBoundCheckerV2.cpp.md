
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


static std::string getRegionName(const SubRegion *Region) {
  if (std::string RegName = Region->getDescriptiveName(); !RegName.empty())
    return RegName;

  // Field regions only have descriptive names when their parent has a
  // descriptive name; so we provide a fallback representation for them:
  if (const auto *FR = Region->getAs<FieldRegion>()) {
    if (StringRef Name = FR->getDecl()->getName(); !Name.empty())
      return formatv("the field '{0}'", Name);
    return "the unnamed field";
  }

  if (isa<AllocaRegion>(Region))
    return "the memory returned by 'alloca'";

  if (isa<SymbolicRegion>(Region) &&
      isa<HeapSpaceRegion>(Region->getMemorySpace()))
    return "the heap area";

  if (isa<StringRegion>(Region))
    return "the string literal";

  return "the region";
}

static std::optional<int64_t> getConcreteValue(NonLoc SV) {
  if (auto ConcreteVal = SV.getAs<nonloc::ConcreteInt>()) {
    return ConcreteVal->getValue().tryExtValue();
  }
  return std::nullopt;
}

static std::optional<int64_t> getConcreteValue(std::optional<NonLoc> SV) {
  return SV ? getConcreteValue(*SV) : std::nullopt;
}

static Messages getPrecedesMsgs(const SubRegion *Region, NonLoc Offset) {
  std::string RegName = getRegionName(Region), OffsetStr = "";

  if (auto ConcreteOffset = getConcreteValue(Offset))
    OffsetStr = formatv(" {0}", ConcreteOffset);

  return {
      formatv("Out of bound access to memory preceding {0}", RegName),
      formatv("Access of {0} at negative byte offset{1}", RegName, OffsetStr)};
}

/// Try to divide `Val1` and `Val2` (in place) by `Divisor` and return true if
/// it can be performed (`Divisor` is nonzero and there is no remainder). The
/// values `Val1` and `Val2` may be nullopt and in that case the corresponding
/// division is considered to be successful.
static bool tryDividePair(std::optional<int64_t> &Val1,
                          std::optional<int64_t> &Val2, int64_t Divisor) {
  if (!Divisor)
    return false;
  const bool Val1HasRemainder = Val1 && *Val1 % Divisor;
  const bool Val2HasRemainder = Val2 && *Val2 % Divisor;
  if (!Val1HasRemainder && !Val2HasRemainder) {
    if (Val1)
      *Val1 /= Divisor;
    if (Val2)
      *Val2 /= Divisor;
    return true;
  }
  return false;
}

static Messages getExceedsMsgs(ASTContext &ACtx, const SubRegion *Region,
                               NonLoc Offset, NonLoc Extent, SVal Location,
                               bool AlsoMentionUnderflow) {
  std::string RegName = getRegionName(Region);
  const auto *EReg = Location.getAsRegion()->getAs<ElementRegion>();
  assert(EReg && "this checker only handles element access");
  QualType ElemType = EReg->getElementType();

  std::optional<int64_t> OffsetN = getConcreteValue(Offset);
  std::optional<int64_t> ExtentN = getConcreteValue(Extent);

  int64_t ElemSize = ACtx.getTypeSizeInChars(ElemType).getQuantity();

  bool UseByteOffsets = !tryDividePair(OffsetN, ExtentN, ElemSize);
  const char *OffsetOrIndex = UseByteOffsets ? "byte offset" : "index";

  SmallString<256> Buf;
  llvm::raw_svector_ostream Out(Buf);
  Out << "Access of ";
  if (!ExtentN && !UseByteOffsets)
    Out << "'" << ElemType.getAsString() << "' element in ";
  Out << RegName << " at ";
  if (AlsoMentionUnderflow) {
    Out << "a negative or overflowing " << OffsetOrIndex;
  } else if (OffsetN) {
    Out << OffsetOrIndex << " " << *OffsetN;
  } else {
    Out << "an overflowing " << OffsetOrIndex;
  }
  if (ExtentN) {
    Out << ", while it holds only ";
    if (*ExtentN != 1)
      Out << *ExtentN;
    else
      Out << "a single";
    if (UseByteOffsets)
      Out << " byte";
    else
      Out << " '" << ElemType.getAsString() << "' element";

    if (*ExtentN > 1)
      Out << "s";
  }

  return {formatv("Out of bound access to memory {0} {1}",
                  AlsoMentionUnderflow ? "around" : "after the end of",
                  RegName),
          std::string(Buf)};
}

static Messages getTaintMsgs(const SubRegion *Region, const char *OffsetName,
                             bool AlsoMentionUnderflow) {
  std::string RegName = getRegionName(Region);
  return {formatv("Potential out of bound access to {0} with tainted {1}",
                  RegName, OffsetName),
          formatv("Access of {0} with a tainted {1} that may be {2}too large",
                  RegName, OffsetName,
                  AlsoMentionUnderflow ? "negative or " : "")};
}

const NoteTag *StateUpdateReporter::createNoteTag(CheckerContext &C) const {
  // Don't create a note tag if we didn't assume anything:
  if (!AssumedNonNegative && !AssumedUpperBound)
    return nullptr;

  return C.getNoteTag([*this](PathSensitiveBugReport &BR) -> std::string {
    return getMessage(BR);
  });
}

std::string StateUpdateReporter::getMessage(PathSensitiveBugReport &BR) const {
  bool ShouldReportNonNegative = AssumedNonNegative;
  if (!providesInformationAboutInteresting(ByteOffsetVal, BR)) {
    if (AssumedUpperBound &&
        providesInformationAboutInteresting(*AssumedUpperBound, BR)) {
      // Even if the byte offset isn't interesting (e.g. it's a constant value),
      // the assumption can still be interesting if it provides information
      // about an interesting symbolic upper bound.
      ShouldReportNonNegative = false;
    } else {
      // We don't have anything interesting, don't report the assumption.
      return "";
    }
  }

  std::optional<int64_t> OffsetN = getConcreteValue(ByteOffsetVal);
  std::optional<int64_t> ExtentN = getConcreteValue(AssumedUpperBound);

  const bool UseIndex =
      ElementSize && tryDividePair(OffsetN, ExtentN, *ElementSize);

  SmallString<256> Buf;
  llvm::raw_svector_ostream Out(Buf);
  Out << "Assuming ";
  if (UseIndex) {
    Out << "index ";
    if (OffsetN)
      Out << "'" << OffsetN << "' ";
  } else if (AssumedUpperBound) {
    Out << "byte offset ";
    if (OffsetN)
      Out << "'" << OffsetN << "' ";
  } else {
    Out << "offset ";
  }

  Out << "is";
  if (ShouldReportNonNegative) {
    Out << " non-negative";
  }
  if (AssumedUpperBound) {
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

bool StateUpdateReporter::providesInformationAboutInteresting(
    SymbolRef Sym, PathSensitiveBugReport &BR) {
  if (!Sym)
    return false;
  for (SymbolRef PartSym : Sym->symbols()) {
    // The interestingess mark may appear on any layer as we're stripping off
    // the SymIntExpr, UnarySymExpr etc. layers...
    if (BR.isInteresting(PartSym))
      return true;
    // ...but if both sides of the expression are symbolic, then there is no
    // practical algorithm to produce separate constraints for the two
    // operands (from the single combined result).
    if (isa<SymSymExpr>(PartSym))
      return false;
  }
  return false;
}

void ArrayBoundCheckerV2::performCheck(const Expr *E, CheckerContext &C) const {
  const SVal Location = C.getSVal(E);

  // The header ctype.h (from e.g. glibc) implements the isXXXXX() macros as
  //   #define isXXXXX(arg) (LOOKUP_TABLE[arg] & BITMASK_FOR_XXXXX)
  // and incomplete analysis of these leads to false positives. As even
  // accurate reports would be confusing for the users, just disable reports
  // from these macros:
  if (isFromCtypeMacro(E, C.getASTContext()))
    return;

  ProgramStateRef State = C.getState();
  SValBuilder &SVB = C.getSValBuilder();

  const std::optional<std::pair<const SubRegion *, NonLoc>> &RawOffset =
      computeOffset(State, SVB, Location);

  if (!RawOffset)
    return;

  auto [Reg, ByteOffset] = *RawOffset;

  // The state updates will be reported as a single note tag, which will be
  // composed by this helper class.
  StateUpdateReporter SUR(Reg, ByteOffset, E, C);

  // CHECK LOWER BOUND
  const MemSpaceRegion *Space = Reg->getMemorySpace();
  if (!(isa<SymbolicRegion>(Reg) && isa<UnknownSpaceRegion>(Space))) {
    // A symbolic region in unknown space represents an unknown pointer that
    // may point into the middle of an array, so we don't look for underflows.
    // Both conditions are significant because we want to check underflows in
    // symbolic regions on the heap (which may be introduced by checkers like
    // MallocChecker that call SValBuilder::getConjuredHeapSymbolVal()) and
    // non-symbolic regions (e.g. a field subregion of a symbolic region) in
    // unknown space.
    auto [PrecedesLowerBound, WithinLowerBound] = compareValueToThreshold(
        State, ByteOffset, SVB.makeZeroArrayIndex(), SVB);

    if (PrecedesLowerBound) {
      // The offset may be invalid (negative)...
      if (!WithinLowerBound) {
        // ...and it cannot be valid (>= 0), so report an error.
        Messages Msgs = getPrecedesMsgs(Reg, ByteOffset);
        reportOOB(C, PrecedesLowerBound, Msgs, ByteOffset, std::nullopt);
        return;
      }
      // ...but it can be valid as well, so the checker will (optimistically)
      // assume that it's valid and mention this in the note tag.
      SUR.recordNonNegativeAssumption();
    }

    // Actually update the state. The "if" only fails in the extremely unlikely
    // case when compareValueToThreshold returns {nullptr, nullptr} becasue
    // evalBinOpNN fails to evaluate the less-than operator.
    if (WithinLowerBound)
      State = WithinLowerBound;
  }

  // CHECK UPPER BOUND
  DefinedOrUnknownSVal Size = getDynamicExtent(State, Reg, SVB);
  if (auto KnownSize = Size.getAs<NonLoc>()) {
    // In a situation where both underflow and overflow are possible (but the
    // index is either tainted or known to be invalid), the logic of this
    // checker will first assume that the offset is non-negative, and then
    // (with this additional assumption) it will detect an overflow error.
    // In this situation the warning message should mention both possibilities.
    bool AlsoMentionUnderflow = SUR.assumedNonNegative();

    auto [WithinUpperBound, ExceedsUpperBound] =
        compareValueToThreshold(State, ByteOffset, *KnownSize, SVB);

    if (ExceedsUpperBound) {
      // The offset may be invalid (>= Size)...
      if (!WithinUpperBound) {
        // ...and it cannot be within bounds, so report an error, unless we can
        // definitely determine that this is an idiomatic `&array[size]`
        // expression that calculates the past-the-end pointer.
        if (isIdiomaticPastTheEndPtr(E, ExceedsUpperBound, ByteOffset,
                                     *KnownSize, C)) {
          C.addTransition(ExceedsUpperBound, SUR.createNoteTag(C));
          return;
        }

        Messages Msgs =
            getExceedsMsgs(C.getASTContext(), Reg, ByteOffset, *KnownSize,
                           Location, AlsoMentionUnderflow);
        reportOOB(C, ExceedsUpperBound, Msgs, ByteOffset, KnownSize);
        return;
      }
      // ...and it can be valid as well...
      if (isTainted(State, ByteOffset)) {
        // ...but it's tainted, so report an error.

        // Diagnostic detail: saying "tainted offset" is always correct, but
        // the common case is that 'idx' is tainted in 'arr[idx]' and then it's
        // nicer to say "tainted index".
        const char *OffsetName = "offset";
        if (const auto *ASE = dyn_cast<ArraySubscriptExpr>(E))
          if (isTainted(State, ASE->getIdx(), C.getLocationContext()))
            OffsetName = "index";

        Messages Msgs = getTaintMsgs(Reg, OffsetName, AlsoMentionUnderflow);
        reportOOB(C, ExceedsUpperBound, Msgs, ByteOffset, KnownSize,
                  /*IsTaintBug=*/true);
        return;
      }
      // ...and it isn't tainted, so the checker will (optimistically) assume
      // that the offset is in bounds and mention this in the note tag.
      SUR.recordUpperBoundAssumption(*KnownSize);
    }

    // Actually update the state. The "if" only fails in the extremely unlikely
    // case when compareValueToThreshold returns {nullptr, nullptr} becasue
    // evalBinOpNN fails to evaluate the less-than operator.
    if (WithinUpperBound)
      State = WithinUpperBound;
  }

  // Add a transition, reporting the state updates that we accumulated.
  C.addTransition(State, SUR.createNoteTag(C));
}

void ArrayBoundCheckerV2::markPartsInteresting(PathSensitiveBugReport &BR,
                                               ProgramStateRef ErrorState,
                                               NonLoc Val, bool MarkTaint) {
  if (SymbolRef Sym = Val.getAsSymbol()) {
    // If the offset is a symbolic value, iterate over its "parts" with
    // `SymExpr::symbols()` and mark each of them as interesting.
    // For example, if the offset is `x*4 + y` then we put interestingness onto
    // the SymSymExpr `x*4 + y`, the SymIntExpr `x*4` and the two data symbols
    // `x` and `y`.
    for (SymbolRef PartSym : Sym->symbols())
      BR.markInteresting(PartSym);
  }

  if (MarkTaint) {
    // If the issue that we're reporting depends on the taintedness of the
    // offset, then put interestingness onto symbols that could be the origin
    // of the taint. Note that this may find symbols that did not appear in
    // `Sym->symbols()` (because they're only loosely connected to `Val`).
    for (SymbolRef Sym : getTaintedSymbols(ErrorState, Val))
      BR.markInteresting(Sym);
  }
}

void ArrayBoundCheckerV2::reportOOB(CheckerContext &C,
                                    ProgramStateRef ErrorState, Messages Msgs,
                                    NonLoc Offset, std::optional<NonLoc> Extent,
                                    bool IsTaintBug /*=false*/) const {

  ExplodedNode *ErrorNode = C.generateErrorNode(ErrorState);
  if (!ErrorNode)
    return;

  auto BR = std::make_unique<PathSensitiveBugReport>(
      IsTaintBug ? TaintBT : BT, Msgs.Short, Msgs.Full, ErrorNode);

  // FIXME: ideally we would just call trackExpressionValue() and that would
  // "do the right thing": mark the relevant symbols as interesting, track the
  // control dependencies and statements storing the relevant values and add
  // helpful diagnostic pieces. However, right now trackExpressionValue() is
  // a heap of unreliable heuristics, so it would cause several issues:
  // - Interestingness is not applied consistently, e.g. if `array[x+10]`
  //   causes an overflow, then `x` is not marked as interesting.
  // - We get irrelevant diagnostic pieces, e.g. in the code
  //   `int *p = (int*)malloc(2*sizeof(int)); p[3] = 0;`
  //   it places a "Storing uninitialized value" note on the `malloc` call
  //   (which is technically true, but irrelevant).
  // If trackExpressionValue() becomes reliable, it should be applied instead
  // of this custom markPartsInteresting().
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
  do {
    const DynTypedNodeList Parents = ParentCtx.getParents(*S);
    if (Parents.empty())
      return false;
    S = Parents[0].get<Stmt>();
  } while (isa_and_nonnull<ParenExpr, ImplicitCastExpr>(S));
  const auto *UnaryOp = dyn_cast_or_null<UnaryOperator>(S);
  return UnaryOp && UnaryOp->getOpcode() == UO_AddrOf;
}

bool ArrayBoundCheckerV2::isIdiomaticPastTheEndPtr(const Expr *E,
                                                   ProgramStateRef State,
                                                   NonLoc Offset, NonLoc Limit,
                                                   CheckerContext &C) {
  if (isa<ArraySubscriptExpr>(E) && isInAddressOf(E, C.getASTContext())) {
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