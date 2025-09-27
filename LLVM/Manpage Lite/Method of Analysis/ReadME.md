## 1. **린터 스타일 (Linter Style)**

린터 스타일 정적분석은 구문과 스타일 규칙을 검사하는 비교적 간단한 형태의 분석입니다. Clang-tidy가 대표적인 예로, 다음과 같은 특징을 갖습니다:[clang.llvm](https://clang.llvm.org/extra/clang-tidy/)

- **빠른 검사 속도**: 경로민감분석보다 훨씬 빠르게 수행됩니다
    
- **스타일 가이드라인 검증**: 코딩 스타일, 네이밍 규칙, API 사용 패턴 검사
    
- **단순한 오류 탐지**: 초기화되지 않은 변수 사용과 같은 기본적인 오류들을 감지[stackoverflow](https://stackoverflow.com/questions/48704681/clang-static-analyzer-skipping-some-checks)
    

## 2. **경로민감분석 (Path-Sensitive Analysis)**

가장 고급 형태의 분석으로, Clang Static Analyzer의 핵심 기능입니다:[clang.llvm+2](https://clang.llvm.org/docs/ClangStaticAnalyzer.html)

- **심볼릭 실행**: 코드를 실제로 실행하지 않고도 모든 가능한 실행 경로를 시뮬레이션[llvm](https://llvm.org/devmtg/2020-09/slides/Using_the_clang_static_ananalyzer_to_find_bugs.pdf)
    
- **제어 흐름 그래프 (CFG) 활용**: 분기문과 반복문의 모든 경로를 추적하여 상세한 분석 수행[llvm](https://llvm.org/devmtg/2020-09/slides/Using_the_clang_static_ananalyzer_to_find_bugs.pdf)
    
- **깊은 버그 탐지**: 메모리 누수, 널 포인터 역참조, 버퍼 오버플로우 등 복잡한 버그 발견[chromium.googlesource](https://chromium.googlesource.com/chromium/src/+/d4afc97b7/docs/clang_static_analyzer.md)
    
- **문맥 인식 분석**: 함수 간 호출 관계를 고려한 inter-procedural 분석[clang.llvm+1](https://clang.llvm.org/docs/ClangStaticAnalyzer.html)
    

## 3. **PP (전처리 파서) 방식**

전처리기 지시문과 매크로 처리에 특화된 분석 방식입니다:[llvm+1](https://llvm.org/devmtg/2017-06/2-Hal-Finkel-LLVM-2017.pdf)

- **프리프로세서 추적**: #include, #define, #ifdef 등의 전처리 지시문 분석
    
- **매크로 확장 분석**: 매크로가 실제로 어떻게 확장되는지 추적하여 오류 탐지
    
- **조건부 컴파일 검사**: 플랫폼별 코드나 조건부 컴파일 블록에서의 문제 발견