
>**AnalysisDeclContext는 함수/블록 등 코드 단위의 구조와 분석 정보를 관리하는 "컨테이너"**

## 핵심 클래스

- **Decl (AST의 선언 노드)**
    
    - 함수, 메서드, 생성자 등 코드의 선언을 표현하는 AST 노드
        
    - 예: `FunctionDecl`, `CXXMethodDecl`, `CXXConstructorDecl` 등
        
- **AnalysisDeclContext**
    
    - 하나의 Decl(주로 함수/메서드/생성자) 단위의 분석 컨텍스트
        
    - 해당 Decl의 AST 노드를 참조
        
    - 내부적으로 CFG, CFGStmtMap, 지역 변수 정보 등 다양한 분석 정보를 보관
        
- **CFG (Control Flow Graph)**
    
    - 함수의 실행 흐름을 블록 단위로 분해해 그래프로 표현
        
    - 각 블록은 여러 CFGElement(문장, 조건 등)로 구성
        
- **CFGStmtMap**
    
    - AST의 Stmt*와 CFGBlock 간의 매핑을 제공
        
    - 특정 문장이 어떤 CFGBlock에 속하는지 빠르게 찾을 수 있음
        
- **CheckerContext**
    
    - 체크러 콜백에 전달되는 분석 시점의 컨텍스트
        
    - 내부적으로 AnalysisDeclContext, ProgramState, BugReporter 등 다양한 정보를 포함
        
    - 현재 분석 중인 Decl(함수 등)에 대한 AnalysisDeclContext에 접근 가능
        
- **ProgramState**
    
    - 분석 경로별로 분기되는 동적 상태(변수 값, 메모리 상태 등)를 추적

```scss
[Decl] <---[참조]--- [AnalysisDeclContext]
   │                      │
   │                      ├── [CFG]
   │                      ├── [CFGStmtMap]
   │                      └── [기타 분석 정보]
   │
   └─> [CheckerContext]
            │
            ├── [AnalysisDeclContext] (현재 함수의 컨텍스트)
            ├── [ProgramState] (현재 경로의 상태)
            └── [기타 유틸리티]

```

아래는 AnalysisDeclContext의 실제 내부 구조에서 중요한 멤버 변수와 그 역할입니다.

|멤버 변수|타입/설명|
|---|---|
|`const Decl *D`|분석 대상이 되는 함수/메서드/생성자의 AST Decl 노드|
|`std::unique_ptr<CFG> cfg`|해당 Decl의 Control Flow Graph (제어 흐름 그래프)|
|`std::unique_ptr<CFGStmtMap> cfgStmtMap`|AST 문장(Stmt*)와 CFGBlock 간의 매핑 정보|
|`std::unique_ptr<ParentMap> PM`|AST 내에서 부모-자식 관계를 빠르게 찾기 위한 맵|
|`std::unique_ptr<VarRegionMap> VRM`|지역 변수와 메모리 영역(Region) 매핑 정보|
|`AnalysisDeclContextManager *Mgr`|전체 컨텍스트를 관리하는 매니저 객체|
|`unsigned blockCount`|CFG 블록의 개수 등 메타데이터|
|기타 캐시 및 분석 관련 멤버|CFG 생성 캐시, 지역 변수 정보 등|