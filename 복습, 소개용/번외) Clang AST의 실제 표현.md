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

---

## 예시


```cpp
int main() 
{

    int x;
    if(x > 0) 
    {
        x++;
    } 
    else 
    {
        x --;
	}
    return 0;
}

```

이는 실제 AST Dump 명령어를 사용하여 확인한 Clang AST의 일부분이며, 해당 구조의 위는 TU(총 번역 단위)와, 기타 초기화 사항들이 있습니다. 해당 부문은 메인함수에 해당하는 AST 부분입니다.
```scss
`-FunctionDecl 0x21c555b6878 <C:\Users\user\Desktop\ErrorTest\ErrorTest\.\ErrorTest.cpp:1:1, line:14:1> line:1:5 main 'int ()'
  `-CompoundStmt 0x21c555b6bb8 <col:12, line:14:1>
    |-DeclStmt 0x21c555b6a18 <line:3:5, col:10>
    | `-VarDecl 0x21c555b69b0 <col:5, col:9> col:9 used x 'int'
    |-IfStmt 0x21c555b6b50 <line:4:5, line:11:2> has_else
    | |-BinaryOperator 0x21c555b6a90 <line:4:8, col:12> 'bool' '>'
    | | |-ImplicitCastExpr 0x21c555b6a78 <col:8> 'int' <LValueToRValue>
    | | | `-DeclRefExpr 0x21c555b6a30 <col:8> 'int' lvalue Var 0x21c555b69b0 'x' 'int'
    | | `-IntegerLiteral 0x21c555b6a50 <col:12> 'int' 0
    | |-CompoundStmt 0x21c555b6ae8 <line:5:5, line:7:5>
    | | `-UnaryOperator 0x21c555b6ad0 <line:6:9, col:10> 'int' postfix '++'
    | |   `-DeclRefExpr 0x21c555b6ab0 <col:9> 'int' lvalue Var 0x21c555b69b0 'x' 'int'
    | `-CompoundStmt 0x21c555b6b38 <line:9:5, line:11:2>
    |   `-UnaryOperator 0x21c555b6b20 <line:10:9, col:11> 'int' postfix '--'
    |     `-DeclRefExpr 0x21c555b6b00 <col:9> 'int' lvalue Var 0x21c555b69b0 'x' 'int'
    `-ReturnStmt 0x21c555b6ba8 <line:13:5, col:12>
      `-IntegerLiteral 0x21c555b6b80 <col:12> 'int' 0
```