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

 **함수 이름**: `FlushDiagnosticsImpl` (부분)

**역할**: Static Analyzer가 발견한 버그들을 Clang-Tidy 형식으로 변환하여 출력


---

### `class ErrorReporter`

**역할**: Clang-Tidy 진단 메시지 출력 및 자동 수정을 담당하는 객체 초기화
