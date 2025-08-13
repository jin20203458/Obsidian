
참조:
[[Double Dispatch]]
[[CRTP]]
[[ASTContext]]
[[AST-based Checking]]


---
## **정적 다형성 (CRTP)**
- ✅ **성능**: 직접 호출, 인라인 최적화 가능
- ✅ **메모리**: vptr 불필요
- ❌ **유연성**: 컴파일 타임에만 타입 결정 가능
## **RecursiveASTVisitor에서 CRTP 적용**

**- 기본 구조**
```cpp
template <typename Derived>
class RecursiveASTVisitor {
public:
    // 핵심 메서드: getDerived()
    Derived &getDerived() { 
        return static_cast<Derived&>(*this); 
    }
    
    const Derived &getDerived() const { 
        return static_cast<const Derived&>(*this); 
    }
    
    // 자동 호출의 핵심
    bool TraverseStmt(Stmt *S) 
    {
        // ... 복잡한 분기 로직
        switch (S->getStmtClass()) 
        {
            case Stmt::CompoundStmtClass:
                return getDerived().TraverseCompoundStmt(
                    //  ^^^^^^^^^^^^
                    //  컴파일 타임에 ShadowVisitor로 캐스팅!
                    static_cast<CompoundStmt*>(S));
        }
    }
};
```


##  **메타프로그래밍의 역할**

 **- 템플릿 인스턴스화**
```cpp
// 컴파일러가 실제로 생성하는 코드 (개념적)
class RecursiveASTVisitor_ShadowVisitor {  // 특화된 버전
public:
    bool TraverseStmt(Stmt *S) {
        switch (S->getStmtClass()) {
            case Stmt::CompoundStmtClass:
                // 직접 바인딩!
                return static_cast<ShadowVisitor*>(this)
                    ->TraverseCompoundStmt(static_cast<CompoundStmt*>(S));
        }
    }
};
```


---

##  **AST란 무엇인가?**

 **정의**
**Abstract Syntax Tree**: C++ 코드를 트리 형태의 추상적인 구문 표현으로 변환한 것

**- 간단한 예시**
```cpp
int main() {
    int x = 5;
    if (x > 0) {
        return x;
    }
}
```

**- AST 구조:**
```scss
FunctionDecl (main)
└── CompoundStmt (함수 본문)
    ├── DeclStmt (변수 선언)
    │   └── VarDecl (x)
    │       └── IntegerLiteral (5)
    ├── IfStmt (if문)
    │   ├── BinaryOperator (x > 0)
    │   │   ├── DeclRefExpr (x)
    │   │   └── IntegerLiteral (0)
    │   └── CompoundStmt (if 본문)
    │       └── ReturnStmt
    │           └── DeclRefExpr (x)
```


## **세 가지 핵심 노드 타입**

 **Decl (선언문들)**
```cpp
// 다양한 선언들
class MyClass {};              // CXXRecordDecl
void func(int param) {}        // FunctionDecl + ParmVarDecl  
int global_var = 10;           // VarDecl
namespace ns {}                // NamespaceDecl
template<typename T> class X {}; // ClassTemplateDecl
```

**Stmt (문장들)**
```cpp

```