![[Pasted image 20250702135759.png]]
```text
ExplodedGraph
├── ExplodedNode (노드 1)
│   ├── ProgramPoint: if (x == 0) 조건 검사 지점
│   └── ProgramStateRef
│       ├── Environment: Expr* → SVal
│       │     └── x → reg_$0
│       ├── Store: VarRegion → SVal
│       │     └── x → reg_$0
│       ├── ConstraintManager:
│       │     └── reg_$0 ∈ { [0, 0] }
│       ├── SymbolManager:
│       │     └── reg_$0 = Symbol for x
│       └── GDM (GenericDataMap): [비어 있음]
│
├── ExplodedNode (노드 2)
│   ├── ProgramPoint: y = 5 실행 지점
│   └── ProgramStateRef
│       ├── Environment:
│       │     └── y = 5 → 5
│       ├── Store:
│       │     ├── x → reg_$0
│       │     └── y → 5
│       ├── ConstraintManager:
│       │     └── reg_$0 ∈ { [0, 0] }
│       ├── SymbolManager:
│       │     └── reg_$0
│       └── GDM: [비어 있음]
│
├── ExplodedNode (노드 3)
│   ├── ProgramPoint: z = 6 실행 지점
│   └── ProgramStateRef
│       ├── Environment:
│       │     └── z = 6 → 6
│       ├── Store:
│       │     ├── x → reg_$0
│       │     └── z → 6
│       ├── ConstraintManager:
│       │     └── reg_$0 ∈ { [-2^31, -1], [1, 2^31 - 1] }  ← 즉 x ≠ 0
│       ├── SymbolManager:
│       │     └── reg_$0
│       └── GDM: [비어 있음]
│
└── ... (다른 경로의 노드들)


```


```cpp
class ExplodedNode {
  const ProgramPoint Location;           // 현재 코드 상 위치
  ProgramStateRef State;                 // 이 시점의 프로그램 상태
  ExplodedNode *Pred;                    // 이전 노드 (그래프 연결용)
  SmallVector<ExplodedNode *, 2> Succs;  // 다음 노드들 (분기)
  NodeIDType NodeID;                     // (디버깅용 ID)
  unsigned Flags;                        // Sink 여부 등 상태 플래그
  ...
};
```

```cpp
class ProgramState {
  Environment Env;              // 표현식(Expression)의 평가 결과
  StoreRef store;               // 변수, 메모리 위치의 값
  GenericDataMap GDM;           // 사용자 정의 체커 상태 (예: StreamMap)
  ConstraintManager *CMgr;      // 조건 (심볼 조건들)
  SymbolManager *SymMgr;        // 심볼(SymbolRef) 생성 및 관리
  MemRegionManager *MRMgr;      // 메모리 영역(region) 관리
  ...
};
```


```
ProgramState
└── GDM
     ├── StreamMap         ← SymbolRef → StreamState
     ├── TaintMap          ← SymbolRef → Taint정보
     ├── RefCountMap       ← Region → RefCount상태
     └── (체커가 정의한 모든 사용자 맵들)
```


>explodedNode가 생성될때 각 체커에 맞는 콜백

| 콜백 함수                    | 언제 호출됨           | 예시 목적               |
| ------------------------ | ---------------- | ------------------- |
| `checkPreCall()`         | 함수 호출 **이전**     | fopen인지 확인, 인자 검사 등 |
| `checkPostCall()`        | 함수 호출 **이후**     | fopen의 리턴값 추적       |
| `checkBind()`            | 값이 변수에 **할당될 때** | 위험한 값 추적, taint 전파  |
| `checkLocation()`        | 메모리 접근 시         | 널 포인터 역참조 탐지        |
| `checkBranchCondition()` | 조건문 평가 시         | 경로 조건 검증            |
| `checkDeadSymbols()`     | 심볼이 죽을 때         | 리소스 누수 확인           |
| `checkEndFunction()`     | 함수 끝날 때          | 정리, 누락된 리소스 확인      |