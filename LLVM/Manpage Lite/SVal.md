
>Symbolic Execution(심볼릭 실행) 과정에서 생성되는 **동적 값**  
 분석 중 변수, 표현식, 메모리 위치 등의 심볼릭/구체적 값을 추상적으로 표현

```cpp
namespace clang {
namespace ento {

class SVal {
  ...
protected:
  void *Data;
  unsigned Kind : 3;    // SValKind (UnknownVal, ConcreteInt, ... )
  unsigned BaseKind : 2; // LocKind or NonLocKind
  ...
};
```
##  주요 필드

| 필드         | 역할                                              |
| ---------- | ----------------------------------------------- |
| `Data`     | 값을 담고 있는 실제 데이터 포인터 (void*)                     |
| `Kind`     | 어떤 SVal 타입인지 구분 (e.g. ConcreteInt, SymbolVal 등) |
| `BaseKind` | `loc`인지 `nonloc`인지 구분                           |

---
## SVal 구조 분류
SVal은 **기본적으로 두 가지 계열**로 나뉩니다:

```scss
SVal
├── Loc (포인터 / 주소값 관련)
│   ├── MemRegionVal
│   ├── ConcreteInt
│   ├── GotoLabel
│   └── FunctionVal
└── NonLoc (숫자, 심볼, 복합값 등)
    ├── ConcreteInt
    ├── SymbolVal
    ├── SymExprVal
    ├── LazyCompoundVal
    └── LocAsInteger
```

그리고 `SVal` 자체는 다음과 같이 3개의 논리 클래스로도 나뉩니다:

```scss
SVal
├── UnknownVal
├── UndefinedVal
└── DefinedSVal
    ├── loc::XXX
    └── nonloc::XXX
```

## 구조도 정리
```scss
SVal
├── DefinedOrUnknownSVal
│   ├── UnknownVal                  ← 값이 뭔지 전혀 모름
│   └── DefinedSVal
│       ├── UndefinedVal            ← 초기화되지 않음
│       └── Concrete/Abstract 값
│           ├── loc::MemRegionVal   ← 어떤 메모리 주소 (Region)
│           ├── loc::ConcreteInt    ← 구체적인 주소 값
│           ├── nonloc::ConcreteInt ← 정수 상수
│           ├── nonloc::SymbolVal   ← 어떤 심볼 (추상값)
│           ├── nonloc::SymExprVal  ← 연산 결과 (sym + 1 등)
│           ├── nonloc::LazyCompoundVal ← 복합타입의 추상값 (struct 등)
│           └── ... 기타
```

---

##  내부 구현 핵심 메서드

### 1. `getAs<T>()`

SVal 내부에 있는 값이 `T` 타입인지 확인하고, 해당 타입으로 반환:

```cpp
if (auto CI = V.getAs<nonloc::ConcreteInt>())
{
  llvm::outs() << "정수 상수: " << CI->getValue() << "\n";
}
```
### 2. `getKind()`, `getBaseKind()`

- `getKind()`: 실제 타입 (예: `nonloc::ConcreteIntKind`)
    
- `getBaseKind()`: `loc`인지 `nonloc`인지 구분
    

---

##  예: ConcreteInt 저장 방식

`nonloc::ConcreteInt`는 내부적으로 `llvm::APSInt`를 저장합니다:

```cpp
class nonloc::ConcreteInt : public SVal {
  llvm::APSInt Value;
public:
  ConcreteInt(const llvm::APSInt &V) : SVal(/*...*/), Value(V) {}
  const llvm::APSInt &getValue() const { return Value; }
};
```

---

##  예: MemRegionVal 저장 방식

```cpp
class loc::MemRegionVal : public SVal {
  const MemRegion *Region;
public:
  MemRegionVal(const MemRegion *r) : SVal(...), Region(r) {}
  const MemRegion *getRegion() const { return Region; }
};
```

---

##  메모리 사용 최적화

- `SVal`은 **값을 직접 갖기보단 포인터로 참조**하거나,
- 내부적으로 **공용 구조(union-like)** 를 사용해 공간을 아낍니다.
- 대부분의 하위 클래스는 `SVal` 자체를 래핑한 thin wrapper 입니다.

---

##  요약

| 구성         | 설명                                           |
| ---------- | -------------------------------------------- |
| `SVal`     | 정적 분석기의 추상 값 표현의 기본 클래스                      |
| `Kind`     | 어떤 타입인지 구분 (`ConcreteInt`, `SymbolVal`, ...) |
| `BaseKind` | loc (주소) 또는 nonloc (값) 구분                    |
| 내부 데이터     | `void* Data`로 보관, 실제 클래스에서 캐스팅               |
```scss
clang::ento::SymbolData (추상 클래스)
├── SymbolRegionValue  ← 지역 변수 등의 초기값 심볼
├── SymbolConjured     ← 표현식 평가 결과에서 생성된 임시 심볼
├── SymbolDerived      ← 어떤 심볼에서 파생된 값 (ex: 구조체 필드)
├── SymbolExtent       ← 배열의 크기 등 메모리 범위
├── SymbolMetadata     ← 부가 정보 표현
├── SymIntExpr         ← 심볼 + 상수 (e.g., x + 1)
├── SymSymExpr         ← 심볼 + 심볼 (e.g., x == y)
└── UnarySymExpr       ← !x, -x 등

```