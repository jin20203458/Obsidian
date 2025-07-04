
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
```
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


