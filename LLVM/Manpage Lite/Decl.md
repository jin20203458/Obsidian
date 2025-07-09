>**Decl**은 Clang AST(추상 구문 트리)에서 "선언(declaration)"을 표현하는 모든 노드의 **최상위 기반 클래스**입니다.  
 함수, 변수, 타입, 구조체, 클래스, 네임스페이스 등 C/C++의 모든 선언 구문이 Decl 또는 그 하위 클래스로 표현됩니다.


참조:
[[ASTContext]]
[[AnalysisDeclContext]]

## Decl 클래스 내부 구조

아래는 Decl의 대표적인 내부 멤버와 역할을 요약한 표입니다.

|멤버/구조체|설명|
|---|---|
|`Decl::Kind Kind`|이 Decl이 어떤 종류(함수, 변수, 타입 등)인지 구분하는 enum 값|
|`DeclContext *DC`|이 Decl이 속한 상위 선언 컨텍스트(예: 함수의 경우 소속 클래스 등)|
|`SourceLocation Loc`|소스코드 상의 위치 정보|
|`llvm::PointerIntPair<Decl*,...>`|같은 컨텍스트 내 다음 Decl을 가리키는 포인터와 비트 플래그|
|`unsigned Access`|C++ 접근 지정자(public, private 등)|
|`unsigned IdentifierNamespace`|이 Decl이 속한 식별자 네임스페이스(변수, 타입, 멤버 등)|
|`unsigned Id`|Decl 인스턴스의 고유 식별자|
|`AttrVec Attrs`|선언에 붙은 속성(attribute) 리스트|
|`Decl *PrevDecl, *NextDecl`|같은 엔티티의 이전/다음 선언(재선언 체인)|

- 주요 멤버 함수 예시:
    
    - `getKind()`: Decl 종류 반환
    - `getDeclContext()`: 상위 DeclContext 반환
    - `getLocation()`: 소스 위치 반환
    - `getPreviousDecl()/getMostRecentDecl()`: 재선언 체인 탐색
    - `getBody()/hasBody()`: 함수/메서드 등에서 본문(Body) 존재 여부 확인

Decl은 다양한 하위 클래스에서 멤버가 확장되며, 실제 선언 유형별로 특화된 정보를 추가로 가집니다[1](https://clang.llvm.org/doxygen/classclang_1_1Decl.html)[2](https://swtv.kaist.ac.kr/courses/cs453-fall14/lec5-Clang-tutorial.pdf).

## Decl 클래스 계층도

Decl은 매우 다양한 하위 클래스를 가지고 있으며, 대표적인 계층 구조는 다음과 같습니다.

```scss
Decl
├── NamedDecl (이름이 있는 선언)
│   ├── ValueDecl (값을 가지는 선언)
│   │   ├── VarDecl (변수 선언)
│   │   ├── FunctionDecl (함수 선언)
│   │   └── FieldDecl (구조체/클래스 멤버 변수)
│   ├── TypeDecl (타입 선언)
│   │   ├── TypedefNameDecl (typedef, using 등)
│   │   └── TagDecl (struct/union/enum/class 선언)
│   │       ├── RecordDecl (struct/class/union)
│   │       │   └── CXXRecordDecl (C++ 클래스/구조체)
│   │       └── EnumDecl (enum)
│   └── NamespaceDecl (네임스페이스 선언)
├── ImportDecl (import 구문)
├── UsingDecl (using 구문)
├── FriendDecl (friend 선언)
└── 기타 Objective-C, 템플릿, 블록 등 각종 선언
```
## 계층 구조와 활용 예시

- **함수 선언**: `FunctionDecl` (→ `NamedDecl` → `Decl`)
- **변수 선언**: `VarDecl` (→ `ValueDecl` → `NamedDecl` → `Decl`)
- **클래스 선언**: `CXXRecordDecl` (→ `RecordDecl` → `TagDecl` → `TypeDecl` → `NamedDecl` → `Decl`)
- **네임스페이스**: `NamespaceDecl` (→ `NamedDecl` → `Decl`)

이처럼 **Decl 계층 구조**는 C/C++의 모든 선언 구문을 AST에서 일관성 있게 표현하고,  
각 선언별로 특화된 정보와 기능을 계층적으로 확장할 수 있도록 설계되어 있습니다[1](https://clang.llvm.org/doxygen/classclang_1_1Decl.html)[2](https://swtv.kaist.ac.kr/courses/cs453-fall14/lec5-Clang-tutorial.pdf)[3](https://clang.llvm.org/docs/IntroductionToTheClangAST.html).

## 참고

- Decl과 Stmt는 Clang AST의 양대 축으로, 각각 "선언"과 "실행문"을 표현합니다.
    
- Decl의 하위 클래스는 매우 많으므로, 실제로는 Clang 도큐먼트의 [Decl 클래스 계층도][4](https://clang.llvm.org/doxygen/hierarchy.html)를 참고하면 전체 구조를 한눈에 볼 수 있습니다.
    

**요약:**  
Decl은 Clang AST에서 "선언"을 표현하는 모든 노드의 기반 클래스이며,  
내부적으로 선언 종류, 위치, 컨텍스트, 재선언 체인, 속성 등 다양한 정보를 멤버로 가집니다.  
함수, 변수, 타입, 클래스 등 모든 선언 구문이 Decl의 하위 클래스로 계층적으로 표현됩니다[1](https://clang.llvm.org/doxygen/classclang_1_1Decl.html)[2](https://swtv.kaist.ac.kr/courses/cs453-fall14/lec5-Clang-tutorial.pdf)[3](https://clang.llvm.org/docs/IntroductionToTheClangAST.html).