
> **핵심 아이디어**: 프로그램이 실행될 수 있는 **모든 가능한 경로를 시뮬레이션**하여 버그를 찾습니다. 이는 마치 탐정이 모든 가능한 시나리오를 따라가며 단서를 찾는 것과 같습니다[4](https://llvm.org/devmtg/2020-09/slides/Using_the_clang_static_ananalyzer_to_find_bugs.pdf)[6](https://github.com/haoNoQ/clang-analyzer-guide/blob/master/clang-analyzer-guide.tex).

**동작 방식**:  
이 방식의 핵심은 **심볼릭 실행(Symbolic Execution)**입니다[7](https://par.nsf.gov/servlets/purl/10285754). 실제 값을 사용하는 대신, 변수에 `x`, `y`와 같은 심볼(Symbol)을 할당하고 코드 실행을 가상으로 시뮬레이션합니다.

1. **경로 분기**: `if (x > 0)`과 같은 조건문을 만나면, 엔진은 `x > 0`인 경로와 `x <= 0`인 경로, 두 가지 가능성으로 분석 경로를 분리합니다[4](https://llvm.org/devmtg/2020-09/slides/Using_the_clang_static_ananalyzer_to_find_bugs.pdf)[6](https://github.com/haoNoQ/clang-analyzer-guide/blob/master/clang-analyzer-guide.tex).
    
2. **상태 추적**: 각 경로마다 변수 값, 메모리 상태 등을 지속적으로 추적하고 기록합니다. 이렇게 갈라지고 확장되는 분석 상태의 그래프를 **확장 그래프(Exploded Graph)**라고 부릅니다[4](https://llvm.org/devmtg/2020-09/slides/Using_the_clang_static_ananalyzer_to_find_bugs.pdf)[6](https://github.com/haoNoQ/clang-analyzer-guide/blob/master/clang-analyzer-guide.tex).
    
3. **버그 탐지**: 특정 경로에서 "메모리 해제 후 다시 사용(Use-after-free)"이나 "널 포인터 참조"와 같은 논리적 모순이나 위험한 상태가 발생하면 버그로 보고합니다[1](https://discourse.llvm.org/t/accessing-taint-status-outside-of-clang-or-combining-ast-matchers-with-taint-status/61495)[6](https://github.com/haoNoQ/clang-analyzer-guide/blob/master/clang-analyzer-guide.tex).
    

**장점**:

- **정밀하고 깊이 있는 분석**: 코드의 실행 흐름과 상태를 이해하므로, 단순한 패턴 매칭으로는 찾을 수 없는 복잡한 버그(예: 메모리 누수, 이중 해제)를 찾아낼 수 있습니다[6](https://github.com/haoNoQ/clang-analyzer-guide/blob/master/clang-analyzer-guide.tex).
    

**단점**:

- **속도**: 가능한 모든 경로를 탐색해야 하므로 분석에 많은 시간이 소요될 수 있습니다[6](https://github.com/haoNoQ/clang-analyzer-guide/blob/master/clang-analyzer-guide.tex). 조건문이 많아지면 분석해야 할 경로가 기하급수적으로 늘어날 수 있습니다(State Explosion).
    

**대표적인 체커: `CastSizeChecker`**  
이 체커는 `malloc`으로 할당된 메모리의 **크기**를 알아야 합니다. 이 크기는 프로그램 실행 중에 결정되는 값일 수 있으므로, 코드 실행을 시뮬레이션하며 메모리 할당 시점의 크기 정보를 추적하는 경로 민감 분석이 반드시 필요합니다.

## 요약 비교

|구분|AST 기반 분석|경로 민감 분석|
|---|---|---|
|**핵심**|코드의 **구조와 패턴** 검사|코드의 **실행 흐름과 상태** 시뮬레이션|
|**분석 방법**|추상 구문 트리(AST) 순회|심볼릭 실행 및 확장 그래프(Exploded Graph) 생성|
|**장점**|빠르고 간단함|깊고 정밀한 버그 탐지|
|**단점**|단순한 버그만 탐지 가능|느리고 복잡함|
|**예시 체커**|`CastToStructChecker`|`CastSizeChecker`|
