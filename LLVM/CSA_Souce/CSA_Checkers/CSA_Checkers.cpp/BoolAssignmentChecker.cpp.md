
> **BoolAssignmentChecker**는 LLVM의 Clang Static Analyzer에 내장된 체커로, Boolean 타입 변수에 Boolean이 아닌 값이 할당되는 상황을 감지하여 경고를 발생시키는 역할을 합니다. 이 체커는 코드의 논리적 오류를 조기에 발견하는 데 도움을 줍니다.

- 값 할당시에 콜백 

## 주요 함수 및 구조

|함수/구조체|역할 요약|
|---|---|
|`checkBind`|변수에 값이 할당될 때 호출. Boolean 타입 및 값의 유효성 검사 수행|
|`emitReport`|경고 메시지 생성 및 리포트 발행|
|`isBooleanType`|타입이 Boolean 계열인지 판별|
|`registerBoolAssignmentChecker`|체커 등록 함수|
|`shouldRegisterBoolAssignmentChecker`|항상 true 반환(체커 기본 활성화)|
## 경고 메시지

- **Assignment of a non-Boolean value**  
    Boolean 타입 변수에 0 또는 1 이외의 값이 할당될 가능성이 있을 때 발생합니다.
    
- **Might assign a tainted non-Boolean value**  
    할당된 값이 Taint(외부 입력 등 신뢰 불가)된 값일 때 발생합니다.

**활용 예시**

```cpp
bool flag;
flag = 2; // 경고 발생: Assignment of a non-Boolean value
```

```cpp
bool flag;
flag = user_input(); // user_input이 tainted 값일 경우: Might assign a tainted non-Boolean value
```