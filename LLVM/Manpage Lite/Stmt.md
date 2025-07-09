## Stmt 클래스란?

**Stmt**는 Clang AST(추상 구문 트리)에서 **"실행문(statement)"**을 표현하는 모든 노드의 최상위 기반 클래스입니다.  
C/C++의 if, for, while, return, 대입, 함수 호출, 블록({ ... }) 등 **실행 구문**이 모두 Stmt 또는 그 하위 클래스로 표현됩니다[2](https://clang.llvm.org/doxygen/classclang_1_1Stmt.html)[3](https://swtv.kaist.ac.kr/courses/cs453-fall14/lec5-Clang-tutorial.pdf)[6](https://swtv.kaist.ac.kr/courses/cs458-fall2022/part1-coverage/lec7-Clang-tutorial-v2.pdf).

참조:
[[ASTContext]]

## Stmt 클래스의 내부 구조

- **Stmt는 다형적(Polymorphic) 기반 클래스**로, 실제로는 다양한 하위 클래스(IfStmt, ForStmt, ReturnStmt 등)가 존재합니다.
    
- 주요 멤버 및 특징:
    
    - **Stmt::StmtClass Kind**  
        어떤 종류의 실행문인지 구분하는 enum 값 (예: IfStmtClass, ForStmtClass 등)
        
    - **자식 노드(Children)**  
        하위 구문(예: if문의 조건식, for문의 초기화/조건/증가/본문 등)을 자식 노드로 가짐  
        → 하위 클래스마다 children() 메서드 오버라이드로 자식 노드 순회 지원[2](https://clang.llvm.org/doxygen/classclang_1_1Stmt.html)[3](https://swtv.kaist.ac.kr/courses/cs453-fall14/lec5-Clang-tutorial.pdf)
        
    - **SourceLocation**  
        해당 구문의 소스 코드 위치 정보
        
    - **ASTNodeID 등 부가 정보**  
        AST 내에서의 고유 식별자 등
        

## Stmt 클래스의 주요 하위 클래스 계층도

```scss
Stmt
├── CompoundStmt      // { ... } 블록
├── DeclStmt          // 지역 변수 선언문
├── ReturnStmt        // return 문
├── IfStmt            // if 문
├── ForStmt           // for 문
├── WhileStmt         // while 문
├── SwitchStmt        // switch 문
├── Expr              // "표현식" (expression)도 Stmt의 하위 클래스
│   ├── CallExpr          // 함수 호출
│   ├── IntegerLiteral    // 정수 리터럴
│   ├── DeclRefExpr       // 변수/함수 참조
│   └── ...               // 기타 다양한 표현식
└── ...               // 기타 C/C++의 모든 실행 구문
```
- **Expr**는 Stmt의 하위 클래스이며,  
    대입, 산술, 함수 호출, 리터럴 등 "표현식"을 모두 포함합니다[3](https://swtv.kaist.ac.kr/courses/cs453-fall14/lec5-Clang-tutorial.pdf).
    

## 예시: 실제 AST 구조

```cpp
int main() {
  int x = 1;
  if (x > 0)
    return x;
  return 0;
}
```
- FunctionDecl(main)
    
    - CompoundStmt
        
        - DeclStmt (int x = 1)
            
        - IfStmt
            
            - 조건식: BinaryOperator (x > 0)
                
            - then: ReturnStmt (return x)
                
        - ReturnStmt (return 0)
            

## 특징 요약

- **Stmt**는 실행문을 일관되게 표현하는 AST의 기반 클래스입니다.
    
- **모든 실행문과 표현식이 Stmt의 하위 클래스**로 구현되어 있습니다.
    
- 각 Stmt는 자식 노드(children)를 통해 블록, 조건, 본문, 표현식 등 하위 구문을 트리 구조로 연결합니다[2](https://clang.llvm.org/doxygen/classclang_1_1Stmt.html)[3](https://swtv.kaist.ac.kr/courses/cs453-fall14/lec5-Clang-tutorial.pdf)[6](https://swtv.kaist.ac.kr/courses/cs458-fall2022/part1-coverage/lec7-Clang-tutorial-v2.pdf).
    
- AST를 순회할 때, Stmt의 종류와 자식 노드를 통해 프로그램의 실행 흐름을 분석할 수 있습니다.
    

**참고:**

- Clang AST에서 "선언"은 Decl, "실행문/표현식"은 Stmt로 구분됩니다[3](https://swtv.kaist.ac.kr/courses/cs453-fall14/lec5-Clang-tutorial.pdf)[6](https://swtv.kaist.ac.kr/courses/cs458-fall2022/part1-coverage/lec7-Clang-tutorial-v2.pdf).
- Stmt와 Decl은 모두 AST의 핵심 기반 클래스입니다.
 