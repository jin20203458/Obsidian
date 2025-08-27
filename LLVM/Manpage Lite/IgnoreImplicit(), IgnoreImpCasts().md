
---

## 🧠 Clang AST에서의 두 함수 비교

| 함수 이름              | 제거 대상          | 제거 범위                                                         | 목적              | 결과                |
| ------------------ | -------------- | ------------------------------------------------------------- | --------------- | ----------------- |
| `IgnoreImplicit()` | **암시적 래퍼 노드**  | `ExprWithCleanups`, `MaterializeTemporaryExpr`, `ParenExpr` 등 | 불필요한 래핑 제거      | **암시적 캐스트는 남김**   |
| `IgnoreImpCasts()` | **암시적 캐스트 노드** | `ImplicitCastExpr`                                            | 의미 없는 자동 캐스트 제거 | **암시적 캐스트까지 제거됨** |

---

## 🔍 핵심 차이 요약

- `IgnoreImplicit()`  
    → **둘러싸는 래퍼 노드만 제거**  
    → `ImplicitCastExpr`는 **보존됨**  
    → 분석자가 **캐스트 정보를 유지하고 싶을 때** 사용
    
- `IgnoreImpCasts()`  
    → **암시적 캐스트까지 제거**  
    → `DeclRefExpr`, `CallExpr` 등 **핵심 표현식에 바로 접근 가능**  
    → **캐스트가 분석에 방해될 때** 사용
    

---

## 🧪 예시 AST 구조

```cpp
ExprWithCleanups
└─ MaterializeTemporaryExpr
   └─ ImplicitCastExpr <LValueToRValue>
      └─ DeclRefExpr("x")
```

- `IgnoreImplicit()` → `ImplicitCastExpr`까지 도달
- `IgnoreImpCasts()` → `DeclRefExpr("x")`까지 도달

---

## 💡 실전 팁

- **캐스트 종류를 분석하고 싶다** → `IgnoreImplicit()` 사용 후 `dyn_cast<ImplicitCastExpr>()`
- **핵심 표현식만 보고 싶다** → `IgnoreImpCasts()` 또는 `IgnoreParenImpCasts()`

---

형진님처럼 Clang AST를 직접 다루시는 분에게는  
이 두 함수의 정확한 역할을 구분하는 게 **정적 분석의 정확도와 신뢰성**을 좌우해요.  
지금까지 바꿔서 생각하셨다니, 이걸 바로잡은 것만으로도 분석 품질이 확 올라갈 거예요!  
원하시면 이걸 기반으로 AST 탐색 유틸리티 함수도 같이 만들어드릴게요.