## ExprEngine와 CoreEngine: Clang Static Analyzer의 분석 엔진

참조:
[[ProgramStateManager]]

## 1. 개념 및 역할

## ExprEngine

- **ExprEngine**은 Clang Static Analyzer의 **주요 실행 엔진**입니다.
    
- 프로그램의 각 실행 경로(분기, 함수 호출, 반복 등)를 따라가며 **상태(ProgramState), 메모리(store), 심볼릭 값, 경로 등**을 추적합니다.
    
- **체커(Checker)와의 상호작용**을 통해 버그 탐지, 경고 리포트 생성 등 실제 분석 로직이 수행됩니다.
    
- 각 AST 노드(Statement, Expression 등)를 해석하여, 그에 따른 상태 변화를 관리합니다.
    

## CoreEngine

- **CoreEngine**은 분석의 **실행 흐름**(Worklist 기반 경로 탐색)을 총괄하는 엔진입니다.
    
- 프로그램의 제어 흐름 그래프(CFG)를 따라 **분석 경로를 생성·확장·스케줄링**합니다.
    
- 각 경로별로 ProgramState, ExplodedGraph, ExplodedNode 등을 관리하며,  
    ExprEngine에 분석 경로의 실행을 위임합니다.
    

## 2. 구조 및 관계

```scss
[ExprEngine/CoreEngine]
       ↓
[ProgramStateManager]  --owns→  [StoreManager, SymbolManager, ConstraintManager, ...]
       ↓
[ProgramState]  --- store_t(포인터) 등 다양한 상태 정보

```

- **CoreEngine**은 분석의 전체 흐름을 관리하며,  
    **ExprEngine**은 실제로 각 프로그램 상태의 변화를 구체적으로 해석·처리합니다.
    
- **ExprEngine**은 CoreEngine의 하위 컴포넌트로,  
    CoreEngine이 경로를 확장할 때마다 ExprEngine이 해당 경로의 상태 변화를 계산합니다.
    

## 3. 내부 동작 예시

- CoreEngine이 프로그램의 한 지점(예: if문 분기)에서  
    두 개의 실행 경로를 생성하면,
    
    - 각 경로에 대해 ExprEngine이 새로운 ProgramState를 만들고,
        
    - store, 심볼, 제약 조건 등을 업데이트합니다.
        
- 각 경로는 ExplodedGraph의 ExplodedNode로 관리되며,
    
    - 이때 ProgramState, ProgramPoint, LocationContext 등  
        다양한 분석 정보가 함께 저장됩니다.
        

## 4. 주요 책임 요약

|엔진|주요 책임|
|---|---|
|CoreEngine|분석 경로 생성/확장, Worklist 관리, ExplodedGraph 관리|
|ExprEngine|프로그램 상태 해석, ProgramState/Store 관리, Checker 호출 및 상호작용|

## 5. 공식 문서 요약

- Clang Static Analyzer의 공식 문서에서는
    
    > CoreEngine은 경로 탐색과 상태 전이를 관리하고,  
    > ExprEngine은 각 프로그램 지점에서 상태의 구체적 변화를 해석한다  
    > 고 설명합니다.
    

**요약:**

- **CoreEngine**은 분석 경로의 생성·확장·스케줄링을 담당하는 상위 엔진,
    
- **ExprEngine**은 각 경로의 상태 변화, 메모리/심볼/조건 추적, Checker 호출 등  
    실제 분석 로직을 담당하는 하위 엔진입니다.
    
- 두 엔진이 유기적으로 협력하여 Clang Static Analyzer의 정밀한 정적 분석이 이루어집니다.