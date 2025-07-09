
> **ExplodedNode는 "특정 실행 위치(ProgramPoint)에서의 프로그램 상태(ProgramState)를 결합한, Symbolic Execution 경로의 한 지점(스냅샷)"을 나타내는 객체입니다.**

참조:
[[ProgramState]]
[[ProgramPoint]]

[[ExplodedGraph]]
## ExplodedNode란?

**ExplodedNode**는 Clang Static Analyzer의 핵심 데이터 구조로, 프로그램의 모든 가능한 실행 경로를 나타내는 **ExplodedGraph**의 한 노드입니다.  
각 ExplodedNode는 프로그램의 특정 실행 위치와 그 시점의 상태를 결합하여, 분석 과정에서 발생하는 다양한 경로와 상태 변화를 정밀하게 추적합니다.

## ExplodedNode의 구성

- **ProgramPoint**
    
    - 이 노드가 프로그램의 어느 위치(예: 특정 문장, 분기, 함수 호출 등)를 나타내는지 기록합니다.
        
    - Control Flow Graph(CFG) 상의 한 지점을 의미하며, 실행 흐름의 좌표 역할을 합니다.
        
- **ProgramState**
    
    - 해당 위치에서의 **프로그램 상태**(변수 값, 메모리 상태, 심볼릭 값, 제약 조건 등)를 나타냅니다.
        
    - 각 분기, 대입, 함수 호출 등에서 상태가 달라질 때마다 새로운 ProgramState가 생성됩니다.
        

## ExplodedNode의 역할

- **실행 경로 추적**
    
    - ExplodedGraph는 프로그램의 모든 가능한 실행 경로를 "폭발적으로" 확장하여 모델링합니다.
        
    - 각 ExplodedNode는 "특정 경로의 특정 시점"을 나타냅니다.
        
- **상태 변화 기록**
    
    - 분기(if, switch), 반복, 함수 호출 등으로 경로가 분기될 때마다 새로운 ExplodedNode가 만들어집니다.
        
    - 각 노드는 이전 노드(Predecessor)와 연결되어 실행 경로를 형성합니다.
        
- **버그 탐지의 기준점**
    
    - 분석 도중 버그 조건(예: 메모리 오류, 조건 위반 등)이 만족되면, 해당 ExplodedNode에서 버그 리포트가 생성됩니다.
        

## 구조적 특징

|구성 요소|설명|
|---|---|
|ProgramPoint|프로그램 내의 위치(문장, 분기, 호출 등)|
|ProgramState|해당 위치에서의 추상적 프로그램 상태|
|Predecessors|이전 ExplodedNode(들)|
|Successors|이후 ExplodedNode(들)|

- ExplodedNode는 **불변(immutable)** 객체로, 생성 이후 상태가 변하지 않습니다.
    
- 동일한 ProgramPoint와 ProgramState를 가진 노드는 그래프 내에서 재사용되어, 경로 중복을 방지합니다[1](https://clang-analyzer.llvm.org/checker_dev_manual.html)[2](https://github.com/llvm/llvm-project/blob/main/clang/lib/StaticAnalyzer/README.txt).
    

## ExplodedNode의 생성과 활용

- **분석 시작점**: 루트 ExplodedNode는 프로그램의 진입점(예: main 함수)에서 생성됩니다.
    
- **분기/상태 변화**: 분기문, 조건문, 함수 호출 등에서 ProgramState가 달라질 때마다 새로운 ExplodedNode가 생성되어 경로가 확장됩니다.
    
- **버그 리포트**: 체크커(Checker)는 ExplodedNode를 기준으로 버그를 탐지하고, 해당 노드를 리포트의 기준점으로 사용합니다.
    

## 요약

- **ExplodedNode는 "실행 위치 + 상태"를 결합한 분석 경로의 핵심 단위**입니다.
    
- ExplodedGraph 내에서 수많은 ExplodedNode가 생성·연결되어,  
    프로그램의 모든 실행 경로와 상태 변화를 정밀하게 모델링합니다.
    
- 이 구조 덕분에 Clang Static Analyzer는 복잡한 경로별 버그, 메모리 오류, 조건 위반 등을 효과적으로 탐지할 수 있습니다[1](https://clang-analyzer.llvm.org/checker_dev_manual.html)[2](https://github.com/llvm/llvm-project/blob/main/clang/lib/StaticAnalyzer/README.txt)[3](https://cyber.bibl.u-szeged.hu/index.php/actcybern/article/view/4104/4011).
    
- 공식 문서: Checker Developer Manual
    
- Clang Static Analyzer README
    
- 논문 및 기술 자료[1](https://clang-analyzer.llvm.org/checker_dev_manual.html)[2](https://github.com/llvm/llvm-project/blob/main/clang/lib/StaticAnalyzer/README.txt)[3](https://cyber.bibl.u-szeged.hu/index.php/actcybern/article/view/4104/4011)
    

1. [https://clang-analyzer.llvm.org/checker_dev_manual.html](https://clang-analyzer.llvm.org/checker_dev_manual.html)
2. [https://github.com/llvm/llvm-project/blob/main/clang/lib/StaticAnalyzer/README.txt](https://github.com/llvm/llvm-project/blob/main/clang/lib/StaticAnalyzer/README.txt)
3. [https://cyber.bibl.u-szeged.hu/index.php/actcybern/article/view/4104/4011](https://cyber.bibl.u-szeged.hu/index.php/actcybern/article/view/4104/4011)
4. [https://stackoverflow.com/questions/48704681/clang-static-analyzer-skipping-some-checks](https://stackoverflow.com/questions/48704681/clang-static-analyzer-skipping-some-checks)
5. [https://llvm.org/devmtg/2012-11/Zaks-Rose-Checker24Hours.pdf](https://llvm.org/devmtg/2012-11/Zaks-Rose-Checker24Hours.pdf)
6. [https://blog.trailofbits.com/2014/04/27/using-static-analysis-and-clang-to-find-heartbleed/](https://blog.trailofbits.com/2014/04/27/using-static-analysis-and-clang-to-find-heartbleed/)
7. [https://github.com/llvm/llvm-project/issues/71174](https://github.com/llvm/llvm-project/issues/71174)
8. [https://stackoverflow.com/questions/74617148/how-do-i-register-my-clang-static-analyzer-checker](https://stackoverflow.com/questions/74617148/how-do-i-register-my-clang-static-analyzer-checker)
9. [https://clang.llvm.org/docs/analyzer/developer-docs/PerformanceInvestigation.html](https://clang.llvm.org/docs/analyzer/developer-docs/PerformanceInvestigation.html)