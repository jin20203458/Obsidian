Clang Static Analyzer(CSA)에서 **BugReport**는 버그 리포트의 "기본 클래스"이며,  
여러 종류의 하위 클래스(특화 리포트)가 존재합니다.  
각 리포트는 버그의 성격, 추적 정보, 경로 민감성 등에 따라 다르게 사용됩니다.

아래에 주요 **BugReport 종류**와 그 특징을 정리합니다.

## 1. **BugReport (기본 클래스)**

- **역할**:
    
    - 버그의 종류, 메시지, 발생 위치 등 **기본적인 버그 리포트 정보**를 담는 추상화 클래스입니다.
        
    - 모든 리포트는 이 클래스를 기반으로 합니다[1](https://clang-analyzer.llvm.org/checker_dev_manual.html).
        
- **사용 예**:
    
    - 경로 추적이 필요 없는 단순 버그(예: 단일 위치에서의 경고 등).
        

## 2. **PathSensitiveBugReport**

- **역할**:
    
    - **경로 민감(path-sensitive)** 버그 리포트.
        
    - 버그가 발생한 **실행 경로 전체(ExplodedGraph 기반 경로)**를 추적·기록합니다.
        
- **특징**:
    
    - 어떤 조건/경로에서 버그가 발생했는지, 변수 값 추적, 조건 분기 등 상세 정보 포함[1](https://clang-analyzer.llvm.org/checker_dev_manual.html)[2](https://szelethus.github.io/gsoc2019/).
        
    - **addVisitor** 등으로 경로 내 변수 추적, 조건 설명, 힌트 추가 가능.
        
    - 리포트의 유일화(uniqueing)와 중복 방지 기능이 강화됨.
        
- **사용 예**:
    
    - 메모리 누수, 더블 프리, UAF, 조건부 버그 등 **경로 추적이 중요한 버그**.
        

## 3. **BasicBugReport**

- **역할**:
    
    - **경로 추적이 필요 없는 단순 버그**에 사용되는 리포트 클래스입니다.
        
- **특징**:
    
    - PathSensitiveBugReport와 달리, 경로 정보(ExplodedGraph)를 포함하지 않고  
        단일 위치에서의 버그만을 리포트합니다.
        
- **사용 예**:
    
    - 단순 API 오용, 명백한 잘못된 호출 등 **경로와 무관한 버그**.
        

## 4. **Other Specialized BugReport Types**

- **Custom/Derived BugReport**
    
    - CSA 사용자(체커 개발자)가 필요에 따라 **BugReport를 상속하여 커스텀 리포트**를 만들 수 있습니다.
        
    - 예: 특정 리소스 관리, API 사용 패턴, 사용자 정의 조건 등.
        

## 5. **비교 표**

|클래스명|경로 민감성|경로 추적(ExplodedGraph)|Visitor/Trace 확장|유일화(중복방지)|대표 사용 예시|
|---|---|---|---|---|---|
|BugReport (base)|X|X|제한적|O|모든 리포트의 기반|
|PathSensitiveBugReport|O|O|O|O|메모리 누수, 더블 프리|
|BasicBugReport|X|X|제한적|O|단순 경고, API 오용|
|Custom BugReport|개발자 정의|개발자 정의|개발자 정의|개발자 정의|특수 목적 리포트|

## 6. **실전에서의 사용**

- **대부분의 체커(메모리, 리소스, 조건부 버그 등)는 PathSensitiveBugReport를 사용**합니다.
    
- 단순한 경고/에러(경로와 무관)는 BasicBugReport로 충분합니다.
    
- 리포트 생성 시,
    
    cpp
    
    `auto R = std::make_unique<PathSensitiveBugReport>(...); auto R2 = std::make_unique<BasicBugReport>(...);`
    
    처럼 명확히 구분하여 사용합니다.
    

## 참고

- **경로 민감 리포트**는 HTML 등 시각화 리포트에서 "어떤 경로를 따라가면 이 버그가 발생하는지"를  
    그래프와 함께 보여주므로, 실제 디버깅과 원인 분석에 매우 유용합니다[3](https://llvm.org/devmtg/2020-09/slides/Using_the_clang_static_ananalyzer_to_find_bugs.pdf)[2](https://szelethus.github.io/gsoc2019/)[4](https://conf.researchr.org/details/icse-2025/icse-2025-software-engineering-in-practice/13/Towards-Better-Static-Analysis-Bug-Reports-in-the-Clang-Static-Analyzer).
    
- **LLVM/Clang 최신 버전**에서는 대부분의 실질적 리포트가 PathSensitiveBugReport를 상속받아 사용됩니다.
    

**요약:**

- **BugReport**: 모든 리포트의 기본 클래스
    
- **PathSensitiveBugReport**: 경로 추적, 변수 추적, 중복방지 등 고급 기능 포함
    
- **BasicBugReport**: 단순 버그, 경로 추적 불필요
    
- **커스텀 리포트**: 필요에 따라 상속/확장 가능
    

실제 체커 개발에서는 **PathSensitiveBugReport**가 가장 널리 쓰입니다!

추가 질문하기

소스 확인

1. [https://clang-analyzer.llvm.org/checker_dev_manual.html](https://clang-analyzer.llvm.org/checker_dev_manual.html)
2. [https://szelethus.github.io/gsoc2019/](https://szelethus.github.io/gsoc2019/)
3. [https://llvm.org/devmtg/2020-09/slides/Using_the_clang_static_ananalyzer_to_find_bugs.pdf](https://llvm.org/devmtg/2020-09/slides/Using_the_clang_static_ananalyzer_to_find_bugs.pdf)
4. [https://conf.researchr.org/details/icse-2025/icse-2025-software-engineering-in-practice/13/Towards-Better-Static-Analysis-Bug-Reports-in-the-Clang-Static-Analyzer](https://conf.researchr.org/details/icse-2025/icse-2025-software-engineering-in-practice/13/Towards-Better-Static-Analysis-Bug-Reports-in-the-Clang-Static-Analyzer)
5. [https://clang.llvm.org/doxygen/BugReporter_8cpp.html](https://clang.llvm.org/doxygen/BugReporter_8cpp.html)
6. [https://blog.trailofbits.com/2014/04/27/using-static-analysis-and-clang-to-find-heartbleed/](https://blog.trailofbits.com/2014/04/27/using-static-analysis-and-clang-to-find-heartbleed/)
7. [https://llvm.org/devmtg/2012-11/Zaks-Rose-Checker24Hours.pdf](https://llvm.org/devmtg/2012-11/Zaks-Rose-Checker24Hours.pdf)
8. [https://stackoverflow.com/questions/19863242/static-analyser-issues-with-command-line-tools](https://stackoverflow.com/questions/19863242/static-analyser-issues-with-command-line-tools)
9. [https://chromium.googlesource.com/chromium/src.git/+/65.0.3283.0/docs/clang_static_analyzer.md](https://chromium.googlesource.com/chromium/src.git/+/65.0.3283.0/docs/clang_static_analyzer.md)
10. [https://arxiv.org/pdf/1810.12041.pdf](https://arxiv.org/pdf/1810.12041.pdf)