
참조 :
[[AnalysisDeclContext]] 
[[ASTContext]]
[[CSA 논리적인 흐름 구조(ExplodedGraph)]]
[[CFG Block,Element]]

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

![[Pasted image 20250708142713.png]]



## 주요 객체 및 연결 구조

- **AnalysisDeclContext**
    
    - 함수/메서드/생성자 등 하나의 Decl(선언) 단위의 분석 컨텍스트입니다.
        
    - 내부에 **CFG**와 **CFGStmtMap**을 모두 소유합니다.
        
- **CFG (Control Flow Graph)**
    
    - 함수 내부의 실행 흐름을 블록 단위로 그래프로 표현합니다.
        
    - 여러 개의 **CFGBlock**으로 구성되어 있습니다.
        
- **CFGBlock**
    
    - CFG의 각 노드(블록)로, 하나의 실행 흐름 단위(예: if, for, 일반 코드 블록 등)를 나타냅니다.
        
    - 각 블록은 여러 **CFGElement**(문장, 조건, 선언 등)를 포함합니다.
        
- **CFGElement**
    
    - CFGBlock을 구성하는 개별 요소로, 실제로는 프로그램의 한 문장(Stmt), 선언(Decl), 조건(Condition) 등이 될 수 있습니다.
        
- **CFGStmtMap**
    
    - AST의 각 문장(Stmt*)이 CFG의 어느 블록(CFGBlock)에 속하는지 빠르게 매핑해주는 자료구조입니다.
        
    - 즉, **AST 문장 ↔ CFGBlock** 간의 인덱스 역할을 합니다.
        
- **AST의 Stmt 노드**
    
    - 소스코드의 각 문장(예: if, for, return, 식 등)을 나타내는 AST의 노드입니다.
        
    - CFGStmtMap을 통해 해당 문장이 CFG의 어느 블록에 매핑되는지 빠르게 찾을 수 있습니다.