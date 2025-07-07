CSA(Clang Static Analyzer)의 논리적인 흐름 구조는 **코드의 제어 흐름과 데이터 흐름을 따라가며, 다양한 체커(checker)를 통해 소스 코드의 잠재적 결함을 탐지**하는 방식으로 구성됩니다. 주요 단계와 구조는 다음과 같습니다:

---
## 1. Translation Unit(TU) 처리 및 AST 생성

## TU 구성 및 전처리

- **Translation Unit 정의**: 하나의 소스 파일(.c, .cpp)과 그 파일이 포함하는 모든 헤더 파일들을 합친 컴파일 단위[1](https://clang-analyzer.llvm.org/checker_dev_manual.html)
    
- **전처리 과정**: 매크로 확장, 헤더 파일 포함, 조건부 컴파일 처리
    
- **토큰화(Lexing)**: 소스 코드를 토큰 단위로 분할
    

## AST 생성

- **파싱(Parsing)**: 토큰들을 구문 분석하여 **하나의 AST 생성**[1](https://clang-analyzer.llvm.org/checker_dev_manual.html)
    
- **AST 구조**: 파일 전체를 나타내는 루트 노드 하위에 함수, 변수 선언, 표현식 등이 계층적으로 배치
    
- **함수 단위 표현**: 각 함수는 AST 내에서 `FunctionDecl` 노드로 표현되며 독립적인 서브트리 구성
    

## 2. CFG(Control Flow Graph) 생성

## AST에서 CFG로의 변환

- **CFG 생성 함수**: `clang::CFG::buildCFG()` 함수를 통해 **FunctionDecl 단위로 CFG 생성**[1](https://clang-analyzer.llvm.org/checker_dev_manual.html)
    
- **CFGBuilder 동작**: AST를 역순으로 순회하여 기본 블록의 후속자를 먼저 구성한 후 전임자 구성[2](https://github.com/haoNoQ/clang-analyzer-guide/blob/master/clang-analyzer-guide.tex)
    
- **기본 블록 생성**: 제어 흐름에 따라 문장들을 기본 블록으로 그룹화
    
- **엣지 연결**: 조건문, 반복문, 함수 호출 등의 제어 구조가 CFG의 엣지로 변환
    

## CFG 전처리

- **blockList 생성**: 방문해야 할 CFG 블록의 순서 정의, 루프의 back edge와 `break`, `goto`문 처리 고려
    
- **edgeMatrix 구성**: CFG 블록 간의 엣지 정보를 저장하는 2D 벡터 생성
    

## 3. Symbolic Execution 및 ExplodedGraph 생성

## Symbolic Execution 시작

- **심볼릭 실행 엔진**: CFG를 기반으로 **ExplodedGraph 구축**[1](https://clang-analyzer.llvm.org/checker_dev_manual.html)
    
- **ExplodedNode 구성**: 각 노드는 `(ProgramPoint, ProgramState)` 쌍으로 구성[1](https://clang-analyzer.llvm.org/checker_dev_manual.html)
    
    - **ProgramPoint**: CFG에서의 해당 위치
        
    - **ProgramState**: 해당 지점까지의 프로그램 상태
        

## 경로 민감한 분석

- **경로 분할**: 조건문에서 가능한 모든 경로를 탐색하여 ExplodedGraph가 분기[3](https://www.scribd.com/document/452467577/Clang-Analyzer-Guide-v0-1)
    
- **심볼릭 값 처리**: 알려지지 않은 값들을 심볼릭 값으로 표현 (예: `reg_$0<x>`)
    
- **상태 추적**: 각 프로그램 지점에서의 변수 상태를 추적하며 제약 조건 관리
    

## ExplodedGraph 확장

- **CFG 폭발**: CFG의 제어 흐름 엣지들이 "폭발"되어 각 CFG 블록에서 가능한 모든 프로그램 상태 고려[1](https://clang-analyzer.llvm.org/checker_dev_manual.html)
    
- **노드 생성**: 문장별 처리를 통해 새로운 ExplodedNode 생성 및 상태 전이
    

## 4. 체커(Checker) 시스템 및 실행 타이밍

## 체커 구조 및 등록

- **체커 분류**: 8개의 체커 집합에 속한 95개 체커 존재[4](http://www.koreascience.kr/article/JAKO201820765436661.page?lang=ko)
    
- **체커 등록**: `Checkers.td` 파일에서 체커 정의 및 패키지 분류[1](https://clang-analyzer.llvm.org/checker_dev_manual.html)
    
- **체커 상속**: 모든 체커는 `Checker` 템플릿 클래스를 상속받아 구현[1](https://clang-analyzer.llvm.org/checker_dev_manual.html)
    

## 주요 체커 카테고리

- **core**: 기본 체커 (예: `core.DivideZero`, `core.NullDereference`)
    
- **alpha**: 실험적 체커 (예: `alpha.unix.cstring.OutOfBounds`)
    
- **security**: 보안 관련 체커
    
- **unix**: UNIX API 관련 체커
    
- **osx**: macOS 관련 체커
    

## 체커 실행 타이밍과 이벤트 처리

## ExplodedGraph 탐색 중 체커 실행

체커들은 **ExplodedGraph를 탐색하는 과정에서 각 노드마다 실행**됩니다[5](https://stackoverflow.com/questions/49242316/how-to-write-checkers-for-searching-specific-system-calls). 심볼릭 실행 엔진이 ExplodedNode를 생성하고 상태를 전이할 때마다, 등록된 체커들이 해당 이벤트에 대해 순차적으로 호출됩니다.

## 주요 체커 이벤트와 실행 시점

- **`checkPreStmt`**: 문장 실행 **전**에 호출
    
- **`checkPostStmt`**: 문장 실행 **후**에 호출
    
- **`checkPreCall`**: 함수 호출 **직전**에 실행[5](https://stackoverflow.com/questions/49242316/how-to-write-checkers-for-searching-specific-system-calls)
    
- **`checkPostCall`**: 함수 호출 **완료 후**에 실행[5](https://stackoverflow.com/questions/49242316/how-to-write-checkers-for-searching-specific-system-calls)
    
- **`checkBeginFunction`**: 함수 분석 **시작 시**
    
- **`checkEndFunction`**: 함수 분석 **종료 시**
    
- **`checkLocation`**: 메모리 위치 **접근 시** (로드/스토어)
    
- **`checkBind`**: 값이 메모리 위치에 **바인딩될 때**
    
- **`checkDeadSymbols`**: 심볼이 **죽을 때** (스코프 벗어남)
    

## 체커 실행 순서와 구체적 타이밍

```
CFG 블록 처리
    ↓
ExplodedNode 생성
    ↓
해당 이벤트에 구독한 체커들 순차 실행
    ↓
체커 결과에 따른 상태 업데이트
    ↓
다음 ExplodedNode로 전이
```


## 5. 버그 탐지 및 리포트 생성

## 버그 탐지 과정

- **체커 실행**: ExplodedGraph 탐색 중 각 노드에서 등록된 체커들이 순차적으로 실행[1](https://clang-analyzer.llvm.org/checker_dev_manual.html)
    
- **상태 검사**: 체커들이 현재 프로그램 상태를 검사하여 버그 패턴 식별
    
- **경로 추적**: 버그가 발견된 경로를 역추적하여 원인 분석
    

## 리포트 생성

- **BugType 정의**: 버그 유형 및 카테고리 정의[1](https://clang-analyzer.llvm.org/checker_dev_manual.html)
    
- **BugReport 생성**: 구체적인 버그 발생 정보 포함
    
- **경로 다이어그램**: 버그 발생까지의 실행 경로를 시각적으로 표현
    

## 6. 전체 파이프라인 요약 (체커 포함)

```
소스 코드 (.c/.cpp)
    ↓
Translation Unit (TU) 구성
    ↓
AST 생성 (파일 단위)
    ↓
CFG 생성 (함수 단위)
    ↓
ExplodedGraph 구축 (Symbolic Execution)
    ↓ (각 ExplodedNode에서)
체커 실행 (이벤트 기반)
    ├─ checkBeginFunction
    ├─ checkPreStmt/checkPostStmt
    ├─ checkPreCall/checkPostCall
    ├─ checkLocation/checkBind
    ├─ checkDeadSymbols
    └─ checkEndFunction
    ↓
버그 탐지 및 리포트 생성
```

---

## 용어


**AST(추상 구문 트리)와 CFG(제어 흐름 그래프)는** 컴파일러나 정적 분석 도구가 소스 코드를 이해하고 처리하는 데 사용하는 대표적인 내부 구조입니다.  
아래에 두 개념을 **비유와 함께 쉽게** 설명합니다.


## AST (Abstract Syntax Tree, 추상 구문 트리)
![[Pasted image 20250701171239.png]]
- **정의:**  
    소스 코드의 **구조(문법적 계층)**를 트리(Tree) 형태로 표현한 자료구조입니다.
    
- **비유:**  
    소설책을 읽는다고 할 때, AST는 **책의 목차**와 비슷합니다.
    
    - 책(프로그램)은 여러 장(함수, 클래스)으로 나뉘고,
        
    - 각 장은 절(명령문, 선언문)로,
        
    - 절은 문장(연산, 변수 등)으로 나눠집니다.
        
- **특징:**
    
    - 소스 코드의 **문법적 구조**만을 계층적으로 표현
        
    - 괄호, 세미콜론 같은 **구두점은 생략**
        
    - 각 노드는 변수 선언, 연산, 함수 호출 등 의미 있는 코드 단위를 나타냄
        
    - **실행 흐름 정보는 없음**
        
    - 컴파일러의 **의미 분석**, **코드 생성** 등 다양한 단계에서 사용
        

> "AST는 프로그램의 뼈대(구조)를 파악하는 지도"입니다[1](https://wiki.onul.works/w/%EC%B6%94%EC%83%81_%EA%B5%AC%EB%AC%B8_%ED%8A%B8%EB%A6%AC)[2](https://ko.wikipedia.org/wiki/%EC%B6%94%EC%83%81_%EA%B5%AC%EB%AC%B8_%ED%8A%B8%EB%A6%AC)[3](https://blog.realsung.kr/2024/04/28/Static-Analysis-Theory/).

## CFG (Control Flow Graph, 제어 흐름 그래프)
![[Pasted image 20250701172849.png]]
- **정의:**  
    프로그램의 **실행 순서(흐름)**를 그래프(Graph) 형태로 표현한 자료구조입니다.
    
- **비유:**  
    놀이공원의 **길 안내도**와 비슷합니다.
    
    - 입구(시작점)에서 출구(종료점)까지
        
    - 여러 갈림길(조건문, 반복문, 함수 호출 등)을 따라
        
    - 실제로 **어떻게 이동(실행)할 수 있는지**를 보여줍니다.
        
- **특징:**
    
    - **노드:** 여러 문장이 연속적으로 실행되는 코드 블록(기본 블록)
        
    - **엣지(선):** 실행 흐름의 방향(분기, 반복, 함수 호출 등)
        
    - if, for, while 등 **분기/반복 구조**가 모두 반영
        
    - 실제 프로그램이 **어떻게 실행될지**를 분석할 때 사용
        
    - 정적 분석, 최적화, 코드 생성 등에서 핵심 역할
        

> "CFG는 프로그램의 실제 실행 경로를 보여주는 지도"입니다[4](https://pseudo-lab.github.io/CPython-Guide/docs/7_0_compiler.html)[5](https://de-things.tistory.com/5)[3](https://blog.realsung.kr/2024/04/28/Static-Analysis-Theory/).