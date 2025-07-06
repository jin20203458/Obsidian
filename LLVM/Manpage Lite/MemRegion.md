
## 🔷 `MemRegion`이란?

Clang Static Analyzer에서 `MemRegion`은 **메모리를 추상적으로 표현하는 기본 클래스**입니다.

- 말 그대로 `"메모리의 한 구역(region)"`을 의미해요.
    
- 실제 주소나 크기를 갖진 않지만, **논리적으로 이 변수가 차지하는 메모리 영역**을 나타냅니다.

## 상속구조
```cpp
class MemRegion {
  // 모든 메모리 영역의 공통 인터페이스
};

class MemSpaceRegion : public MemRegion {
  // 스택, 힙, 전역 등 메모리 공간 루트 역할
};

class SubRegion : public MemRegion {
  const MemRegion *SuperRegion; // 부모 포인터!
};
```


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
## 🔷 MemRegion 클래스 계층 구조 (일부)

**축약형**
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

**실제 상속 계층도**
```scss
MemRegion                            ← (추상 클래스, 모든 Region의 부모)

├── MemSpaceRegion                   ← "메모리 공간" 표현 (스택, 힙, 전역 등)
│   ├── StackLocalsSpaceRegion      ← 함수 지역변수 공간 (지역 변수)
│   ├── StackArgumentsSpaceRegion   ← 함수 매개변수 공간 (파라미터)
│   ├── HeapSpaceRegion             ← malloc/new 할당 공간
│   └── GlobalSpaceRegion           ← 전역 변수 공간

├── CodeTextRegion                  ← 함수, 블록, 람다 등의 코드 위치

├── SubRegion                       ← 특정 상위 Region 위에 얹히는 하위 영역
│   ├── VarRegion                   ← 변수 하나 (지역/전역 포함)
│   ├── ParamVarRegion              ← 매개변수
│   ├── SymbolicRegion              ← 심볼로 식별되는 메모리
│   ├── FieldRegion                 ← 구조체나 클래스의 멤버
│   ├── ElementRegion               ← 배열 요소
│   ├── CompoundLiteralRegion       ← 복합 리터럴
│   ├── AllocaRegion                ← alloca 결과
│   ├── StringRegion                ← 문자열 리터럴
│   ├── ObjCIvarRegion              ← Objective-C 인스턴스 변수
│   ├── CXXTempObjectRegion         ← C++ 임시 객체
│   ├── CXXBaseObjectRegion         ← 기초 클래스의 서브 객체
│   └── 기타 블록, 람다 관련 Region들...

```


## 🔷 객체 계층도 (런타임에서 실제 구성된 객체들 간의 관계)
```scss
StackLocalsSpaceRegion        ← MemSpaceRegion 객체
└── VarRegion(x)              ← SubRegion 객체
    └── FieldRegion(x.a)      ← SubRegion 객체
        └── ElementRegion(2)  ← SubRegion 객체
```


---
## 🔷 실제 예

```cpp
int x = 5;
int *p = &x;
```

- `x`는 `VarRegion`으로 모델링
    
- 만약 `int *p = &x;` 라면 `p`의 `SVal`은 `loc::MemRegionVal(VarRegion(x))`

```cpp
int arr[10];
arr[3] = 42;
```

- `arr` → `VarRegion`
    
- `arr[3]` → `ElementRegion` (superRegion: arr, index: 3)

---

## 🔷 요약

|항목|설명|
|---|---|
|역할|메모리 공간을 추상적으로 나타냄|
|형태|클래스 계층으로 다양하게 나뉨 (`VarRegion`, `FieldRegion` 등)|
|생성 시점|변수 선언, 배열 접근, 구조체 필드 접근, 힙 할당 등|
|사용 목적|SVal이 어떤 메모리를 참조하는지 표현하기 위해|

---

## 🧠 꿀팁: SVal과 MemRegion의 관계

- `SVal`이 포인터 값을 나타낼 때 → `loc::MemRegionVal`
    
- 이 `MemRegion`이 `VarRegion`, `FieldRegion`, `SymbolicRegion` 등으로 구성되어  
    `"이 포인터는 어떤 메모리를 가리키고 있다"`는 걸 설명해 줌

