- `CheckerDocumentation`은 **Clang Static Analyzer**에서 지원하는 **모든 주요 콜백(callback) 함수**와 **인터페이스**를 코드 내에 명시적으로 나열하고, 각 콜백의 역할과 사용 방법을 문서화하는 목적의 "예시용/도움말용 checker"입니다.
    
- 실제로 버그 검출이나 분석 기능을 제공하지 않으며, **체커를 새로 작성하려는 개발자**가 각 콜백의 역할과 시그니처를 참고할 수 있도록 **체커 개발 입문용 참고서**로 활용됩니다.
    
- 각 콜백의 함수 헤더에는 어떤 이벤트에 대응하는 콜백인지, 파라미터 의미, 사용 예시, 오버라이드 방법 등이 자세하게 주석처리 되어 있습니다.

```cpp
/// Statement(문장) 분석 전에 호출됩니다.
///
/// 이 함수는 분석 엔진이 해당 Statement를 처리하기 **직전**에 호출됩니다.
/// (단, if문과 같은 제어 흐름 분기문은 제외됩니다.)
/// Statement의 타입(예: ReturnStmt)에 따라 특화할 수 있습니다.
///
/// 분기문 같은 조건 처리는 checkBranchCondition()을 참고하세요.
void checkPreStmt(const ReturnStmt *DS, CheckerContext &C) const {}

/// Statement(문장) 분석이 끝난 후에 호출됩니다.
///
/// 이 함수는 분석 엔진이 Statement를 처리한 **직후**에 호출됩니다.
/// (단, if문과 같은 제어 흐름 분기문은 제외됩니다.)
/// Statement의 타입(예: DeclStmt)에 따라 특화할 수 있습니다.
void checkPostStmt(const DeclStmt *DS, CheckerContext &C) const {}

/// Objective-C 메시지를 분석하기 **전에** 호출됩니다.
///
/// 메서드 호출, 프로퍼티 접근 등 Objective-C 메시지 전송이 발생하는 모든 경우에 호출됩니다.
void checkPreObjCMessage(const ObjCMethodCall &M, CheckerContext &C) const {}

/// Objective-C 메시지 분석이 끝난 **후에** 호출됩니다.
///
void checkPostObjCMessage(const ObjCMethodCall &M, CheckerContext &C) const {}

/// 메시지의 리시버(receiver)가 nil일 때 호출됩니다.
///
/// 리시버가 확실히 nil인 경우에만 호출되며, 이 경우에는 checkPreObjCMessage, checkPostObjCMessage, checkPreCall, checkPostCall은 호출되지 않습니다.
void checkObjCMessageNil(const ObjCMethodCall &M, CheckerContext &C) const {}

/// 함수/메서드 호출 **전**에 호출됩니다.
///
/// 모든 함수, 메서드, 연산자 호출 전에 호출됩니다.
/// 특정 종류의 호출만 검사하려면 이 함수 내부에서 Call의 타입을 검사하세요.
void checkPreCall(const CallEvent &Call, CheckerContext &C) const {}

/// 함수/메서드 호출 **후**에 호출됩니다.
///
void checkPostCall(const CallEvent &Call, CheckerContext &C) const {}

/// 조건문 분기(예: if, while 등)에서 조건식 분석 시 호출됩니다.
///
void checkBranchCondition(const Stmt *Condition, CheckerContext &Ctx) const {}

/// C++의 new 연산자 할당 호출 시점에 호출됩니다.
///
/// (1) operator new()로 메모리 할당, (2) 결과를 클래스 포인터로 캐스팅,
/// (3) 값이 non-null일 때 생성자 호출, (4) 최종적으로 new-expression의 값을 할당.
/// 이 콜백은 (2)와 (3) 사이에 호출됩니다.
void checkNewAllocator(const CXXAllocatorCall &, CheckerContext &) const {}

/// 포인터 주소(load/store) 접근 시 호출됩니다.
///
/// \param Loc    접근하는 메모리 위치
/// \param IsLoad true면 읽기(load), false면 쓰기(store)
/// \param S      해당 Statement
void checkLocation(SVal Loc, bool IsLoad, const Stmt *S, CheckerContext &) const {}

/// 값이 특정 메모리(주소)에 할당될 때 호출됩니다.
///
void checkBind(SVal Loc, SVal Val, const Stmt *S, CheckerContext &) const {}

/// 심볼(변수 등)이 더 이상 사용되지 않아(dead) 소멸될 때 호출됩니다.
///
/// checkers는 이 시점에 상태를 정리하거나, 리소스 해제 검사를 할 수 있습니다.
/// 예: malloc/free 분석 등.
void checkDeadSymbols(SymbolReaper &SR, CheckerContext &C) const {}

/// 함수 분석 시작 시 호출됩니다.
///
void checkBeginFunction(CheckerContext &Ctx) const {}

/// 함수 분석 종료 시 호출됩니다.
///
void checkEndFunction(const ReturnStmt *RS, CheckerContext &Ctx) const {}
```