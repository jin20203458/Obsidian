```scss
1. main()에서 runClangTidy() 호출

2. ClangTidyASTConsumerFactory 생성
   → 레지스트리 순회
   → 모든 모듈에서 체크 등록

3. 각 소스 파일마다:
   createASTConsumer() 호출
   → 활성화된 체크 생성
   → MatchFinder에 매처 등록
   → Static Analyzer 설정

4. AST 파싱 및 분석
   → 매처가 AST 노드 찾음
   → 체크가 문제 발견
   → 진단 메시지 생성

5. handleErrors() 호출
   → ErrorReporter가 메시지 출력
   → 자동 수정 적용 (옵션)

6. 결과 반환
```

`static const char *AnalyzerCheckNamePrefix = "clang-analyzer-";` << 기존
`static const char *AnalyzerCheckNamePrefix = "path-sensitivity-";` << 변경


--- 

### `class AnalyzerDiagnosticConsumer`

**역할**: Static Analyzer의 진단 결과를 Clang-Tidy 형식으로 변환
**상속**: `ento::PathDiagnosticConsumer` (Static Analyzer의 진단 소비자 인터페이스)

**함수명**: `FlushDiagnosticsImpl` (부분)
**역할**: Static Analyzer가 발견한 버그들을 Clang-Tidy 형식으로 변환하여 출력


---

### `class ErrorReporter`

**역할**: Clang-Tidy 진단 메시지 출력 및 자동 수정을 담당하는 객체 초기화


**함수명**: `ErrorReporter` (생성자)
**역할**: Clang-Tidy 진단 메시지 출력 및 자동 수정을 담당하는 객체 초기화

**함수명**: `reportDiagnostic`
**역할**: `ClangTidyCheck`가 발견한 단일 에러(`ClangTidyError`)를 받아, 사용자에게 보여줄 진단 메시지를 생성하고 자동 수정(`Fix-It`)을 준비합니다.


---

### `class ClangTidyASTConsumer`

**역할**: `ClangTidyASTConsumerFactory`가 생성하는 **최종 AST 소비자(Consumer)**입니다. 여러 개의 하위 AST 소비자(예: `ASTMatchFinder`가 생성한 소비자, `Static Analyzer`가 생성한 소비자)를 **하나로 묶어 관리**합니다. AST가 파싱되면, 이 객체가 AST를 받아서 등록된 모든 하위 소비자들에게 **"분배"**하는 역할을 합니다. 또한, 분석에 사용된 `MatchFinder`, `ClangTidyCheck` 객체, `Profiling` 객체들의 **소유권(lifetime)을 관리**합니다. (주석에 명시된 대로, 특히 소멸자 순서가 중요합니다.)

**상속**: `MultiplexConsumer` (여러 `ASTConsumer`에게 AST 이벤트를 전달하는 분배기 클래스)

**함수명**: `ClangTidyASTConsumer` (생성자) **역할**: 분석에 필요한 모든 구성 요소(하위 소비자 목록 `Consumers`, `MatchFinder`, `Checks` 벡터, `Profiling` 객체)를 `std::move`를 통해 소유권을 이전받아 멤버 변수로 저장합니다. 하위 소비자 목록은 부모 클래스인 `MultiplexConsumer`의 생성자로 전달합니다.


# ClangTidy.cpp 실행 논리 흐름 (구조도)

질문해주신 흐름을 코드 구조대로 최대한 명확하게 설명합니다. 핵심은 **체크 팩토리 ➔ 액션 팩토리 ➔ 액션 ➔ AST 생성 및 분석**의 실행입니다.

---

## 1. ClangTidyASTConsumerFactory

- 역할: 체크 모듈(플러그인)의 팩토리들을 담아서, 체크, 프로파일링, AST매처를 준비.
    
- 체크 등록 후 `createASTConsumer`에서 실제로 AST Consumer와 매처 객체를 생성
    

## 2. runClangTidy

- **ClangTool** 생성: 여러 입력파일을 분석할 툴
    
- 내부적으로 파일별 컴파일 단위로 동작
    
- **ActionFactory 생성**: 아래 Action 객체를 직접 생성하는 팩토리 클래스
    

## 3. ActionFactory

- `create()`에서 FrontendAction을 생성 (주로 ASTFrontendAction을 반환)
    
- 내부적으로 `Action` 클래스(FrontendAction 상속)를 생성
    
- `CreateASTConsumer()`에서 **ClangTidyASTConsumerFactory의 createASTConsumer**를 호출해 파일별 ASTConsumer 객체를 생성
    
- 즉, 파일별로 AST Consumer가 만들어짐
    

## 4. Tool.run(&Factory)

- 각 파일별로 `ActionFactory.create()`를 통해 FrontendAction을 생성
    
    - 예: 파일1 ➔ FrontendAction1
        
    - 파일2 ➔ FrontendAction2
        
    - ...
        
- FrontendAction이 실행되면 내부적으로 Clang 파서가 **AST를 생성**
    
- 이 과정에서 `createASTConsumer`가 호출되고, 각 체크가 AST 매처 또는 PP 콜백을 등록함
    

## 5. AST 분석 및 체크 실행

- AST가 생성되자 마자 체크 로직이 실행되어
    
    - AST를 매칭하여 진단 메시지 작성
        
    - 필요하면 자동 수정(Fix) 제안
        

---

## 핵심 흐름 요약 (코드 기반)

```scss
1. runClangTidy(
      ...
      Tool.run(&Factory)                 // 모든 파일 반복 분석
        ↪  for file in InputFiles:
             action = Factory.create()
             action.CreateASTConsumer(...)
               ↪ ClangTidyASTConsumerFactory.createASTConsumer(...)
                  ↪ ASTConsumer/매처/체크 생성
               (AST 생성: Compiler 파서 실행)
             (분석, 진단, 수정 제안)
```
---

## 즉, 실행 구조는

- **ClangTidyASTConsumerFactory** ➔ 체크, ASTConsumer, 매처 준비
    
- **ActionFactory** ➔ 파일마다 FrontendAction(Action) 생성
    
- **FrontendAction** ➔ AST 생성, ASTConsumer로 체크 정적분석 실행
    
- **Tool.run(&Factory)** ➔ 다수 파일 반복 수행
    

---

## 추가 설명

- 이 구조는 각 source파일 별로 **독립적인 AST와 체크 실행** 구조를 만듭니다.
    
- 커스텀 체크를 만들고 싶을 때, ASTConsumerFactory/CheckFactory를 확장해 별도 체크를 등록하면 됩니다.
    
- 모든 흐름은 Clang의 표준 FrontendAction/ASTConsumer 오브젝트 라이프사이클을 따릅니다.
    

---

궁금한 부분, 예: 체크 클래스와 Consumer 내부 또는 AST 매처 동작 상세도 추가로 질문해주시면 곧바로 따라가드립니다!

(더 세부적으로 각 객체/클래스의 연결 과정을 함수별로 단계별 소스/호출 흐름으로 파고들 수도 있습니다.)
