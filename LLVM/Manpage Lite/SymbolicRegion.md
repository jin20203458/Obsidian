## 🔷 1. `SymbolicRegion`이란?

`SymbolicRegion`은 Clang Static Analyzer에서 **정확한 주소를 알 수 없는 메모리 객체**를 추상적으로 표현하는 [[MemRegion]]입니다.

### 즉,

> “여기에 뭔가 메모리 객체가 _있긴 한데_, 우리가 주소나 구체적인 정체를 모르겠어. 대신 이 심볼로 추적할게!”

---

## 🔷 2. 언제 사용될까?

`SymbolicRegion`은 다음과 같은 경우에 자동으로 생성됩니다:

|상황|이유|
|---|---|
|`new`, `malloc`, `alloca` 등|주소를 알 수 없는 힙 영역|
|함수 리턴값이 포인터일 때|어떤 주소를 반환할지 알 수 없음|
|구조체 내부의 포인터 필드|필드가 뭘 가리키는지 정적 분석 시점에 불명확|
|`this` 포인터|객체 자체를 포인터로 표현해야 하므로, 추상적 모델 필요|
e.g
```cpp
int *p = malloc(sizeof(int));   // → p는 SymbolicRegion을 가리킴
MyClass *obj = new MyClass();   // → obj도 SymbolicRegion
```
