
> **ProgramState**는 Clang Static Analyzer에서 **프로그램의 추상적 실행 상태(abstract state of the program)**를 표현하는 핵심 객체입니다.  

참조:
[[ProgramStateManager]]

각 분석 경로(분기, 함수 호출, 반복 등)별로 **ProgramState**가 독립적으로 존재하며,  
이 객체가 프로그램의 변수 값, 메모리 상태, 심볼릭 값, 제약 조건 등 모든 분석 정보를 캡슐화합니다.

## ProgramState의 주요 구성 요소

아래는 공식 문서와 Clang 소스 기준의 구조 요약입니다[5](https://clang-analyzer.llvm.org/checker_dev_manual.html)[6](https://github.com/llvm/llvm-project/blob/main/clang/lib/StaticAnalyzer/README.txt):

|구성 요소|설명|
|---|---|
|**Environment**|소스 코드 내 각 표현식(Expression)에 대한 심볼릭 값(SVal) 매핑|
|**Store**|메모리 지역(MemRegion, 변수/포인터/배열 등) ↔ SVal(값) 매핑|
|**GenericDataMap**|체크커(Checker) 등에서 확장적으로 사용하는 임의의 추가 데이터 맵|
|**Constraints**|심볼릭 값에 대한 제약 조건(분기, 조건문 등에서 발생하는 논리적 제약)|

## ProgramState의 특징

- **불변(immutable) 객체**
    
    - ProgramState는 한 번 생성되면 변경되지 않습니다.  
        새로운 상태가 필요하면 기존 상태에서 복사·수정된 새로운 ProgramState 인스턴스가 만들어집니다[6](https://github.com/llvm/llvm-project/blob/main/clang/lib/StaticAnalyzer/README.txt).
        
- **분석 경로별 독립적 관리**
    
    - if, switch, 함수 호출 등으로 경로가 분기될 때마다  
        각 경로에 맞는 ProgramState가 독립적으로 관리됩니다.
        
- **효율적 공유**
    
    - 내부적으로 불변 자료구조(ImmutableMap 등)를 사용해  
        중복된 데이터는 공유, 변경된 부분만 복사합니다.
        
- **체커와의 상호작용**
    
    - 각 체크커(Checker)는 ProgramState를 통해  
        자신만의 분석 데이터를 안전하게 저장·조회할 수 있습니다[5](https://clang-analyzer.llvm.org/checker_dev_manual.html).
        

## ProgramState의 내부 구조 예시 (TEXT MAP)

```scss
[ProgramState]
   |
   +-- Environment:  { Expr* → SVal }
   |
   +-- Store:        { MemRegion* → SVal }
   |
   +-- GenericDataMap: { 사용자/체커 확장 데이터 }
   |
   +-- Constraints:  { SymbolRef → 조건(논리식) }
```
## ProgramState의 동작 예시

- 변수 x에 1을 대입하면:
    
    - Store에 "x의 MemRegion → SVal(1)"이 저장됨
        
- if (x > 0) 분기 시:
    
    - Constraints에 "x > 0" 또는 "x ≤ 0" 조건이 추가된  
        두 개의 ProgramState가 각각 생성됨
        
- 체크커가 특정 함수 호출을 추적하고 싶으면:
    
    - GenericDataMap에 관련 정보를 저장
        

## 공식 문서 및 소스 근거

- [Clang Static Analyzer Checker Developer Manual][5](https://clang-analyzer.llvm.org/checker_dev_manual.html):
    
    > ProgramState represents abstract state of the program.  
    > It consists of: Environment (mapping from expressions to symbolic values), Store (mapping from memory locations to symbolic values), GenericDataMap (constraints on symbolic values)...
    
- [Clang Static Analyzer README][6](https://github.com/llvm/llvm-project/blob/main/clang/lib/StaticAnalyzer/README.txt):
    
    > The state of the program (values of variables and expressions) is encapsulated by the state (ProgramState).  
    > ProgramState and ExplodedNodes are basically immutable once created.  
    > ...we use functional data structures (e.g., ImmutableMaps) which share data between instances.
    

## 요약

- **ProgramState**는 Clang Static Analyzer에서 프로그램의 모든 추상적 실행 상태(변수 값, 메모리, 조건 등)를 담는 불변 객체입니다.
    
- Environment, Store, Constraints, GenericDataMap 등으로 구성되어  
    경로별, 시점별, 체크커별로 다양한 분석 정보를 효율적으로 관리합니다.
    
- 새로운 상태가 필요할 때마다 복사·수정된 ProgramState가 만들어지며,  
    내부적으로는 불변 자료구조로 효율적 공유가 이뤄집니다.
    

**참고:**

- [5](https://clang-analyzer.llvm.org/checker_dev_manual.html) Clang Static Analyzer Checker Developer Manual
    
- [6](https://github.com/llvm/llvm-project/blob/main/clang/lib/StaticAnalyzer/README.txt) Clang Static Analyzer README  
    (모두 공식 문서 및 소스 기준)
    

1. [http://www.koreascience.kr/article/JAKO201820765436661.page?lang=ko](http://www.koreascience.kr/article/JAKO201820765436661.page?lang=ko)
2. [https://koreascience.kr/article/JAKO202211954997025.view?orgId=anpor](https://koreascience.kr/article/JAKO202211954997025.view?orgId=anpor)
3. [https://nx006.tistory.com/37](https://nx006.tistory.com/37)
4. [https://www.lazenca.net/display/TEC/Clang+Static+Analyzer](https://www.lazenca.net/display/TEC/Clang+Static+Analyzer)
5. [https://clang-analyzer.llvm.org/checker_dev_manual.html](https://clang-analyzer.llvm.org/checker_dev_manual.html)
6. [https://github.com/llvm/llvm-project/blob/main/clang/lib/StaticAnalyzer/README.txt](https://github.com/llvm/llvm-project/blob/main/clang/lib/StaticAnalyzer/README.txt)
7. [https://kthan.tistory.com/entry/%ED%94%84%EB%A1%9C%EA%B7%B8%EB%9E%98%EB%B0%8D-clang%EC%A0%95%EC%A0%81-%EC%86%8C%EC%8A%A4-%EB%B6%84%EC%84%9D%EA%B8%B0%EA%B3%BC-%EA%B2%B0%EA%B3%BC-%EB%B6%84%EC%84%9D%EC%9D%84-%EC%9C%84%ED%95%9C-scan-buildscan-view-%ED%88%B4](https://kthan.tistory.com/entry/%ED%94%84%EB%A1%9C%EA%B7%B8%EB%9E%98%EB%B0%8D-clang%EC%A0%95%EC%A0%81-%EC%86%8C%EC%8A%A4-%EB%B6%84%EC%84%9D%EA%B8%B0%EA%B3%BC-%EA%B2%B0%EA%B3%BC-%EB%B6%84%EC%84%9D%EC%9D%84-%EC%9C%84%ED%95%9C-scan-buildscan-view-%ED%88%B4)
8. [https://uzooin.tistory.com/200](https://uzooin.tistory.com/200)
9. [https://clang-analyzer.llvm.org](https://clang-analyzer.llvm.org/)