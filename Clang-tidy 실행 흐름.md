

## 1단계: 준비 (runClangTidy 시작)

```cpp
std::vector<ClangTidyError> runClangTidy(...) {
```

- ClangTool 생성 (컴파일 DB + 분석 대상 파일)
    
- 진단 Consumer 설정
    
- **ActionFactory 생성** (아직 체커 인스턴스는 생성 안 됨)
    

## 2단계: 분석 시작

```cpp
ActionFactory Factory(Context, BaseFS);
Tool.run(&Factory);  // ← 여기서 실제 분석 시작
```

## 3단계: 각 파일마다 반복

ClangTool이 각 소스 파일에 대해:

```scss
파일 1 (test.cpp):
  ↓
1. ActionFactory::create() 호출
  → Action 인스턴스 생성
  ↓
2. Action::CreateASTConsumer() 호출
  → ClangTidyASTConsumerFactory::createASTConsumer() 호출
  ↓
3. createASTConsumer 내부:
  ① 활성화된 체커 인스턴스 생성 (Checks 벡터)
  ② MatchFinder 생성
  ③ 모든 체커가 Matcher 등록
  ④ Finder->newASTConsumer() 로 Consumer 생성
  ⑤ Static Analyzer Consumer 생성 (있다면)
  ⑥ Consumers 벡터에 모두 추가
  ⑦ ClangTidyASTConsumer 반환
  ↓
4. Clang 컴파일러가 AST 생성
  ↓
5. ClangTidyASTConsumer::HandleTranslationUnit() 호출
  → 내부의 모든 Consumer 실행
  → Clang-Tidy 체커 실행
  → Static Analyzer 실행
  ↓
6. 진단 결과 수집
```

## 4단계: 결과 처리

```cpp
return DiagConsumer.take();  // 모든 파일 분석 후 진단 반환
```


## 시각화

```scss
runClangTidy()
    ↓
Tool.run(&Factory)
    ↓
[각 파일마다]
    ↓
Factory.create() → Action
    ↓
Action.CreateASTConsumer() → ClangTidyASTConsumerFactory
    ↓
createASTConsumer() {
    체커 인스턴스 생성 (Checks)
    Matcher 등록 (Finder)
    Consumer 생성 (Consumers)
    반환
}
    ↓
Clang AST 생성
    ↓
Consumer.HandleTranslationUnit() {
    모든 체커 실행
    Static Analyzer 실행
}
    ↓
진단 결과 수집
```

## 중요한 차이점

1. **체커 인스턴스는 각 파일마다 새로 생성됨**
    
    - `test.cpp` 분석 시 체커 생성
        
    - `main.cpp` 분석 시 다시 체커 생성
        
2. **Consumer는 Action이 생성함**
    
    - Action이 먼저 만들어짐
        
    - Action이 Consumer를 생성 요청
        
    - Consumer가 체커를 초기화
        
3. **실행 순서**
    
    - ClangTool 실행 → 파일별로 Action → Action이 Consumer 생성 → Consumer가 체커 실행

