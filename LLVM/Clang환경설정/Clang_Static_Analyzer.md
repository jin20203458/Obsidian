## Clang Static Analyzer 원리

### 1. **목적**

Clang Static Analyzer는 **코드를 실행하지 않고**, 소스 코드만 보고 **잠재적 버그**(예: 널 포인터 역참조, 메모리 누수, 초기화되지 않은 값 사용 등)를 찾아내는 **정적 분석 도구**다.

---

### 2. **분석 과정 개요**

Clang Static Analyzer의 분석 과정은 다음 순서로 이루어진다.

|단계|설명|
|---|---|
|1. **코드 파싱**|Clang 프론트엔드가 소스 코드를 **구문 분석(Parsing)** 해서 **AST(Abstract Syntax Tree, 추상 구문 트리)** 를 생성|
|2. **Semantic Analysis (의미 분석)**|AST 기반으로 타입 검사, 이름 해석 등 **의미 분석 수행**|
|3. **Control Flow Graph (CFG) 생성**|**함수/메서드 단위로** **제어 흐름 그래프(Control Flow Graph)** 생성  <br>→ 코드 실행 흐름(분기, 반복 등)을 그래프로 표현|
|4. **Path-sensitive Analysis (경로 민감 분석)**|여러 가지 가능한 **실행 경로(branch)** 별로 따로따로 시뮬레이션하면서 분석  <br>→ if/else, loop 등 조건 분기 모두 고려|
|5. **Symbolic Execution (심볼릭 실행)**|변수를 **고정된 값이 아니라 심볼(기호)** 로 취급하고, 코드 경로를 따라가면서 **값의 상태를 추적**  <br>예: 변수 x의 값이 어떤 조건일 때 버그가 발생하는지 추적|
|6. **Constraint Solving (제약 조건 풀기)**|조건문(if문 등)에 따라 가능한 경로의 **조건을 수학적 제약식으로 계산**해서 실제로 그 경로가 가능한지 판단|
|7. **버그 리포트 생성**|버그 의심 경로에 대해 **어떤 흐름에서 어떤 이유로 버그가 발생하는지** 리포트 생성|

---

### 3. **핵심 개념 요약**

|개념|설명|
|---|---|
|AST|코드의 문법 구조|
|CFG|실행 흐름|
|Path-sensitive|모든 분기 케이스 다 따짐|
|Symbolic Execution|변수값을 상징적으로 추적|
|Constraint Solving|불가능한 경로 걸러냄|
|Bug Report|가능한 버그 경로만 출력|

---

### 4. **예시 (널 포인터 접근 검출 과정)**

cpp

복사편집

`int* ptr = nullptr; if (x > 0) {     *ptr = 10;  // 이 줄에서 Null Dereference 버그 검출 }`

분석 과정:

- CFG상에서 `x > 0`일 때 해당 경로만 탐색
    
- `ptr` 값은 `nullptr`로 Symbolic Execution 단계에서 유지
    
- 해당 경로에서 `*ptr` 접근 시 **Null Dereference 버그 리포트**
    

---

### 5. **장점 / 단점**

|장점|단점|
|---|---|
|실행 없이도 버그 찾음|False Positive (오탐지) 발생|
|다양한 버그 검출 가능|Loop unrolling 한계|
|코드 커버리지 넓음|대규모 프로젝트에서 성능 저하|

---

### 6. **결론**

Clang Static Analyzer는 **정적 분석**, **경로별 시뮬레이션**, **심볼릭 실행**을 기반으로 동작하며, 실제 실행 없이도 **잠재적 버그를 미리 찾아주는 도구**다.