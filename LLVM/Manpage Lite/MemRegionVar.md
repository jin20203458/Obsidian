```scss
SVal
└── Loc
    └── MemRegionVal
```

- `SVal`: 모든 symbolic value의 최상위 클래스
    
- `Loc`: “위치(location)” 값을 나타내는 SVal의 서브클래스 (포인터, 주소 등)
    
- `MemRegionVal`: 특정 메모리 영역(MemRegion)을 가리키는 포인터 값을 나타냄

---
## 2. 정의 예시 (간략화된 형태)


```cpp
namespace clang {
namespace ento {

class MemRegionVal : public Loc {
  const MemRegion *Region;

public:
  explicit MemRegionVal(const MemRegion *R) : Loc(MemRegionKind), Region(R) {}

  const MemRegion *getRegion() const { return Region; }
};

} // namespace ento
} // namespace clang
```

- `Loc`의 서브클래스이므로 `SVal`로도 취급 가능
    
- 내부에 `const MemRegion*` 하나만 저장
    
- `&x`, `p`, `this` 같은 포인터 값을 표현할 때 사용

---
## 3. 예시 코드에서의 의미

```cpp
int x = 42;
int *p = &x;
```

- `x` → `VarRegion(x)` ← 메모리 영역
    
- `&x` → `loc::MemRegionVal(VarRegion(x))` ← 포인터 값
    
- `p` → `VarRegion(p)` ← 변수 자체
    
- `p`의 값 → `loc::MemRegionVal(VarRegion(x))`
    
---
## ✅ 요약

|항목|설명|
|---|---|
|이름|`loc::MemRegionVal`|
|타입|클래스 (`Loc`의 서브클래스)|
|역할|특정 `MemRegion`을 가리키는 포인터 값을 표현|
|내부|`const MemRegion*` 저장|
|예시|`&x`, `p`, `this`, `arr + i` 등|
