>**ParentMapContext**는 Clang의 AST(추상 구문 트리)에서 **특정 노드(예: Stmt, Expr, Decl 등)의 부모 노드(Parent)를 효율적으로 찾기 위한 유틸리티 클래스**입니다.
## **1. ParentMapContext의 계층 구조**
```scss
ASTContext
   │
   └── ParentMapContext
           │
           └── ParentMap
```

- **ASTContext**: 전체 소스 코드의 AST(추상 구문 트리) 정보를 관리하는 핵심 객체입니다.
    
- **ParentMapContext**:
    
    - ASTContext에 연결되어,
        
    - "이 AST 노드의 부모는 누구인가?"를 빠르게 찾을 수 있도록 해주는 유틸리티 클래스입니다.
        
    - 내부적으로 **ParentMap** 객체를 관리합니다.
        
- **ParentMap**:
    
    - 실제로 AST 노드와 그 부모 노드 간의 매핑(맵, 사전)을 저장합니다.
        
    - AST를 탐색(RecursiveASTVisitor 이용)해서 노드-부모 관계를 구축합니다.

## 2. **객체 관계도**

