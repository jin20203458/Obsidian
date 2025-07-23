
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

// realloc 실패 상황에서 스택 트레이스에 표시할 메시지(힌트)를 생성하는 클래스
class StackHintGeneratorForReallocationFailed : public StackHintGeneratorForSymbol

// 특정 심볼(메모리 블록 등)을 더 이상 추적하지 않도록
// 프로그램 상태(RegionState) 에서 해당 심볼 정보를 제거하는 역할
class StopTrackingCallback final : public SymbolVisitor 
```


### 함수 디스패치를 위한 맵 (Function Dispatch Maps)

`checkPostCall`과 같은 메인 라우터 함수가 특정 함수 호출을 만났을 때, 어떤 검사 로직을 실행할지 빠르고 효율적으로 찾기 위해 사용됩니다. 이 맵들은 모두 **`CallDescriptionMap`** 타입이며, (함수 프로필, 실행할 콜백 함수) 쌍을 저장합니다[1](https://clang.llvm.org/doxygen/MallocChecker_8cpp_source.html).

|맵 이름|주요 역할 및 대상 함수|
|---|---|
|**`AllocatingMemFnMap`**|메모리 **할당** 함수를 처리합니다[1](https://clang.llvm.org/doxygen/MallocChecker_8cpp_source.html). `malloc`, `calloc`, `valloc`, `strdup`, `kmalloc` 등[1](https://clang.llvm.org/doxygen/MallocChecker_8cpp_source.html).|
|**`FreeingMemFnMap`**|메모리 **해제** 함수를 처리합니다[1](https://clang.llvm.org/doxygen/MallocChecker_8cpp_source.html). `free`, `kfree`, `g_free` 등[1](https://clang.llvm.org/doxygen/MallocChecker_8cpp_source.html).|
|**`ReallocatingMemFnMap`**|메모리 **재할당** 함수를 처리합니다[1](https://clang.llvm.org/doxygen/MallocChecker_8cpp_source.html). `realloc`, `reallocf`, `g_realloc_n` 등[1](https://clang.llvm.org/doxygen/MallocChecker_8cpp_source.html).|
|**`AllocaMemFnMap`**|스택 메모리를 할당하는 `alloca` 계열 함수를 처리합니다[1](https://clang.llvm.org/doxygen/MallocChecker_8cpp_source.html). `alloca`, `_alloca`, `__builtin_alloca_with_align`[1](https://clang.llvm.org/doxygen/MallocChecker_8cpp_source.html).|
|**`PreFnMap` / `PostFnMap`**|`getdelim`, `getline`과 같이 호출 전/후에 특별한 검사가 필요한 특정 함수들을 처리합니다[1](https://clang.llvm.org/doxygen/MallocChecker_8cpp_source.html).|

### 프로그램 상태(ProgramState) 저장을 위한 맵과 셋

분석기가 코드 경로를 따라가면서 각 메모리 영역의 상태를 추적하고 저장하기 위해 사용되는 핵심 자료구조입니다. 이들은 `REGISTER_MAP_WITH_PROGRAMSTATE` 와 `REGISTER_SET_WITH_PROGRAMSTATE` 매크로를 통해 분석기의 상태 시스템에 등록됩니다[1](https://clang.llvm.org/doxygen/MallocChecker_8cpp_source.html).


```cpp
// 맵
//  각 메모리 영역의 상태를 추적하는 맵
REGISTER_MAP_WITH_PROGRAMSTATE(RegionState, SymbolRef, RefState)

// `realloc` 함수의 복잡한 동작을 모델링하기 위한 맵
REGISTER_MAP_WITH_PROGRAMSTATE(ReallocPairs, SymbolRef, ReallocPair)

//  "만약 [Key]라는 값을 반환하면, [Value]라는 메모리는 해제된 것이다" 라는 규칙을 저장하는 테이블
REGISTER_MAP_WITH_PROGRAMSTATE(FreeReturnValue, SymbolRef, SymbolRef)

// 셋
// 크기가 0으로 할당된 메모리 심볼들을 모아놓은 블랙리스트
REGISTER_SET_WITH_PROGRAMSTATE(ReallocSizeZeroSymbols, SymbolRef)

```


```cpp
ProgramStateRef MallocChecker::MallocMemAux(CheckerContext &C,
                                            const CallEvent &Call, SVal Size,
                                            SVal Init, ProgramStateRef State,
                                            AllocationFamily Family) const {
```



































