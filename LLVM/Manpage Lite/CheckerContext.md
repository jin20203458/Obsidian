
>  **CheckerContext는 ExplodedNode 생성 시점의 모든 분석 문맥(정적+동적 정보, 유틸리티 등)을 한데 모아둔 집합체**입니다.

참조:
[[AnalysisDeclContext]]
[[BugReport]]
[[LocationContext]]

```scss
[CheckerContext]
   ├── AnalysisDeclContext*         (현재 함수/메서드의 구조 정보)
   ├── ProgramStateRef              (현재 분석 경로의 동적 상태)
   ├── ExplodedNode*                (현재 분석 노드)
   ├── BugReporter*                 (버그 리포트 유틸리티)
   ├── LocationContext*             (현재 위치 정보)
   ├── SValBuilder*                 (심볼릭 값 생성기)
   └── 기타 분석 유틸리티/설정 정보
```



```scss
[AST (Abstract Syntax Tree)]
   │
   └─> [Decl] (함수, 메서드, 생성자 등 선언 노드)
            │
            └─> [AnalysisDeclContext]
                     │
                     ├── [CFG] (Control Flow Graph)
                     ├── [CFGStmtMap] (AST 문장 ↔ CFG 블록 매핑)
                     └── [기타 분석 정보]
            │
            └─> [CheckerContext]
                     │
                     ├── AnalysisDeclContext* (현재 함수 구조 정보)
                     ├── ProgramStateRef (경로별 동적 상태)
                     ├── ExplodedNode* (분석 경로의 노드)
                     ├── BugReporter* (버그 리포트 유틸리티)
                     └── 기타 유틸리티
            │
            └─> [ProgramState]
                     │
                     └─ (변수 값, 메모리 상태, 심볼 등 동적 정보)
```

##  CheckerContext의 역할 및 특징

- **정적 정보와 동적 정보의 통합**
    
    - 함수 구조(AnalysisDeclContext)와 분석 경로별 상태(ProgramState)를 모두 포함합니다.
        
- **분석 경로별 컨텍스트 제공**
    
    - 각 ExplodedNode(경로 분기점)마다 별도의 CheckerContext가 생성되어,  
        분석 경로별로 독립적인 분석이 가능합니다.
        
- **체커(Checker)에서 분석 정보 활용의 중심**
    
    - 체크러는 CheckerContext를 통해 함수 구조, 변수 값, 메모리 상태, 위치 정보, 버그 리포트 등  
        분석에 필요한 모든 정보를 효율적으로 사용할 수 있습니다.