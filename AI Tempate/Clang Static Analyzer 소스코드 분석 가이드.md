
---

## 1. [Checker/함수 개요]

- **Checker/클래스 이름**: 어떤 버그를 검출하는지, 주요 목적
    
- **함수 이름**: 함수의 역할(어떤 Analzyer 콜백인지), 반환값, 인자(매개변수) 및 각 역할
    
- Clang Static Analyzer 특화 요소: 콜백타입(checkPreCall, checkBind 등), ProgramState, SVal 등 API 설명
    

---

## 2. [Line-by-Line 분석]

각 코드 줄을 아래 예시처럼 분석합니다:

## Line N

```cpp
코드 색상 출력
```

- **구성요소**: 변수, 매개변수, 상수, 연산자, 키워드 등 상세 설명
    
- **함수 호출**: 호출된 함수의 역할과 Clang Static Analyzer에서 동작 방식
    
- **API 설명**: ProgramState, SVal, BugReporter, ExplodedGraph 등 주요 객체와 동작원리 해설
    

---

## 3. [예시]

- 실제 버그 검출 시나리오 짧은 코드 예시
    
- ProgramState 변화 Before-After 예시 (상태 추적)
    
- Symbolic Execution Handling 예시 (값의 추적)
    
- 내부 API 사용 시: 간단한 직관적 비유나 예시
    

---

## 4. [출력 형식 안내]

- [함수/Checker 개요] → 주요 역할, 반환값, 인자 설명
    
- [Line N 분석] → 소스 각 줄별 "코드 색상 출력", 상세분석
    
- [예시] → 필요시 짧은 코드 예시/비유
    

---

## 5. 목표

- Clang Static Analyzer checker와 내부 동작, API 구조를 쉽게 파악할 수 있게 설명
    
- 초보자도 바로 이해할 수 있도록 예시 및 비유 적극 활용
    
- 심볼릭 실행/상태 변화/버그 리포트의 연계 흐름까지 자연스럽게 설명
    

---

**소스코드를 붙여넣으면 위 템플릿에 맞춰 AI 분석을 제공합니다.

> 원하시면 사용자의 시험 수준이나 구체적 이해 수준에 맞춰 설명 난이도를 조정할 수 있습니다. 필요하시면 말씀해 주세요.