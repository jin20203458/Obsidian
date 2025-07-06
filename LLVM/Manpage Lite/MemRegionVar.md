
Clang Static Analyzer는 C/C++ 코드 분석 중 변수나 표현식의 값을  
`SVal`이라는 추상 구조로 표현합니다.

```cpp
int x = 5;
int *p = &x;
```

- `x`: 값 `5` → `nonloc::ConcreteInt`
- `p`: 값 `&x` → `loc::MemRegionVal(VarRegion(x))`
