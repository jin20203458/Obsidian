
### 메모리 Leak 탐지
```cpp
void f() {
  int *p = (int*)malloc(sizeof(int));
  // free(p);  // 주석시 Leak
}
```

### Double Free 탐지
```cpp
void f() {
  int *p = (int*)malloc(sizeof(int));
  free(p);
  free(p); // Double Free
}
```

### Use After Free 탐지
```cpp
void f() {
  int *p = (int*)malloc(sizeof(int));
  free(p);
  *p = 1; // Use After Free
}
```

### Ownership Propagation(ownership_transfer)
```cpp
int *wrap_malloc() { return (int*)malloc(sizeof(int)); }
void f() {
  int *p = wrap_malloc();
  free(p);
}
```

### 사용자 정의 할당 함수 대응 (Optimistic Mode)
```cpp
__attribute__((ownership_returns(malloc, 1))) // 함수가 malloc처럼 동작
void *my_alloc(size_t n);

void f() {
  void *p = my_alloc(16);
  // free(p); // 해제 안 하면 leak 경고
}
```


### class RefState
 메모리 관리 관점에서의 심볼(symbol) 상태.


```scss
[Clang Analyzer]  
     ↓  
[MallocChecker::checkPostCall()] ← 공식 콜백 (자동 호출)  
     ↓  
if (malloc 호출) → checkBasicAlloc() ← CHECK_FN 함수 (내부 분기)  
if (free 호출)   → checkFree()  
if (strdup 호출) → checkStrdup()  
...
```

```cpp
  /// 휴리스틱(경험적)으로 호출 대상 함수(callee)가 메모리 해제를 의도하는지 추정합니다.
  /// 이는 구문적으로(syntactically) 판단하는데, 이유는 실행 경로의 다양한 분기(대체 경로)를 고려해야 하며
  /// 그 결과, 경로-민감(path-sensitive)한 정보는 사용할 수 없기 때문입니다.
  bool doesFnIntendToHandleOwnership(const Decl *Callee,
                                     ASTContext &ACtx) final {
    using namespace clang::ast_matchers;
    const FunctionDecl *FD = dyn_cast<FunctionDecl>(Callee);

    // 함수 본문에서 delete 또는 call(함수 호출)이 있는지 패턴 매칭
    auto Matches = match(findAll(stmt(anyOf(cxxDeleteExpr().bind("delete"),
                                            callExpr().bind("call")))),
                         *FD->getBody(), ACtx);
    for (BoundNodes Match : Matches) {
      // delete 표현식이 있다면, 소유권(메모리 해제)을 처리한다고 간주
      if (Match.getNodeAs<CXXDeleteExpr>("delete"))
        return true;

      // 함수 호출이 있다면, isFreeingCallAsWritten을 통해 해제 함수인지 추가로 확인
      if (const auto *Call = Match.getNodeAs<CallExpr>("call"))
        if (isFreeingCallAsWritten(*Call))
          return true;
    }
    // TODO: 해제뿐 아니라 메모리 저장(대입)을 시도한 경우에도 소유권이 바뀔 수 있음.
    // 향후에는 이런 경우도 확인하도록 추가해야 함.
    return false;
  }

```


```cpp

```


```cpp
// - **역할: 메모리 누수 검사기 (총괄 책임자)**
class MallocChecker
    : public Checker<check::DeadSymbols, check::PointerEscape,
                     check::ConstPointerEscape, check::PreStmt<ReturnStmt>,
                     check::EndFunction, check::PreCall, check::PostCall,
                     check::NewAllocator, check::PostStmt<BlockExpr>,
                     check::PostObjCMessage, check::Location, eval::Assume> 

// - **역할: 소유권 변경 없음 검증기 (특수 임무 전문가)**
class NoMemOwnershipChangeVisitor final : public NoOwnershipChangeVisitor

// - **역할: 버그 리포트 생성기 (보고서 작성자)**
class MallocBugVisitor final : public BugReporterVisitor


```
















































