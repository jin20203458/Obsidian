
참조:
[[Decl]]
[[Stmt]]

```cpp
class ASTContext {
  llvm::FoldingSet<PointerType> PointerTypes; //포인터 타입 집합
  llvm::FoldingSet<FunctionProtoType> FunctionProtoTypes;//함수시그니처 집합
  llvm::DenseMap<const Decl*, AttrVec*> DeclAttrs;//
  QualType IntTy, FloatTy, DoubleTy; // 순수한 의미의 "타입" 자체
  ...
};

// e.g
clang::ASTContext &Context = CI.getASTContext();
clang::QualType IntType = Context.IntTy;
```

계층구조
```scss
clang::ASTContext
├── FoldingSet<Type>           (각종 타입 유일 집합)
├── DenseMap<Decl*, AttrVec*>  (선언별 속성)
├── IdentifierTable            (식별자 테이블)
├── SourceManager              (소스 위치 정보)
├── TranslationUnitDecl*       (최상위 선언)
├── AST 전용 Allocator         (메모리 관리)
├── ExternalASTSource*         (외부 AST 연동)
├── QualType                   (내장 타입 캐시: IntTy, FloatTy 등)
└── 기타                       (진단, 내장 함수, 언어 옵션 등)
```

> Clang이 C/C++ 소스 코드를 분석하여 만든 **추상 구문 트리(AST, Abstract Syntax Tree)의 모든 정보를 총괄하는 핵심 클래스**

TU(.cpp, .c)당 1개의 AST가 생성
```cpp
int add(int a, int b) {
    int sum = a + b;
    return sum;
}
```

```scss
TranslationUnitDecl
└── FunctionDecl (name: "add", return type: 'int', params: 'a' (int), 'b' (int))
    └── CompoundStmt (function body)
        ├── DeclStmt (declaration of 'sum')
        │   └── VarDecl (name: "sum", type: 'int')
        │       └── BinaryOperator (op: '+', type: 'int') (initialization of 'sum')
        │           ├── DeclRefExpr (refers to 'a', type: 'int')
        │           └── DeclRefExpr (refers to 'b', type: 'int')
        └── ReturnStmt
            └── DeclRefExpr (refers to 'sum', type: 'int')
```



Clang AST는 일반적으로 다음과 같은 종류의 노드들로 구성됩니다:

| 카테고리                             | 대표 클래스 | 예시                                                                  |
| -------------------------------- | ------ | ------------------------------------------------------------------- |
| **선언(Declaration)**              | `Decl` | `FunctionDecl`, `VarDecl`, `RecordDecl`                             |
| **문장/표현식(Statement/Expression)** | `Stmt` | `IfStmt`, `ReturnStmt`, `BinaryOperator`, `CallExpr`, `DeclRefExpr` |
| **타입(Type)**                     | `Type` | `BuiltinType`, `PointerType`, `FunctionProtoType`                   |
`FunctionDecl` : 함수 선언과 정의를 모두 표현
- 정의일 경우 함수 몸체가 붙는데 이것이 `CompoundStmt`
