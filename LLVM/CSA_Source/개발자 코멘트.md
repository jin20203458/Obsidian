

|구성 요소|역할 및 위치|
|---|---|
|**Core**|분석 엔진(심볼릭 실행, 상태 추적 등)만 담당. 체커 로직은 없음.|
|**Checkers**|각종 버그 유형을 탐지하는 체커(Checker)들의 실제 구현 코드.|
||`clang/lib/StaticAnalyzer/Checkers` 폴더에 존재.|
|**Frontend**|소스코드 파싱, AST/CFG 생성 등 분석 준비 단계.|

## 상세 설명

## 1. **Core**

- **역할:**  
    프로그램의 실행 경로를 따라가며 상태(ProgramState)를 추적하고, 각 지점에서 체커에 이벤트를 전달하는 "엔진" 역할만 수행합니다.
    
- **구현 위치:**  
    `clang/lib/StaticAnalyzer/Core` 폴더.
    
- **체커 코드 없음:**  
    Core에는 버그 탐지 로직(예: 널 포인터 체크, 메모리 누수 체크 등)이 구현되어 있지 않습니다.  
    → **체커는 Core 엔진이 탐색하는 각 지점에서 호출되는 "플러그인" 형태**로 동작합니다[1](https://clang-analyzer.llvm.org/checker_dev_manual.html)[2](https://github.com/microsoft/checkedc-clang/blob/master/clang/docs/checkedc/static-analyzer-with-z3.md).
    

## 2. **Checkers**

- **역할:**  
    실제로 버그를 탐지하는 로직(예: core.NullDereference, unix.Malloc 등)이 구현되어 있습니다.
    
- **구현 위치:**  
    `clang/lib/StaticAnalyzer/Checkers` 폴더에 각 체커별 cpp 파일이 존재합니다[1](https://clang-analyzer.llvm.org/checker_dev_manual.html)[2](https://github.com/microsoft/checkedc-clang/blob/master/clang/docs/checkedc/static-analyzer-with-z3.md).
    
- **등록 및 호출:**  
    분석 엔진(Core)이 각 분석 지점에서 등록된 체커들을 차례로 호출합니다.  
    체커는 상태를 검사하여 버그를 리포트하거나, 상태를 업데이트할 수 있습니다[1](https://clang-analyzer.llvm.org/checker_dev_manual.html).
    

## 3. **Frontend**

- **역할:**  
    소스코드 → AST → CFG 생성, 분석 준비 등 전체 파이프라인의 앞단을 담당합니다.


---

### Clang Static Analyzer

---

## 📚 라이브러리 구조 (Library Structure)

분석기 라이브러리는 **두 개의 계층**으로 구성되어 있습니다:

1. **저수준(static analysis engine)** – `ExprEngine.cpp` 및 그 관련 파일들
    
2. **체커(static checkers)** – `*Checker.cpp` 파일들
    

체커는 분석 엔진 위에 `Checker` 및 `CheckerVisitor` 인터페이스(`Checker.h` 및 `CheckerVisitor.h`)를 통해 구축됩니다. `Checker` 인터페이스는 **간단하고 최소한의 인터페이스**로 설계되어 있으며, 체커 작성자가 내부 분석 엔진의 복잡한 구현에 대해 알지 않아도 되도록 돕습니다.

---

## ⚙️ 작동 방식 (How It Works)

Clang Static Analyzer는 여러 개의 **기초적인 연구 논문**([1], [2])에서 영감을 받아 개발되었습니다.  
(※ 참고: 링크는 Kremenek이 추가 예정임)

핵심적으로 이 분석기는 **소스 코드 시뮬레이터**입니다. 이는 프로그램의 가능한 실행 경로들을 추적(tracing)하여 시뮬레이션합니다.

- 프로그램의 상태(변수와 표현식의 값 등)는 `ProgramState`로 캡슐화됩니다.
    
- 프로그램의 한 위치는 `ProgramPoint`라고 합니다.
    
- `ProgramState`와 `ProgramPoint`의 조합은 **ExplodedGraph**라는 그래프에서 하나의 노드가 됩니다.
    
- **Exploded(폭발된)** 이라는 용어는 **기존의 제어 흐름 그래프(CFG)** 내의 간선을 세부적으로 "폭발시켜" 더 세밀한 실행 경로를 탐색하기 위함입니다.
    

개념적으로 이 분석기는 **ExplodedGraph를 통해 도달 가능성 분석(reachability analysis)** 을 수행합니다.

- 루트 노드에서 시작하여(`entry point` 및 초기 상태 포함), 개별 표현식을 분석하며 상태 전이를 시뮬레이션합니다.
    
- 하나의 표현식을 분석하면 상태가 변경될 수 있고, 이는 새로운 `ExplodedGraph` 노드를 생성하게 됩니다.
    
- 특정 "버그 조건(bug condition)"을 만족하는 노드에 도달하면, **버그가 발견된 것**으로 간주합니다. (이는 불변 조건을 위반한 경우 등)
    

분석기는 **조건 분기(branches)** 를 논리적으로 추론하며 다중 경로를 탐색합니다:

- 참인 분기에서는 조건이 참이라고 가정하고
    
- 거짓인 분기에서는 조건이 거짓이라고 가정합니다.
    

이러한 "가정(assumptions)"은 프로그램의 값에 **제약조건(constraints)** 을 생성하며, 이 제약들은 `ProgramState` 객체에 기록됩니다.  
(제약 조건은 `ConstraintManager`가 조작합니다)

- 어떤 분기 조건이 충족될 수 없는(모순된) 상태라면, 해당 분기는 **비실행 경로(infeasible path)** 로 간주되고 분석 대상에서 제외됩니다.
    
- 이렇게 해서 분석기는 **경로 민감(path-sensitive)** 하게 됩니다.
    

### 📌 노드 캐싱

- **지수 폭발(exponential blow-up)** 을 줄이기 위해, 분석기는 노드를 캐싱합니다.
    
- 동일한 상태와 동일한 프로그램 포인트를 가진 새로운 노드가 생성되려 할 경우, 기존 노드를 재사용하고 **분석 경로를 종료(cache out)** 합니다.
    
- 이 때문에 **ExplodedGraph는 DAG(방향 비순환 그래프)가 아니며**, 루프와 사이클이 존재할 수 있습니다.
    

---

## 🧱 불변 객체 구조 (Immutability)

- `ProgramState` 와 `ExplodedNode` 는 일단 생성되면 **변경 불가능(immutable)** 합니다.
    
- 상태를 변경하려면, **새로운 `ProgramState`를 생성해야** 합니다.
    
- 이러한 불변성은 ExplodedGraph가 프로그램의 실행 동작을 정밀하게 나타내기 위해 **중요한 특성**입니다.
    

효율적으로 이러한 객체들을 표현하기 위해, 분석기는 **함수형 자료구조(functional data structures)** 예: `ImmutableMaps` 등을 사용하여 **객체 간 데이터 공유**를 가능하게 합니다.

---

## 🕵️ 체커(Checker)의 역할

체커도 분석 상태를 조작하여 버그를 탐지할 수 있습니다.

- 분석 엔진은 체커에게 **방문자 인터페이스(visitor interface)** 를 통해 신호를 보냅니다.
    
- 예를 들어, `ExprEngine`은 `CallExpr`(함수 호출 표현식)을 분석하기 직전에 `PreVisitCallExpr()` 메서드를 호출합니다.
    
- 이 때 체커는 사전 조건(precondition)을 검사하고:
    
    - 아무 작업도 하지 않거나,
        
    - 새로운 상태(`ProgramState`)와 새로운 노드(`ExplodedNode`)를 생성할 수 있으며,
        
    - 문제가 발견되면 `BugReporter` 객체에 버그를 **보고(report)** 할 수 있습니다.
        

---

## 🇨🇵 C++ 관련 사항 (Notes about C++)

### 생성자 처리

- CFG에서 생성자가 변수보다 먼저 나타나기 때문에, 먼저 **임시 객체(temporary object)** 를 생성하여 대입 대상이 될 **영역(region)** 을 만듭니다.  
    → 관련 코드: `ExprEngine::VisitCXXConstructExpr()`
    

### 메서드 종료 시 처리

- `ExprEngine::processCallExit()` 에서는 항상 평가된 생성자 표현식(CXXConstructExpr)에 **객체 영역(object region)** 을 바인딩합니다.
    
- `VisitDeclStmt()`에서는:
    
    - 변수 타입이 참조가 아니라면 → lazy compound value를 계산하여 바인딩합니다.
        
    - 참조라면 → 객체 영역 자체를 초기값으로 사용합니다.
        

### this 포인터 처리

- 메서드 진입 시 `'this' 영역(this region)` 은 객체 영역(object region)에 바인딩됩니다.
    
- 생성자에서는 `CXXRecordDecl*`을 이용해 `'this'` 영역을 생성하며, 이는 타입 한정자(type qualifier)를 사용하지 않습니다.
    
- 일반 메서드에서는 `CXXMethodDecl*`로 `'this'`를 만들며 `getThisType()`을 통해 타입 한정자를 고려합니다.
    

→ 한 메서드 안에서는 `'this'`의 일관성만 유지하면 되며, 서로 다른 방식(qualified/unqualified)을 써도 무방합니다.

---

## 🔧 Analyzer 작업 시 참고사항 (Working on the Analyzer)

### C++ 표현식 분석 개발

- `ExprEngine` 안의 **visitation 로직**(방문자 메서드들)을 참고하세요.
    
- 다양한 표현식 시뮬레이션 예제가 포함되어 있습니다.
    

### 새 체커 작성

- `Checker.h`, `CheckerVisitor.h` 인터페이스를 참고하세요.
    
- `*Checker.cpp` 파일에서 체커 구현 예시를 볼 수 있습니다.
    

---

## 🐞 디버깅 옵션 (Debugging the Analyzer)

아래는 유용한 디버깅용 명령줄 옵션입니다:

```bash
$ clang -cc1 -help | grep analyze
 -analyze-function <함수이름>          // 특정 함수만 분석
 -analyzer-display-progress            // 분석 진행 상황 출력
 -analyzer-viz-egraph-graphviz         // ExplodedGraph를 Graphviz dot 파일로 시각화
```

---

## 🔍 CFG(Control-Flow Graph) 확인 방법

CFG를 시각화하거나 텍스트로 출력할 수 있는 옵션:

```bash
$ clang -cc1 -analyzer-checker-help-developer

 -analyzer-checker=debug.DumpCFG       // CFG를 콘솔에 텍스트로 출력
 -analyzer-checker=debug.ViewCFG       // GraphViz로 CFG 시각화
 -cfg-add-implicit-dtors               // C++의 암시적 소멸자 추가
 -cfg-add-initializers                 // C++ 멤버 초기화자 추가
 -unoptimized-cfg                      // 최적화되지 않은 CFG 생성
```

---

## 📖 참고 문헌 (References)

1. **Precise interprocedural dataflow analysis via graph reachability**
    
    - 저자: T. Reps, S. Horwitz, M. Sagiv
        
    - POPL '95
        
    - [논문 링크](http://portal.acm.org/citation.cfm?id=199462)
        
2. **A memory model for static analysis of C programs**
    
    - 저자: Z. Xu, T. Kremenek, J. Zhang
        
    - [논문 링크](http://lcs.ios.ac.cn/~xzx/memmodel.pdf)
        



---


//===----------------------------------------------------------------------===//
// Clang Static Analyzer
//===----------------------------------------------------------------------===//

= Library Structure =

The analyzer library has two layers: a (low-level) static analysis
engine (ExprEngine.cpp and friends), and some static checkers
(*Checker.cpp).  The latter are built on top of the former via the
Checker and CheckerVisitor interfaces (Checker.h and
CheckerVisitor.h).  The Checker interface is designed to be minimal
and simple for checker writers, and attempts to isolate them from much
of the gore of the internal analysis engine.

= How It Works =

The analyzer is inspired by several foundational research papers ([1],
[2]).  (FIXME: kremenek to add more links)

In a nutshell, the analyzer is basically a source code simulator that
traces out possible paths of execution.  The state of the program
(values of variables and expressions) is encapsulated by the state
(ProgramState).  A location in the program is called a program point
(ProgramPoint), and the combination of state and program point is a
node in an exploded graph (ExplodedGraph).  The term "exploded" comes
from exploding the control-flow edges in the control-flow graph (CFG).

Conceptually the analyzer does a reachability analysis through the
ExplodedGraph.  We start at a root node, which has the entry program
point and initial state, and then simulate transitions by analyzing
individual expressions.  The analysis of an expression can cause the
state to change, resulting in a new node in the ExplodedGraph with an
updated program point and an updated state.  A bug is found by hitting
a node that satisfies some "bug condition" (basically a violation of a
checking invariant).

The analyzer traces out multiple paths by reasoning about branches and
then bifurcating the state: on the true branch the conditions of the
branch are assumed to be true and on the false branch the conditions
of the branch are assumed to be false.  Such "assumptions" create
constraints on the values of the program, and those constraints are
recorded in the ProgramState object (and are manipulated by the
ConstraintManager).  If assuming the conditions of a branch would
cause the constraints to be unsatisfiable, the branch is considered
infeasible and that path is not taken.  This is how we get
path-sensitivity.  We reduce exponential blow-up by caching nodes.  If
a new node with the same state and program point as an existing node
would get generated, the path "caches out" and we simply reuse the
existing node.  Thus the ExplodedGraph is not a DAG; it can contain
cycles as paths loop back onto each other and cache out.

ProgramState and ExplodedNodes are basically immutable once created.  Once
one creates a ProgramState, you need to create a new one to get a new
ProgramState.  This immutability is key since the ExplodedGraph represents
the behavior of the analyzed program from the entry point.  To
represent these efficiently, we use functional data structures (e.g.,
ImmutableMaps) which share data between instances.

Finally, individual Checkers work by also manipulating the analysis
state.  The analyzer engine talks to them via a visitor interface.
For example, the PreVisitCallExpr() method is called by ExprEngine
to tell the Checker that we are about to analyze a CallExpr, and the
checker is asked to check for any preconditions that might not be
satisfied.  The checker can do nothing, or it can generate a new
ProgramState and ExplodedNode which contains updated checker state.  If it
finds a bug, it can tell the BugReporter object about the bug,
providing it an ExplodedNode which is the last node in the path that
triggered the problem.

= Notes about C++ =

Since now constructors are seen before the variable that is constructed
in the CFG, we create a temporary object as the destination region that
is constructed into. See ExprEngine::VisitCXXConstructExpr().

In ExprEngine::processCallExit(), we always bind the object region to the
evaluated CXXConstructExpr. Then in VisitDeclStmt(), we compute the
corresponding lazy compound value if the variable is not a reference, and
bind the variable region to the lazy compound value. If the variable
is a reference, just use the object region as the initializer value.

Before entering a C++ method (or ctor/dtor), the 'this' region is bound
to the object region. In ctors, we synthesize 'this' region with
CXXRecordDecl*, which means we do not use type qualifiers. In methods, we
synthesize 'this' region with CXXMethodDecl*, which has getThisType()
taking type qualifiers into account. It does not matter we use qualified
'this' region in one method and unqualified 'this' region in another
method, because we only need to ensure the 'this' region is consistent
when we synthesize it and create it directly from CXXThisExpr in a single
method call.

= Working on the Analyzer =

If you are interested in bringing up support for C++ expressions, the
best place to look is the visitation logic in ExprEngine, which
handles the simulation of individual expressions.  There are plenty of
examples there of how other expressions are handled.

If you are interested in writing checkers, look at the Checker and
CheckerVisitor interfaces (Checker.h and CheckerVisitor.h).  Also look
at the files named *Checker.cpp for examples on how you can implement
these interfaces.

= Debugging the Analyzer =

There are some useful command-line options for debugging.  For example:

$ clang -cc1 -help | grep analyze
 -analyze-function <value>
 -analyzer-display-progress
 -analyzer-viz-egraph-graphviz
 ...

The first allows you to specify only analyzing a specific function.
The second prints to the console what function is being analyzed.  The
third generates a graphviz dot file of the ExplodedGraph.  This is
extremely useful when debugging the analyzer and viewing the
simulation results.

Of course, viewing the CFG (Control-Flow Graph) is also useful:

$ clang -cc1 -analyzer-checker-help-developer

 -analyzer-checker=debug.DumpCFG   Display Control-Flow Graphs
 -analyzer-checker=debug.ViewCFG   View Control-Flow Graphs using GraphViz
(outdated below?)
 -cfg-add-implicit-dtors           Add C++ implicit destructors to CFGs for all analyses
 -cfg-add-initializers             Add C++ initializers to CFGs for all analyses
 -unoptimized-cfg                  Generate unoptimized CFGs for all analyses

debug.DumpCFG dumps a textual representation of the CFG to the console, and
debug.ViewCFG creates a GraphViz representation.

= References =

[1] Precise interprocedural dataflow analysis via graph reachability,
    T Reps, S Horwitz, and M Sagiv, POPL '95,
    http://portal.acm.org/citation.cfm?id=199462

[2] A memory model for static analysis of C programs, Z Xu, T
    Kremenek, and J Zhang, http://lcs.ios.ac.cn/~xzx/memmodel.pdf



