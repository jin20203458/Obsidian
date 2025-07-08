
![[Pasted image 20250708135351.png]]


## 1. 전체 구조도 (텍스트 트리)
```scss
[ASTContext]
 └─ [TranslationUnitDecl] (소스 파일 단위)
      ├─ [FunctionDecl], [CXXMethodDecl], ... (함수/메서드/생성자 선언)
      │    └─ [AnalysisDeclContext]
      │         ├─ [CFG] (Control Flow Graph)
      │         ├─ [CFGStmtMap] (AST 문장 ↔ CFG 블록 매핑)
      │         └─ [기타 분석 정보]
      │
      └─ [VarDecl], [RecordDecl], ... (변수/타입 선언 등)
 
[ExplodedGraph]
 └─ [ExplodedNode] (분석 경로의 한 지점)
      ├─ [ProgramState] (해당 지점의 동적 상태)
      ├─ [ProgramPoint] (분석 위치 정보)
      └─ [CheckerContext] (콜백 시점의 전체 문맥)
           ├─ [AnalysisDeclContext*] (현재 함수/메서드 구조 정보)
           ├─ [ProgramStateRef] (현재 경로의 상태)
           ├─ [ExplodedNode*] (현재 노드)
           ├─ [BugReporter*] (버그 리포트 유틸리티)
           └─ [LocationContext*], [SValBuilder*], ...
```

## 2. 객체별 역할 및 연결

## ASTContext & Decl

- **ASTContext**: 전체 AST(구문 트리)와 타입 시스템의 관리 컨테이너
    
- **Decl**: 함수, 메서드, 생성자, 변수, 타입 등 코드의 선언 노드
    
    - 함수/메서드/생성자 Decl에 대해 **AnalysisDeclContext**가 생성됨
        

## AnalysisDeclContext

- 각 Decl(함수/메서드/생성자) 단위로 생성되어, 해당 Decl의 구조 정보, CFG, 문장-블록 매핑 등 정적 분석 정보를 보관
    
- 내부적으로 **CFG**(Control Flow Graph)를 멤버로 가짐
    

## CFG & CFGStmtMap

- **CFG**: 함수 내부의 실행 흐름을 블록 단위로 그래프 구조로 표현
    
- **CFGStmtMap**: AST의 Stmt*와 CFGBlock 간의 매핑을 제공
    

## ExplodedGraph & ExplodedNode

- **ExplodedGraph**: 모든 가능한 실행 경로(분기점, 상태 변화 등)를 노드(ExplodedNode)로 표현한 그래프
    
- **ExplodedNode**: 경로 상의 한 지점, 각 노드는 **ProgramState**(동적 상태)와 **ProgramPoint**(위치)를 포함
    

## ProgramState

- 각 ExplodedNode마다 별도의 ProgramState가 존재
    
- 변수 값, 심볼릭 값, 메모리 상태 등 동적 분석 정보를 보관
    

## CheckerContext

- 체크러 콜백에 전달되는 분석 시점의 컨텍스트 집합체
    
- 내부적으로 **AnalysisDeclContext**, **ProgramState**, **ExplodedNode**, **BugReporter**, **LocationContext** 등 다양한 정보를 참조
    
- 체크러는 이 객체를 통해 함수 구조, 경로별 상태, 위치, 버그 리포트 등 분석에 필요한 모든 정보를 활용
    

## 3. 객체 간 관계 요약 표

|객체명|주요 역할/정보|상호 연결 관계|
|---|---|---|
|ASTContext|전체 AST/타입 시스템 관리|TranslationUnitDecl, Decl 등 포함|
|Decl|함수/메서드/생성자 등 선언 노드|AnalysisDeclContext와 1:1|
|AnalysisDeclContext|Decl 단위 구조 정보, CFG, 매핑 등|Decl, CFG, CFGStmtMap 등과 연결|
|CFG|함수 내부 제어 흐름 그래프|AnalysisDeclContext의 멤버|
|CFGStmtMap|AST 문장 ↔ CFG 블록 매핑|AnalysisDeclContext의 멤버|
|ExplodedGraph|전체 분석 경로의 집합(그래프)|ExplodedNode들로 구성|
|ExplodedNode|분석 경로의 한 지점(노드)|ProgramState, ProgramPoint, CheckerContext 참조|
|ProgramState|해당 경로 지점의 동적 상태(값, 메모리 등)|ExplodedNode와 1:1|
|CheckerContext|체크러 콜백용 분석 시점 컨텍스트|AnalysisDeclContext, ProgramState, ExplodedNode 등 참조|

## 4. 요약

- **Decl(함수/메서드/생성자)**마다 **AnalysisDeclContext**가 생성되고,  
    내부적으로 **CFG** 등 구조 정보를 가짐
    
- **ExplodedGraph**는 모든 실행 경로를 **ExplodedNode**로 표현하며,  
    각 노드는 **ProgramState**(동적 상태)와 **CheckerContext**(전체 문맥)를 포함
    
- **CheckerContext**는 체크러가 분석 시점에 필요한 모든 정보를 집약해서 제공
    
- 이 구조 덕분에 정적 분석기는 복잡한 코드의 구조와 실행 경로, 상태를 효과적으로 추적할 수 있습니다.