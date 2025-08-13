
> 현재 탐색 중인 노드를 우리가 원하는 비지터로 탐색하는 것

- **CRTP**: 우리 비지터로 캐스팅하기 위해
- **Double Dispatch**: 노드 타입과 비지터 타입 매칭하기 위해
- **매크로 시스템**: 수백 개 노드를 자동으로 우리 비지터와 연결하기 위해
- **전위순회**: 노드를 체계적으로 탐색하면서 우리 비지터에게 전달하기 위해
```cpp
// 1. AST 노드 하나씩 탐색
for (각각의 AST 노드) {
    // 2. 그 노드를 우리가 만든 비지터로 처리
    우리비지터.적절한함수(현재노드);
}

컨베이어 벨트에서 CompoundStmt 발견
↓
"이건 블록문이네, ShadowVisitor의 TraverseCompoundStmt 작업대로!"
↓
ShadowVisitor::TraverseCompoundStmt()에서 스코프 관리 처리
```

참조:
[[CRTP]]
[[ASTContext]]
[[AST-based Checking]]
[[RecursiveASTVisitor_2]]

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
// 다양한 문장들
{ int x = 1; }                 // CompoundStmt + DeclStmt
if (condition) {}              // IfStmt
for (int i = 0; i < 10; ++i) {} // ForStmt
while (true) {}                // WhileStmt
return value;                  // ReturnStmt
```

**Type (타입들)**
```cpp
// 다양한 타입들
int                            // BuiltinType
int*                           // PointerType
std::vector<int>               // TemplateSpecializationType
void(int, float)               // FunctionProtoType
const int&                     // ReferenceType (with qualifiers)
```


## **Clang AST의 특징**

**- 계층적 상속 구조**
```scss
// Stmt 계층 구조 (일부)
Stmt
├── Expr (표현식들)
│   ├── DeclRefExpr           // 변수 참조
│   ├── CallExpr              // 함수 호출  
│   ├── BinaryOperator        // 이항 연산자
│   └── UnaryOperator         // 단항 연산자
├── CompoundStmt              // 블록문 {}
├── IfStmt                    // if문
├── ForStmt                   // for문
└── ReturnStmt                // return문
```

**- 표현식도 문장의 하위 클래스**
```cpp
// Clang의 독특한 점: Expr도 Stmt의 하위 클래스
class Expr : public Stmt {
    // 모든 표현식이 문장으로 취급됨
};
```

---

##  **RecursiveASTVisitor와 AST 노드들의 연결**


**각 AST 노드 타입별 대응하는 Visitor 함수들**

**- Decl 계열**
```cpp
class MyVisitor : public RecursiveASTVisitor<MyVisitor> {
    bool VisitFunctionDecl(FunctionDecl *FD);     // 함수 선언
    bool VisitVarDecl(VarDecl *VD);               // 변수 선언
    bool VisitCXXRecordDecl(CXXRecordDecl *RD);   // 클래스/구조체
    bool VisitParmVarDecl(ParmVarDecl *PVD);      // 매개변수
    bool VisitNamespaceDecl(NamespaceDecl *ND);   // 네임스페이스
};
```

**- Stmt 계열**
```cpp
class MyVisitor : public RecursiveASTVisitor<MyVisitor> {
    bool VisitCompoundStmt(CompoundStmt *CS);     // 블록문 {}
    bool VisitIfStmt(IfStmt *IS);                 // if문
    bool VisitForStmt(ForStmt *FS);               // for문
    bool VisitDeclStmt(DeclStmt *DS);             // 선언문
    bool VisitReturnStmt(ReturnStmt *RS);         // return문
    
    // Expr 계열 (Stmt의 하위 클래스)
    bool VisitCallExpr(CallExpr *CE);             // 함수 호출
    bool VisitBinaryOperator(BinaryOperator *BO); // 이항 연산
    bool VisitDeclRefExpr(DeclRefExpr *DRE);      // 변수 참조
};
```

**- Type 계열**
```cpp
class MyVisitor : public RecursiveASTVisitor<MyVisitor> {
    bool VisitPointerType(PointerType *PT);              // 포인터 타입
    bool VisitFunctionProtoType(FunctionProtoType *FPT); // 함수 타입
    bool VisitTemplateTypeParmType(TemplateTypeParmType *TTPT); // 템플릿 매개변수
};
```


---

## **실제 AST 순회 시나리오**

**- 예제 코드**
```cpp
void example(int param) {
    {
        int local = param + 1;
        if (local > 0) {
            std::cout << local;
        }
    }
}
```

 **순회 순서와 호출되는 함수들(기본적으로 전위순회)**
 ```scss
 1. VisitFunctionDecl(example)
   └── 2. VisitParmVarDecl(param)
   └── 3. TraverseCompoundStmt (함수 본문)
       └── 4. TraverseCompoundStmt (내부 블록)
           ├── 5. VisitDeclStmt (int local = param + 1)
           │   └── 6. VisitVarDecl(local)
           │       └── 7. VisitBinaryOperator(param + 1)
           │           ├── 8. VisitDeclRefExpr(param)
           │           └── 9. VisitIntegerLiteral(1)
           └── 10. VisitIfStmt (if문)
               ├── 11. VisitBinaryOperator(local > 0)
               └── 12. VisitCallExpr(std::cout << local)
```

##  **AST 노드 타입 분류와 특징**

|카테고리|주요 노드들|RecursiveASTVisitor 함수|특징|
|---|---|---|---|
|**함수/클래스**|FunctionDecl, CXXRecordDecl|VisitFunctionDecl, VisitCXXRecordDecl|복잡한 내부 구조|
|**변수**|VarDecl, ParmVarDecl, FieldDecl|VisitVarDecl, VisitParmVarDecl|타입 정보 포함|
|**제어문**|IfStmt, ForStmt, WhileStmt|VisitIfStmt, VisitForStmt|조건과 본문 구조|
|**표현식**|CallExpr, BinaryOperator, DeclRefExpr|VisitCallExpr, VisitBinaryOperator|값과 연산|
|**블록**|CompoundStmt, DeclStmt|TraverseCompoundStmt, VisitDeclStmt|스코프 경계|
