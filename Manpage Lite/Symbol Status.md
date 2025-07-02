Clang Static Analyzer에서 심볼 상태는 분석 과정에서 다양한 형태로 관리되며, 주요 상태들은 다음과 같습니다:

## 1. **Live (활성 상태)**

- **정의**: 현재 실행 경로에서 유효하게 참조되는 심볼
    
- **판단**: `SymbolReaper::isLive(SymbolRef sym)`
    
- **사용 예시**:
    
    cpp
    
    `void checkLiveSymbols(ProgramStateRef State, SymbolReaper &SR) const {     if (SR.isLive(Sym)) {        // 심볼 계속 추적    } }`
    

## 2. **Dead (비활성 상태)**

- **정의**: 더 이상 참조되지 않는 심볼 (메모리 해제 대상)
    
- **판단**: `SymbolReaper::isDead(SymbolRef sym)`
    
- **사용 예시**:
    
    cpp
    
    `void checkDeadSymbols(SymbolReaper &SymReaper, CheckerContext &C) const {     if (SymReaper.isDead(Sym) && !ResourceClosed) {        reportLeak(); // 리소스 누수 감지    } }`
    

## 3. **Escaped (추적 불가 상태)**

- **정의**: 분석 범위를 벗어난 심볼 (외부 전달 등)
    
- **종류**: `PointerEscapeKind` 열거형
    
    - `PSK_DirectEscapeOnCall`: 함수 호출로 직접 전달
        
    - `PSK_IndirectEscapeOnCall`: 포인터를 통해 간접 전달
        
    - `PSK_EscapeOutParameters`: 출력 매개변수로 전달
        
- **처리**:
    
    cpp
    
    `ProgramStateRef checkPointerEscape(ProgramStateRef State, ...) const {     for (SymbolRef Sym : Escaped) {        State = State->remove<ResourceMap>(Sym); // 추적 중단    }    return State; }`
    

## 4. **Constrained (제약 조건 적용 상태)**

- **정의**: 값의 범위가 제한된 심볼 (e.g., `x > 5`)
    
- **관리**: `ConstraintManager`
    
- **사용 예시**:
    
    cpp
    
    `ConditionTruthVal Val = CMgr.assume(State, SymGT, true); if (Val.isConstrainedTrue()) {     // 조건 만족 시 처리 }`
    

## 5. **Undefined (정의되지 않음)**

- **표현**: `UndefinedVal()`
    
- **사용**: 초기화되지 않은 변수, 잘못된 메모리 접근 감지
    
- **예시**:
    
    cpp
    
    `SVal Val = Call.getReturnValue(); if (Val.isUndef()) {     reportBug(); // 정의되지 않은 값 사용 }`
    

## 6. **Symbolic Region (심볼릭 메모리 영역)**

- **정의**: 메모리 영역을 나타내는 심볼 (`SymbolicRegion`)
    
- **사용**: 동적 할당 메모리 추적
    
    cpp
    
    `SymbolRef Sym = SymMgr.getSymbol(Sym); const MemRegion *Region = MRMgr.getSymbolicRegion(Sym);`
    

## 7. **Concrete (구체적 값)**

- **표현**: `ConcreteInt`, `ConcreteFloat`
    
- **특징**: 실제 값이 알려진 심볼 (e.g., `42`)
    
- **최적화**: 상태 추적 불필요
    

## 상태 관리 메커니즘

|**메커니즘**|**목적**|**관련 클래스**|
|---|---|---|
|`SymbolReaper`|Live/Dead 심볼 분류|`checkDeadSymbols` 콜백|
|`ConstraintManager`|심볼 제약 조건 관리|범위 분석, 분기 조건|
|`ProgramState`|상태 저장/복구|`ExplodedGraph` 노드 관리|
|`InvalidatedSymbols`|무효화된 심볼 추적|포인터 이스케이프 처리|

## 실제 적용 예시

cpp

`// 리소스 누수 감지 (SimpleStreamChecker) void checkDeadSymbols(SymbolReaper &SymReaper, CheckerContext &C) const {   ProgramStateRef State = C.getState();  for (auto [Sym, Status] : State->get<StreamMap>()) {    if (SymReaper.isDead(Sym) && Status.isOpened()) {      reportLeak(Sym, C); // Dead 상태 + 열린 파일 → 누수    }    if (SymReaper.isDead(Sym)) {      State = State->remove<StreamMap>(Sym); // 상태 제거    }  } }`

> ✅ **핵심**:
> 
> - **Dead/Escaped**는 리소스 관리 핵심 상태
>     
> - **Constrained**는 경로 분기 최적화에 활용
>     
> - **Symbolic Region**은 동적 메모리 분석의 기반  
>     체커는 이러한 상태 전이를 활용해 정밀한 버그 탐지를 수행합니다.
>     

1. [https://clang-analyzer.llvm.org/checker_dev_manual.html](https://clang-analyzer.llvm.org/checker_dev_manual.html)
2. [https://llvm.org/devmtg/2019-10/slides/Dergachev-DevelopingTheClangStaticAnalyzer.pdf](https://llvm.org/devmtg/2019-10/slides/Dergachev-DevelopingTheClangStaticAnalyzer.pdf)
3. [https://clang.llvm.org/doxygen/SmartPtrModeling_8cpp_source.html](https://clang.llvm.org/doxygen/SmartPtrModeling_8cpp_source.html)
4. [https://clang-analyzer.llvm.org/available_checks.html](https://clang-analyzer.llvm.org/available_checks.html)
5. [https://github.com/haoNoQ/clang-analyzer-guide/blob/master/clang-analyzer-guide.tex](https://github.com/haoNoQ/clang-analyzer-guide/blob/master/clang-analyzer-guide.tex)
6. [https://clang.llvm.org/docs/UsersManual.html](https://clang.llvm.org/docs/UsersManual.html)
7. [https://srg.doc.ic.ac.uk/files/papers/sa-dse-issta-22.pdf](https://srg.doc.ic.ac.uk/files/papers/sa-dse-issta-22.pdf)
8. [https://llvm.org/devmtg/2012-11/Zaks-Rose-Checker24Hours.pdf](https://llvm.org/devmtg/2012-11/Zaks-Rose-Checker24Hours.pdf)
9. [https://www.youtube.com/watch?v=bCr2Rw7UpBI](https://www.youtube.com/watch?v=bCr2Rw7UpBI)
10. [https://docs.nordicsemi.com/bundle/ncs-3.1.0-preview3/page/zephyr/develop/sca/clang.html](https://docs.nordicsemi.com/bundle/ncs-3.1.0-preview3/page/zephyr/develop/sca/clang.html)
11. [https://stackoverflow.com/questions/23448540/how-to-keep-track-of-a-variable-with-clangs-static-analyzer](https://stackoverflow.com/questions/23448540/how-to-keep-track-of-a-variable-with-clangs-static-analyzer)
12. [https://clang.llvm.org/doxygen/MallocChecker_8cpp_source.html](https://clang.llvm.org/doxygen/MallocChecker_8cpp_source.html)
13. [https://blog.trailofbits.com/2014/04/27/using-static-analysis-and-clang-to-find-heartbleed/](https://blog.trailofbits.com/2014/04/27/using-static-analysis-and-clang-to-find-heartbleed/)
14. [https://clang.llvm.org/doxygen/ExprEngine_8h_source.html](https://clang.llvm.org/doxygen/ExprEngine_8h_source.html)
15. [https://ceur-ws.org/Vol-2650/paper12.pdf](https://ceur-ws.org/Vol-2650/paper12.pdf)