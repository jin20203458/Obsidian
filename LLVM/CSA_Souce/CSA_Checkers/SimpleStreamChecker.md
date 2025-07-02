
```cpp
//===-- SimpleStreamChecker.cpp -----------------------------------*- C++ -*--//
//
// 이 파일은 Clang Static Analyzer에 포함되는 커스텀 체커입니다.
// fopen/fclose 함수의 올바른 사용을 감지하여
// - 리소스 누수 (fclose 하지 않은 경우)
// - 이중 fclose (이미 닫은 파일을 다시 닫으려는 경우)
// 를 체크합니다.
//
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "clang/StaticAnalyzer/Checkers/BuiltinCheckerRegistration.h" // 체커 등록용 매크로
#include "clang/StaticAnalyzer/Core/BugReporter/BugType.h" // 버그 리포트 생성용
#include "clang/StaticAnalyzer/Core/Checker.h"             // Checker 클래스
#include "clang/StaticAnalyzer/Core/PathSensitive/CallDescription.h" // 함수 호출 설명
#include "clang/StaticAnalyzer/Core/PathSensitive/CallEvent.h" // 함수 호출 이벤트
#include "clang/StaticAnalyzer/Core/PathSensitive/CheckerContext.h" // 분석 상태 및 인터페이스
#include <utility> // std::move 등 유틸리티 함수

using namespace clang; // Clang 컴파일러의 핵심 기능이 정의된 네임스페이스
                       // (e.g., AST, Sema, Lex 등)
using namespace ento;  // Clang Static Analyzer 엔진 관련 기능이 정의된
                       // 네임스페이스 (e.g., Checker, ProgramState, SymbolRef
                       // 등)

namespace {
typedef SmallVector<SymbolRef, 2> SymbolVector;
// 심볼 벡터 타입 정의(2개의 심볼을 저장할 수 있는 벡터)

struct StreamState {
private:
  enum Kind { Opened, Closed } K; // 열림, 닫힘 상태 구분
  StreamState(Kind InK) : K(InK) {}

public:
  bool isOpened() const { return K == Opened; }
  bool isClosed() const { return K == Closed; }

  static StreamState getOpened() { return StreamState(Opened); }
  static StreamState getClosed() { return StreamState(Closed); }

  bool operator==(const StreamState &X) const { return K == X.K; }
  void Profile(llvm::FoldingSetNodeID &ID) const { ID.AddInteger(K); }
};

class SimpleStreamChecker
    : public Checker<check::PostCall,     // fopen 호출 후 처리
                     check::PreCall,      // fclose 호출 전 처리
                     check::DeadSymbols,  // 심볼이 죽었을 때
                     check::PointerEscape // 포인터가 범위를 벗어날 때
                     > {
  const CallDescription OpenFn{CDM::CLibrary, {"fopen"}, 2};
  const CallDescription CloseFn{CDM::CLibrary, {"fclose"}, 1};

  const BugType DoubleCloseBugType{this, "Double fclose",
                                   "Unix Stream API Error"};
  const BugType LeakBugType{this, "Resource Leak", "Unix Stream API Error",
                            /*SuppressOnSink=*/true};

  void reportDoubleClose(SymbolRef FileDescSym, const CallEvent &Call,
                         CheckerContext &C) const;

  void reportLeaks(ArrayRef<SymbolRef> LeakedStreams, CheckerContext &C,
                   ExplodedNode *ErrNode) const;

  bool guaranteedNotToCloseFile(const CallEvent &Call) const;

public:
  /// Process fopen.
  void checkPostCall(const CallEvent &Call, CheckerContext &C) const;
  /// Process fclose.
  void checkPreCall(const CallEvent &Call, CheckerContext &C) const;

  void checkDeadSymbols(SymbolReaper &SymReaper, CheckerContext &C) const;

  /// Stop tracking addresses which escape.
  ProgramStateRef checkPointerEscape(ProgramStateRef State,
                                     const InvalidatedSymbols &Escaped,
                                     const CallEvent *Call,
                                     PointerEscapeKind Kind) const;
};

} // end anonymous namespace

// SymbolRef (파일 포인터) → StreamState (Opened/Closed) 로 저장됨
// ProgramState에 StreamMap을 등록합니다.
REGISTER_MAP_WITH_PROGRAMSTATE(StreamMap, SymbolRef, StreamState)


// 함수 호출 후 처리 : fopen 호출이 성공적으로 이루어진 경우
void SimpleStreamChecker::checkPostCall(const CallEvent &Call,
                                        CheckerContext &C) const {

  // 1. 기존 정의한 OpenFn과 일치하는지 확인
  if (!OpenFn.matches(Call))
    return;

  //  2. 함수의 반환 값을 "심볼(Symbol)"로 가져옵니다.
  SymbolRef FileDesc = Call.getReturnValue().getAsSymbol();
  if (!FileDesc)
    return;

  // 3. 현재 ProgramState를 기반으로 새로운 상태를 생성합니다.
  ProgramStateRef State = C.getState();
  State = State->set<StreamMap>(FileDesc, StreamState::getOpened());

  // 4. 상태 변화를 분석기에 알려 ExplodedGraph에 새 노드를 추가합니다.
  C.addTransition(State);
}


// 함수 호출 전 처리 : fclose 호출이 이루어지기 전에
void SimpleStreamChecker::checkPreCall(const CallEvent &Call,
                                       CheckerContext &C) const {

  // 1. fclose 함수 호출인지 확인합니다.
  if (!CloseFn.matches(Call))
    return;

  // 2. fclose의 첫 번째 인자(파일 포인터)를 가져옵니다. 
  SymbolRef FileDesc = Call.getArgSVal(0).getAsSymbol();
  if (!FileDesc)
    return;


  // 3. 이전 StreamState가 isClosed()였고, 현재도 fclose이기에 doubleClose
  ProgramStateRef State = C.getState();
  const StreamState *SS = State->get<StreamMap>(FileDesc);
  if (SS && SS->isClosed()) 
  {
    reportDoubleClose(FileDesc, Call, C);
    return;
  }

  // 4. 현재상태를 업데이트하고, 현재상태를 기반으로 다음전이 생성
  State = State->set<StreamMap>(FileDesc, StreamState::getClosed());
  C.addTransition(State);
}

static bool isLeaked(SymbolRef Sym, const StreamState &SS, bool IsSymDead,
                     ProgramStateRef State) {

    // 심볼이 죽고, StreamState가 Opened 상태인 경우 판별
    // 즉,fopen후 fclose가 호출되지 않고 스코프가 종료된 경우등
  if (IsSymDead && SS.isOpened())
  {
  
    // 단 fopen이 실패한 경우는 제외(예: fopen이 NULL을 반환한 경우)
    ConstraintManager &CMgr = State->getConstraintManager();
    ConditionTruthVal OpenFailed = CMgr.isNull(State, Sym);
    return !OpenFailed.isConstrainedTrue();
  }
  return false;
}

// 심볼이 죽었을 때 호출되는 함수
void SimpleStreamChecker::checkDeadSymbols(SymbolReaper &SymReaper,
                                           CheckerContext &C) const {

  // 1. 현재 분석 경로에서의 상태 정보를 가져온다
  ProgramStateRef State = C.getState();
  // 누수가 의심되는 심볼을 저장할 벡터
  SymbolVector LeakedStreams;
  // 상태에서 스트림을 추적하는 맵을 가져온다 (파일 디스크립터 등 리소스를 관리)
  // 인자가 없는 경우 맵 전체를 가져오는것
  StreamMapTy TrackedStreams = State->get<StreamMap>();

  // 2. 모든 스트림을 순회하면서 누수가 발생했는지 확인
  for (auto [Sym, StreamStatus] : TrackedStreams) 
  {

    // 심볼이 죽었는지 확인
    bool IsSymDead = SymReaper.isDead(Sym);

    // 누수가 발생했지는지 확인, 발생시 LeakedStreams에 추가
    if (isLeaked(Sym, StreamStatus, IsSymDead, State))
      LeakedStreams.push_back(Sym);

    // 죽은 심볼은 StreamMap에서 제거(고려하지 않음)
    if (IsSymDead)
      State = State->remove<StreamMap>(Sym);
  }

  // 에러 테그를 붙히고 다음지점으로 이동
  ExplodedNode *N = C.generateNonFatalErrorNode(State);
  if (!N)
    return;
  reportLeaks(LeakedStreams, C, N);
}

// 이중 fclose를 감지하고 리포트하는 함수
void SimpleStreamChecker::reportDoubleClose(SymbolRef FileDescSym,
                                            const CallEvent &Call,
                                            CheckerContext &C) const {
  // 1. 경로를 종료하고 오류 노드 생성
  ExplodedNode *ErrNode = C.generateErrorNode();

  // 2. 같은 오류 경로가 이미 분석됐으면 건너뜀
  if (!ErrNode)
    return;

  // 3. 버그 리포트(경로 민감 버그리포트) 객체 생성
  auto R = std::make_unique<PathSensitiveBugReport>(
      DoubleCloseBugType,
      "Closing a previously closed file stream",
      ErrNode);

  // 4. 소스 코드 위치 표시 (하이라이트)
  R->addRange(Call.getSourceRange());
  // 5. 사용자에게 흥미로운 값 강조 (추적 경로에서 표시됨)
  R->markInteresting(FileDescSym);
  // 6. 리포트 등록
  C.emitReport(std::move(R));
}

// 리소스 누수(열린 파일이 닫히지 않은 경우)를 감지하고 리포트하는 함수
void SimpleStreamChecker::reportLeaks(ArrayRef<SymbolRef> LeakedStreams,
                                      CheckerContext &C,
                                      ExplodedNode *ErrNode) const {

    // 1. 누수된 스트림이 없으면 리포트하지 않음
  for (SymbolRef LeakedStream : LeakedStreams)
  {
      // 2. 버그 리포트 객체 생성
    auto R = std::make_unique<PathSensitiveBugReport>(
        LeakBugType,
        "Opened file is never closed; potential resource leak",
        ErrNode);

    R->markInteresting(LeakedStream);
    C.emitReport(std::move(R));
  }
}

// 특정 함수 호출이 "파일을 절대 닫지 않는다"고 확신할 수 있는지를 판별하는 유틸리티 함수
bool SimpleStreamChecker::guaranteedNotToCloseFile(
    const CallEvent &Call) const {
  // 1. 시스템 헤더가 아니면, 파일을 닫을 수 있다고 가정 (보수적)
  if (!Call.isInSystemHeader())
    return false;

  // 2. 인자가 escape될 수 있으면(포인터가 저장되거나, 콜백 등으로 전달), 
  //     파일을 닫을 수 있다고 가정 (보수적)
  if (Call.argumentsMayEscape())
    return false;

  // fclose 같은 함수는 여기서 따로 처리하지 않음(체커에서 직접 모델링하므로)
  // e.g: fclose는 실제로 파일을 닫지만, 이 함수에서는 true/false로 따로 구분하지 않음

  return true;
}

// SimpleStreamChecker에서 파일 포인터(심볼)가 "escape"되는 상황
// (즉, 추적 불가해지는 상황)을 처리하는 함수입니다.
ProgramStateRef SimpleStreamChecker::checkPointerEscape(
    ProgramStateRef State, const InvalidatedSymbols &Escaped,
    const CallEvent *Call, PointerEscapeKind Kind) const {

  // 함수 인자로 직접 전달되었고 && 파일을 닫을수 없으면 계속 추적
  // 해당 조건이 아니라면 심볼을 삭제 
  if (Kind == PSK_DirectEscapeOnCall && guaranteedNotToCloseFile(*Call)) {
    return State;
  }
  
// escape된 모든 파일 포인터 심볼을 StreamMap에서 제거합니다.
  for (SymbolRef Sym : Escaped) {
  // 해당 파일이 다른 곳에서 닫힐 것이라고 가정
    State = State->remove<StreamMap>(Sym);
  }
  return State;
}

// 체커를 실제로 등록하는 함수
void ento::registerSimpleStreamChecker(CheckerManager &mgr) {
  mgr.registerChecker<SimpleStreamChecker>();
}

// 체커 활성화 여부 (여기에선 무조건 활성화)
bool ento::shouldRegisterSimpleStreamChecker(const CheckerManager &mgr) {
  return true;
}

```