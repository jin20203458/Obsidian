>**ParentMapContext**는 Clang의 AST(추상 구문 트리)에서 **특정 노드(예: Stmt, Expr, Decl 등)의 부모 노드(Parent)를 효율적으로 찾기 위한 유틸리티 클래스**입니다.


참조:
[[ASTContext]]
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

```scss
[ASTContext]
    │
    │ 1:1
    ▼
[ParentMapContext]
    │
    │ 1:1
    ▼
[ParentMap]
    │
    │ 1:N
    ▼
[AST 노드] (Stmt, Decl, Expr, Type 등)
```
- **ASTContext**는 소스 전체의 AST와 ParentMapContext를 소유합니다.
    
- **ParentMapContext**는 ParentMap을 소유하며,  
    ParentMap은 모든 AST 노드(Stmt, Decl, Expr, Type 등)에 대해  
    "이 노드의 부모 노드는 무엇인가?"를 저장합니다.


## 3. **주요 메서드 흐름**

## ParentMapContext

- **getParents(const NodeT &Node)**  
    → 입력한 노드의 부모 노드 리스트(DynTypedNodeList)를 반환  
    → 내부적으로 ParentMap을 사용해 빠르게 부모를 찾음
    
- **clear()**  
    → ParentMap을 초기화(메모리 해제)
    
- **setTraversalKind(), getTraversalKind()**  
    → AST를 탐색할 때의 방식(예: 소스 코드에 실제로 적힌 대로만 볼지 등) 설정/조회
    

## ParentMap

- **getParents(TraversalKind TK, const DynTypedNode &Node)**  
    → 특정 탐색 방식에 따라, 해당 노드의 부모 노드 리스트 반환
    
- **AscendIgnoreUnlessSpelledInSource(const Expr _E, const Expr _Child)__  
    → 소스 코드에 실제로 적힌 부모 노드만 따라 올라가며 찾기(예: 매크로, 암시적 변환 등 무시)
    

## 4. **동작 흐름 예시**

1. **ASTContext**가 소스 전체의 AST를 관리
    
2. **ParentMapContext**가 ASTContext에 연결되어 있음
    
3. 분석기가 특정 노드(예: Stmt*)의 부모가 궁금할 때
    
    - `ParentMapContext::getParents(Node)` 호출
        
    - ParentMap이 없으면 AST 전체를 순회해 ParentMap을 처음 한 번 만듦 (lazy)
        
    - ParentMap에서 Node의 부모 노드 리스트 반환


## 5. **실제 사용 예시**
```cpp
ParentMapContext &ParentCtx = ACtx.getParentMapContext();
DynTypedNodeList Parents = ParentCtx.getParents(*S);
```

- 여기서 `S`는 예를 들어 배열 접근, 변수 참조 등 AST 노드
- Parents는 S의 부모 노드 리스트
    

## 6. **요약**

- **ParentMapContext**는 ASTContext에 연결되어,
    
    - 내부적으로 ParentMap을 관리하며,
    - AST 노드(Stmt, Decl, Expr, Type 등)의 "부모 노드"를 빠르게 찾을 수 있게 해줍니다.
        
- ParentMap은 실제로 노드-부모 매핑을 저장하며,  
    ParentMapContext의 getParents()를 통해 외부에서 쉽게 부모를 조회할 수 있습니다.
    
- 이 구조 덕분에 Clang의 정적 분석기, 리팩토링 도구 등에서  
    **문맥(상위 연산자, 제어문, 함수 등) 파악이 매우 빠르고 효율적**으로 이루어집니다.