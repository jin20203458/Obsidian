
## 🔷 `MemRegion`이란?

Clang Static Analyzer에서 `MemRegion`은 **메모리를 추상적으로 표현하는 기본 클래스**입니다.

- 말 그대로 `"메모리의 한 구역(region)"`을 의미해요.
    
- 실제 주소나 크기를 갖진 않지만, **논리적으로 이 변수가 차지하는 메모리 영역**을 나타냅니다.
    

---

## 🔷 왜 필요한가?

정적 분석기(Static Analyzer)는 실제 메모리 주소를 모릅니다.

- 실제 메모리는 런타임에만 결정됨.
    
- 분석기는 코드를 보고 **“이 변수는 이 시점에 메모리 어딘가에 있어야 해”** 라는 가정만 가능.
    

그래서 Clang은 추상화된 메모리 모델로써 `MemRegion`을 사용해 **변수, 필드, 배열, 포인터, 힙** 등을 모델링합니다.

---

## 🔷 예시로 이해하기

```cpp
int x = 5;
int arr[10];
Point p;
```

이 코드에 대해 Clang은 다음과 같은 메모리 지역을 모델링합니다:

|코드|메모리 지역 (MemRegion 서브클래스)|
|---|---|
|`x`|`VarRegion`|
|`arr[3]`|`ElementRegion`|
|`p.x`|`FieldRegion`|
|`new int`|`SymbolicRegion` (힙 영역이므로 추상적으로 모델링됨)|

---

🔷 MemRegion 클래스 계층 구조 (일부)
```scss
MemRegion
├── CodeTextRegion      (함수 등)
├── StackSpaceRegion    (스택 프레임)
│   ├── VarRegion       (지역 변수)
│   └── ParamVarRegion  (매개변수)
├── HeapSpaceRegion     (new/malloc 결과)
├── SymbolicRegion      (정확한 주소 없는 심볼릭 객체)
├── ElementRegion       (배열 요소)
├── FieldRegion         (구조체 필드)
├── CXXBaseObjectRegion (기초 클래스 하위 객체)
└── ObjCIvarRegion      (Objective-C 인스턴스 변수)
```
