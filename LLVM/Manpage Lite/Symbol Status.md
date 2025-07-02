

---

## 🔍 심볼 상태 종류 요약

| 상태명 | 의미 | 사용 목적 | 관련 기능/클래스 |
|-------|------|-----------|------------------|
| **Live** | 현재 경로에서 유효 | 심볼 추적 유지 | `SymbolReaper::isLive()` |
| **Dead** | 참조 종료됨 (해제 대상) | 리소스 누수 탐지 | `SymbolReaper::isDead()` |
| **Escaped** | 분석 밖으로 전달됨 | 추적 중단 | `PointerEscapeKind`, `checkPointerEscape()` |
| **Constrained** | 값에 제약이 있음 (e.g., `x > 5`) | 조건 분기 최적화 | `ConstraintManager`, `assume()` |
| **Undefined** | 정의되지 않은 값 (초기화 안됨 등) | 오류 탐지 | `UndefinedVal`, `isUndef()` |
| **Symbolic Region** | 동적 메모리 영역 추적 | 동적 할당 추적 | `SymbolicRegion`, `MemRegionManager` |
| **Concrete** | 확정된 실제 값 | 추적 불필요 (최적화) | `ConcreteInt`, `ConcreteFloat` |

---

## ⚙️ 핵심 상태 관리 메커니즘

| 메커니즘 | 역할 | 주요 클래스/메서드 |
|----------|------|--------------------|
| `SymbolReaper` | Live/Dead 판별 | `checkDeadSymbols()` |
| `ConstraintManager` | 심볼 값의 범위 제약 | `assume()`, `isConstrainedTrue()` |
| `ProgramState` | 분석 상태 저장/복구 | `ExplodedGraph` 연동 |
| `InvalidatedSymbols` | 분석 중 무효화된 심볼 관리 | 포인터 이스케이프 처리 |

---

## 💡 실제 체커 활용 예시

```cpp
void checkDeadSymbols(SymbolReaper &SymReaper, CheckerContext &C) const 
{
  ProgramStateRef State = C.getState();
  for (auto [Sym, Status] : State->get<StreamMap>()) 
  {
    if (SymReaper.isDead(Sym) && Status.isOpened()) 
    {
      reportLeak(Sym, C); // 리소스 누수 발생
    }
    
    if (SymReaper.isDead(Sym)) 
    {
      State = State->remove<StreamMap>(Sym); // 상태 제거
    }
  }
}
```

- `Dead + isOpened()` 조건 → 파일 누수 보고
- 누수 처리 후 `StreamMap`에서 제거

---

## ✅ 요점 정리

- **Dead/Escaped** → 리소스 해제·누수 관련 핵심
- **Constrained** → 분기 조건 관리로 경로 최적화
- **Symbolic Region** → 동적 메모리 기반 분석 가능
