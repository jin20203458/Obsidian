

```
clang-check .\ErrorTest.cpp -ast-dump -- -std=c++17
```

```cpp
#include <stdio.h>
#include <stdarg.h>



int main() {

    int a, b;
    scanf("%*d %d %d", &a, &b, &extra);  // extra는 불필요
    //     ^무시 ^int* ^int* ^불필요


    return 0;
}

```

```scss
PS C:\Users\user\Desktop\ErrorTest\ErrorTest> clang-check .\ErrorTest.cpp -ast-dump -ast-dump-filter=main -- -std=c++17
C:\Users\user\Desktop\ErrorTest\ErrorTest\.\ErrorTest.cpp:9:33: error: use of undeclared identifier 'extra'
    9 |     scanf("%*d %d %d", &a, &b, &extra);  // extra는 불필요
      |                                 ^
C:\Users\user\Desktop\ErrorTest\ErrorTest\.\ErrorTest.cpp:14:17: warning: more '%' conversions than data arguments
      [-Wformat-insufficient-args]
   14 |     printf("%d %s %f", 42);  // 3개 필요, 1개 제공
      |                ~^
Dumping main:
FunctionDecl 0x2795aa879f8 <C:\Users\user\Desktop\ErrorTest\ErrorTest\.\ErrorTest.cpp:6:1, line:18:1> line:6:5 main 'int ()'
`-CompoundStmt 0x2795aa881a8 <col:12, line:18:1>
  |-DeclStmt 0x2795aa87bb8 <line:8:5, col:13>
  | |-VarDecl 0x2795aa87ab8 <col:5, col:9> col:9 used a 'int'
  | `-VarDecl 0x2795aa87b38 <col:5, col:12> col:12 used b 'int'
  |-CallExpr 0x2795aa87f30 <line:9:5, col:38> '<dependent type>' contains-errors
  | |-UnresolvedLookupExpr 0x2795aa87eb0 <col:5> '<overloaded function type>' lvalue (ADL) = 'scanf' 0x2795ab19b28
  | |-StringLiteral 0x2795aa87ca0 <col:11> 'const char[10]' lvalue "%*d %d %d"
  | |-UnaryOperator 0x2795aa87ce8 <col:24, col:25> 'int *' prefix '&' cannot overflow
  | | `-DeclRefExpr 0x2795aa87cc8 <col:25> 'int' lvalue Var 0x2795aa87ab8 'a' 'int'
  | |-UnaryOperator 0x2795aa87d50 <col:28, col:29> 'int *' prefix '&' cannot overflow
  | | `-DeclRefExpr 0x2795aa87d30 <col:29> 'int' lvalue Var 0x2795aa87b38 'b' 'int'
  | `-UnaryOperator 0x2795aa87f18 <col:32, col:33> '<dependent type>' contains-errors prefix '&' cannot overflow
  |   `-RecoveryExpr 0x2795aa87ef8 <col:33> '<dependent type>' contains-errors lvalue
  |-CallExpr 0x2795aa88128 <line:14:5, col:26> 'int'
  | |-ImplicitCastExpr 0x2795aa88110 <col:5> 'int (*)(const char *const, ...) __attribute__((cdecl))' <FunctionToPointerDecay>
  | | `-DeclRefExpr 0x2795aa88088 <col:5> 'int (const char *const, ...) __attribute__((cdecl))':'int (const char *const, ...)' lvalue Function 0x2795a9e0518 'printf' 'int (const char *const, ...) __attribute__((cdecl))':'int (const char *const, ...)'
  | |-ImplicitCastExpr 0x2795aa88158 <col:12> 'const char *' <ArrayToPointerDecay>
  | | `-StringLiteral 0x2795aa88040 <col:12> 'const char[9]' lvalue "%d %s %f"
  | `-IntegerLiteral 0x2795aa88060 <col:24> 'int' 42
  `-ReturnStmt 0x2795aa88198 <line:17:5, col:12>
    `-IntegerLiteral 0x2795aa88170 <col:12> 'int' 0

1 warning and 1 error generated.

```