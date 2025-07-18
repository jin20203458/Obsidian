
> - **DeadStoresChecker**는 "더 이상 사용되지 않는 변수(=dead variable)에 값을 저장(=store)하는 코드"를 찾는 **정적분석기(Clang Static Analyzer)용 플로우 센서티브(Flow-sensitive) 체커**입니다.
    
- 즉, **읽히지 않을 값(Dead value)이 저장된 곳**을 자동으로 경고해줍니다.

