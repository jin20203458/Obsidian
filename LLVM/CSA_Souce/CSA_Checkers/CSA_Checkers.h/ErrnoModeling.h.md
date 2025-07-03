
> **C/C++ 프로그램에서 `errno`라는 전역 시스템 값을 정적으로 추적**하기 위한  
  **Clang Static Analyzer용 “errno 상태 관리 API”**를 정의한 헤더입니다.
  
- errno는 시스템 콜이나 c표준라이브러리 함수들이 주로 사용
```cpp

```

- C 언어에서 `errno`는 사실상 전역 변수입니다.
    
- Static Analyzer는 분석 중에  
    `errno`가 저장된 메모리 영역(=MemoryRegion)이  
    **언제나(프로그램 어디서나) 접근 가능**하다는 걸 알고 있으니  
    "글로벌 메모리 공간"에 위치한 것으로 간주합니다.