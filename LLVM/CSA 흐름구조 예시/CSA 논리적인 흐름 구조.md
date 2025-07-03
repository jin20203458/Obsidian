CSA(Clang Static Analyzer)의 논리적인 흐름 구조는 **코드의 제어 흐름과 데이터 흐름을 따라가며, 다양한 체커(checker)를 통해 소스 코드의 잠재적 결함을 탐지**하는 방식으로 구성됩니다. 주요 단계와 구조는 다음과 같습니다:

##  0. **분석 대상 코드의 중간 표현(IR) 생성**

- Clang 프론트엔드는 입력된 C/C++/Objective-C 소스 코드를 **중간 언어(Intermediate Representation, IR)**로 변환합니다. 이 과정에서 코드의 구조와 의미가 분석에 적합한 형태로 추상화됩니다[1](http://www.koreascience.kr/article/JAKO201820765436661.page?lang=ko).

##  0. **체커(Checker) 선택 및 적용**

- 사용자는 분석하고자 하는 보안 약점 또는 버그 유형에 맞는 **체커**를 선택할 수 있습니다.
    
- 각 체커는 메모리 누수, 널 포인터 역참조, 버퍼 오버플로우 등 특정 결함 유형을 탐지하는 논리를 내장하고 있습니다[1](http://www.koreascience.kr/article/JAKO201820765436661.page?lang=ko).

	
---
## 1. **CFG 생성 및 전처리**

- 소스 코드는 먼저 Clang 파서에 의해 **AST(Abstract Syntax Tree)**로 변환됩니다[1](https://www.themoonlight.io/ko/review/c-analyzer-a-static-program-analysis-tool-for-c-programs)[8](https://kthan.tistory.com/entry/%ED%94%84%EB%A1%9C%EA%B7%B8%EB%9E%98%EB%B0%8D-clang%EC%A0%95%EC%A0%81-%EC%86%8C%EC%8A%A4-%EB%B6%84%EC%84%9D%EA%B8%B0%EA%B3%BC-%EA%B2%B0%EA%B3%BC-%EB%B6%84%EC%84%9D%EC%9D%84-%EC%9C%84%ED%95%9C-scan-buildscan-view-%ED%88%B4).
    
- AST를 기반으로 **CFG(제어 흐름 그래프)**가 생성됩니다. CFG는 코드의 실행 경로를 노드(기본 블록)와 간선(제어 흐름)으로 표현합니다[1](https://www.themoonlight.io/ko/review/c-analyzer-a-static-program-analysis-tool-for-c-programs)[7](https://stackoverflow.com/questions/22371789/how-to-make-the-clang-static-analyzer-output-its-working-from-command-line).
    
- 생성된 CFG는 전처리 단계를 거칩니다:
    
    - `blockList`: 블록 순서 최적화 (루프 백 에지, `break`, `goto` 처리).
        
    - `edgeMatrix`: 블록 간 엣지 정보 정리[1](https://www.themoonlight.io/ko/review/c-analyzer-a-static-program-analysis-tool-for-c-programs).
        

## 2. **추상 도메인 초기화**

- 사용자가 선택한 추상 도메인(**Interval**, **Octagon**, **Polyhedra**, **Bit Vector** 등)이 초기화됩니다[1](https://www.themoonlight.io/ko/review/c-analyzer-a-static-program-analysis-tool-for-c-programs).
    
- 각 도메인은 `Analyzer` 클래스를 상속받아 도메인별 연산을 구현합니다.
    

## 3. **CFG 순회 및 심볼릭 실행**

- **CFG 완성 후**, 분석기는 블록 단위로 순회를 시작합니다[1](https://www.themoonlight.io/ko/review/c-analyzer-a-static-program-analysis-tool-for-c-programs)[7](https://stackoverflow.com/questions/22371789/how-to-make-the-clang-static-analyzer-output-its-working-from-command-line).
    
- **심볼릭 실행**이 이 단계에서 수행됩니다:
    
    - 변수의 실제 값 대신 **심볼릭 상태**를 추적합니다.
        
    - 조건 분기 시 가능한 경로를 모두 탐색하며, 각 경로에서 변수의 상태를 추론합니다[1](https://www.themoonlight.io/ko/review/c-analyzer-a-static-program-analysis-tool-for-c-programs)[7](https://stackoverflow.com/questions/22371789/how-to-make-the-clang-static-analyzer-output-its-working-from-command-line).
        
- 연산 적용:
    
    - **Join**: 여러 경로의 추상 값 결합 (일반화).
        
    - **Meet**: 조건 분기 시 추상 값 겹침 (구체화)[1](https://www.themoonlight.io/ko/review/c-analyzer-a-static-program-analysis-tool-for-c-programs).
        

## 4. **체커 기반 결함 탐지**

- 심볼릭 실행으로 얻은 상태 정보를 바탕으로 **체커(checker)**가 동작합니다.
    
    - 예: 널 포인터 역참조 체커는 포인터의 널 가능성 경로를 검출[7](https://stackoverflow.com/questions/22371789/how-to-make-the-clang-static-analyzer-output-its-working-from-command-line)[8](https://kthan.tistory.com/entry/%ED%94%84%EB%A1%9C%EA%B7%B8%EB%9E%98%EB%B0%8D-clang%EC%A0%95%EC%A0%81-%EC%86%8C%EC%8A%A4-%EB%B6%84%EC%84%9D%EA%B8%B0%EA%B3%BC-%EA%B2%B0%EA%B3%BC-%EB%B6%84%EC%84%9D%EC%9D%84-%EC%9C%84%ED%95%9C-scan-buildscan-view-%ED%88%B4).
        
- 발견된 결함은 경로, 코드 위치, 추론 과정과 함께 리포트됩니다[7](https://stackoverflow.com/questions/22371789/how-to-make-the-clang-static-analyzer-output-its-working-from-command-line)[8](https://kthan.tistory.com/entry/%ED%94%84%EB%A1%9C%EA%B7%B8%EB%9E%98%EB%B0%8D-clang%EC%A0%95%EC%A0%81-%EC%86%8C%EC%8A%A4-%EB%B6%84%EC%84%9D%EA%B8%B0%EA%B3%BC-%EA%B2%B0%EA%B3%BC-%EB%B6%84%EC%84%9D%EC%9D%84-%EC%9C%84%ED%95%9C-scan-buildscan-view-%ED%88%B4).
    

## 요약: CSA의 분석 단계

|단계|설명|
|---|---|
|1. AST 생성|소스 코드 → 추상 구문 트리 변환|
|2. CFG 빌드|AST → 제어 흐름 그래프 생성 및 전처리|
|**3. 심볼릭 실행**|**CFG 기반 경로 탐색 및 심볼릭 상태 추적**|
|4. 체커 적용|결함 탐지 및 리포트|


---

## 용어


**AST(추상 구문 트리)와 CFG(제어 흐름 그래프)**는 컴파일러나 정적 분석 도구가 소스 코드를 이해하고 처리하는 데 사용하는 대표적인 내부 구조입니다.  
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