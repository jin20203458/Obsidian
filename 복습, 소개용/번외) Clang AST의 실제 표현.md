Clang의 AST는 개념적인 트리가 아니라, **수백 개의 C++ 클래스가 정교하게 상속 관계를 이루는 거대한 클래스 계층(Class Hierarchy)**입니다. 코드의 모든 구성 요소(선언, 구문, 표현식 등)는 이 클래스 계층의 특정 클래스에 해당하는 객체(노드)로 표현됩니다.

모든 AST 노드는 몇 가지 중요한 기본 클래스로부터 파생됩니다.

---

### ## 1. `Decl` (Declarations): 선언

`Decl`은 이름(name)을 프로그램에 도입하는 모든 선언을 나타내는 기본 클래스입니다.

- **`VarDecl`:** 변수 선언 (`int x;`)
    
- **`FunctionDecl`:** 함수 선언 (`void foo();`)
    
- **`CXXRecordDecl`:** `class`, `struct`, `union` 선언
    
- **`EnumDecl`:** `enum` 선언
    

### ## 2. `Stmt` (Statements): 문

`Stmt`는 프로그램의 실행 가능한 동작 단위를 나타내는 문(statement)의 기본 클래스입니다.

- **`CompoundStmt`:** 중괄호 `{...}`로 묶인 코드 블록
    
- **`IfStmt`:** `if` 문
    
- **`ForStmt`:** `for` 문
    
- **`ReturnStmt`:** `return` 문
    

### ## 3. `Expr` (Expressions): 식

`Expr`은 값을 계산하는 식(expression)을 나타내는 노드입니다. `Stmt`를 상속받기 때문에 모든 식은 문이 될 수 있습니다.

- **`IntegerLiteral`:** 정수 리터럴 (`10`, `42`)
    
- **`DeclRefExpr`:** 변수 사용과 같이 선언을 참조하는 식 (`x`)
    
- **`CallExpr`:** 함수 호출 (`foo()`)
    
- **`BinaryOperator`:** 이항 연산자 (`+`, `*`, `=`)
    
- **`UnaryOperator`:** 단항 연산자 (`++`, `--`, `!`)

