## 주요 check:: 템플릿과 콜백 함수들



`check::Location` :**Clang Static Analyzer에서 "메모리 위치(주소) 접근"이 발생할 때 호출되는 콜백**을 구현하는 체크 포인트


## **함수 호출 관련**

|템플릿 매개변수|콜백 함수|호출 시점|용도|
|---|---|---|---|
|`check::PreCall`|`checkPreCall(const CallEvent &Call, CheckerContext &C)`|함수 호출 직전|인자 검증, 사전 조건 체크|
|`check::PostCall`|`checkPostCall(const CallEvent &Call, CheckerContext &C)`|함수 호출 직후|반환값 처리, 상태 업데이트|

## **문장(Statement) 관련**

|템플릿 매개변수|콜백 함수|호출 시점|용도|
|---|---|---|---|
|`check::PreStmt<ReturnStmt>`|`checkPreStmt(const ReturnStmt *S, CheckerContext &C)`|return 문 실행 직전|반환값 검증|
|`check::PostStmt<DeclStmt>`|`checkPostStmt(const DeclStmt *S, CheckerContext &C)`|선언문 실행 직후|변수 초기화 추적|

## **심볼 생명주기 관련**

|템플릿 매개변수|콜백 함수|호출 시점|용도|
|---|---|---|---|
|`check::DeadSymbols`|`checkDeadSymbols(SymbolReaper &SR, CheckerContext &C)`|심볼 소멸 시|리소스 누수 탐지|
|`check::LiveSymbols`|`checkLiveSymbols(ProgramStateRef State, SymbolReaper &SR)`|심볼 생존 확인 시|심볼 생존 연장|

## **포인터 관련**

|템플릿 매개변수|콜백 함수|호출 시점|용도|
|---|---|---|---|
|`check::PointerEscape`|`checkPointerEscape(ProgramStateRef State, const InvalidatedSymbols &Escaped, const CallEvent *Call, PointerEscapeKind Kind)`|포인터 추적 불가 시|오탐 방지|
|`check::Location`|`checkLocation(SVal Loc, bool IsLoad, const Stmt *S, CheckerContext &)`|메모리 접근 시|버퍼 오버플로우 등|

## **함수 분석 경계**

|템플릿 매개변수|콜백 함수|호출 시점|용도|
|---|---|---|---|
|`check::BeginFunction`|`checkBeginFunction(CheckerContext &Ctx)`|함수 분석 시작|초기 상태 설정|
|`check::EndFunction`|`checkEndFunction(const ReturnStmt *RS, CheckerContext &Ctx)`|함수 분석 종료|최종 상태 검증|

## **Objective-C 관련**

|템플릿 매개변수|콜백 함수|호출 시점|용도|
|---|---|---|---|
|`check::PreObjCMessage`|`checkPreObjCMessage(const ObjCMethodCall &M, CheckerContext &C)`|메시지 전송 직전|메시지 검증|
|`check::PostObjCMessage`|`checkPostObjCMessage(const ObjCMethodCall &M, CheckerContext &C)`|메시지 전송 직후|결과 처리|
|`check::ObjCMessageNil`|`checkObjCMessageNil(const ObjCMethodCall &M, CheckerContext &C)`|nil 수신자 메시지|nil 메시지 처리|

## **AST 기반 체크**

|템플릿 매개변수|콜백 함수|호출 시점|용도|
|---|---|---|---|
|`check::ASTDecl<FunctionDecl>`|`checkASTDecl(const FunctionDecl *D, AnalysisManager &Mgr, BugReporter &BR)`|AST 순회 시|구문 분석 기반 체크|
|`check::ASTCodeBody`|`checkASTCodeBody(const Decl *D, AnalysisManager &Mgr, BugReporter &BR)`|함수 본문 분석 시|코드 패턴 분석|

## **평가(Evaluation) 관련**

|템플릿 매개변수|콜백 함수|호출 시점|용도|
|---|---|---|---|
|`eval::Call`|`evalCall(const CallEvent &Call, CheckerContext &C)`|함수 호출 평가|커스텀 함수 모델링|
|`eval::Assume`|`evalAssume(ProgramStateRef State, SVal Cond, bool Assumption)`|조건 가정 시|제약 조건 추가|

## 실제 사용 예시

```cpp
class SimpleStreamChecker : public Checker<check::PostCall,     // 템플릿 매개변수
                                           check::PreCall,      
                                           check::DeadSymbols,  
                                           check::PointerEscape> {
public:
  void checkPostCall(...) const;     // 자동 바인딩되는 콜백
  void checkPreCall(...) const;      
  void checkDeadSymbols(...) const;  
  void checkPointerEscape(...) const;
};
```

## 핵심 특징

- **컴파일 타임 바인딩**: 템플릿 메타프로그래밍으로 컴파일 시점에 콜백이 자동 등록됩니다[1](https://clang.llvm.org/doxygen/CheckerDocumentation_8cpp_source.html).
    
- **타입 안전성**: 잘못된 시그니처의 콜백 함수는 컴파일 에러가 발생합니다.
    
- **성능 최적화**: 런타임 오버헤드 없이 필요한 이벤트에만 콜백이 등록됩니다.
    
- **확장성**: 새로운 check:: 템플릿을 추가하여 새로운 이벤트 타입을 쉽게 지원할 수 있습니다.
    

이 구조 덕분에 체커 개발자는 **템플릿 매개변수만 지정하면 자동으로 해당 이벤트의 콜백이 등록**되어, 매우 직관적이고 안전한 체커 개발이 가능합니다.