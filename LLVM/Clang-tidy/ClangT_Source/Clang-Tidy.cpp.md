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