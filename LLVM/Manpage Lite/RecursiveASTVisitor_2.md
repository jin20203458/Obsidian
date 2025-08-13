
참조:
[[Double Dispatch]]
[[RecursiveASTVisitor_1]]

## **자동 호출의 핵심 - Double Dispatch**

**Double Dispatch**: 두 객체의 타입을 모두 고려해서 적절한 메서드를 선택하는 패턴
```cpp
// 문제 상황: 두 타입이 모두 중요
RecursiveASTVisitor* visitor;  // ShadowVisitor? PrintVisitor?
Stmt* node;                    // IfStmt? CompoundStmt? DeclStmt?

// 어떤 함수를 호출해야 할까?
visitor->처리함수(node);  // ← 두 타입 모두 알아야 올바른 함수 호출!
```

 **Single Dispatch의 한계**
```cpp
// C++의 기본 가상함수는 Single Dispatch만 지원
class Visitor {
    virtual void visit(Stmt* s) = 0;  // 오직 수신자(visitor) 타입만으로 결정
};

// 문제: Stmt의 구체적인 타입(IfStmt, CompoundStmt 등)을 구분할 수 없음!
```


##  **RecursiveASTVisitor의 Double Dispatch 구현**

**- 1차 디스패치: AST 노드 타입에 따른 분기**
```cpp
template <typename Derived>
bool RecursiveASTVisitor<Derived>::TraverseStmt(Stmt *S) {
    if (!S) return true;
    
    // 1차 디스패치: 인자(S)의 실제 타입에 따른 분기
    switch (S->getStmtClass()) {
        case Stmt::CompoundStmtClass:
            // 2차 디스패치 준비
            return getDerived().TraverseCompoundStmt(
                static_cast<CompoundStmt*>(S));
                
        case Stmt::IfStmtClass:
            return getDerived().TraverseIfStmt(
                static_cast<IfStmt*>(S));
                
        case Stmt::DeclStmtClass:
            return getDerived().VisitDeclStmt(
                static_cast<DeclStmt*>(S));
    }
}
```

**- 2차 디스패치: Visitor 타입에 따른 함수 선택**
```cpp
// getDerived()의 마법
template <typename Derived>
Derived &RecursiveASTVisitor<Derived>::getDerived() { 
    return static_cast<Derived&>(*this); 
    //     ^^^^^^^^^^^^^^^^^^^^^^^^^^^
    //     컴파일 타임에 ShadowVisitor로 캐스팅!
}

// 실제 호출 과정
getDerived().TraverseCompoundStmt(...)
↓
static_cast<ShadowVisitor&>(*this).TraverseCompoundStmt(...)
↓
ShadowVisitor::TraverseCompoundStmt(...) 직접 호출!
```


---
##  **3. 자동 호출의 전체 흐름**

 **예제 코드**
```cpp
void func() {
    {                    // CompoundStmt
        int x = 10;      // DeclStmt
        if (x > 0) {     // IfStmt
            return x;    // ReturnStmt
        }
    }
}
```

 **자동 호출 체인**
```scss
1. V.TraverseFunctionDecl(func) 호출
   └── RecursiveASTVisitor::TraverseFunctionDecl()
       └── TraverseFunctionHelper()
           └── TraverseStmt(CompoundStmt) 호출 ← 1차 디스패치 시작!
               ↓
2. switch (CompoundStmt->getStmtClass()) 분기
   └── case Stmt::CompoundStmtClass:
       └── getDerived().TraverseCompoundStmt() ← 2차 디스패치!
           └── ShadowVisitor::TraverseCompoundStmt() 직접 호출!
               ↓
3. ScopeStack.emplace_back() 실행
   └── for (Stmt *S : CS->body()) {
       └── TraverseStmt(DeclStmt) 호출 ← 다시 1차 디스패치!
           ↓
4. switch (DeclStmt->getStmtClass()) 분기  
   └── case Stmt::DeclStmtClass:
       └── getDerived().VisitDeclStmt() ← 2차 디스패치!
           └── ShadowVisitor::VisitDeclStmt() 직접 호출!
```


---

## **매크로 기반 자동 코드 생성**

**STMT 매크로의 실제 확장**
```cpp
// clang/AST/StmtNodes.inc의 내용
STMT(CompoundStmt, Stmt)
STMT(IfStmt, Stmt) 
STMT(ForStmt, Stmt)
STMT(DeclStmt, Stmt)
// ... 수백 개의 STMT 정의들
```

 **매크로 확장 결과 (개념적)**
```cpp
// TraverseStmt 내부의 switch문 자동 생성
switch (S->getStmtClass()) {
    case Stmt::CompoundStmtClass:
        return getDerived().TraverseCompoundStmt(
            static_cast<CompoundStmt*>(S));
            
    case Stmt::IfStmtClass:
        return getDerived().TraverseIfStmt(
            static_cast<IfStmt*>(S));
            
    case Stmt::ForStmtClass:
        return getDerived().TraverseForStmt(
            static_cast<ForStmt*>(S));
            
    case Stmt::DeclStmtClass:
        return getDerived().VisitDeclStmt(
            static_cast<DeclStmt*>(S));
            
    // ... 수백 개의 case문들이 자동 생성!
}
```

## **컴파일 타임 vs 런타임 비교**

 **기존 Visitor 패턴 (런타임 다형성)**
 ```cpp
 class Node {
public:
    virtual void accept(Visitor* v) = 0;  // 가상함수
};

class CompoundNode : public Node {
public:
    void accept(Visitor* v) override {
        v->visit(this);  // 런타임에 vtable lookup
    }
};

// 실행 시 오버헤드
// call qword ptr [rax+8]  ; vtable 간접 호출
```

 **RecursiveASTVisitor (컴파일타임 다형성)**
```cpp
// 컴파일러가 생성하는 실제 코드 (개념적)
class RecursiveASTVisitor_ShadowVisitor {
public:
    bool TraverseStmt(Stmt *S) {
        switch (S->getStmtClass()) {
            case Stmt::CompoundStmtClass:
                // 직접 호출! vtable 없음
                return static_cast<ShadowVisitor*>(this)
                    ->TraverseCompoundStmt(static_cast<CompoundStmt*>(S));
        }
    }
};

// 실행 시 최적화
// call ShadowVisitor::TraverseCompoundStmt  ; 직접 호출!
```
