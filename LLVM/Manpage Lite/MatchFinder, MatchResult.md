
> **MatchFinder(파인더)**는 전체 코드(AST)를 탐색하다가, 등록된 패턴(matcher)과 일치하는 노드를 찾으면  
  자동으로 해당 콜백 함수(예: 각 체크의 `check` 메서드 등)를 호출

참조:
[[AST-based Checking]]
## 1. MatchFinder란?

**MatchFinder**는 Clang AST(추상 구문 트리) 탐색 엔진에서  
특정한 코드 패턴(함수 선언, if문, 변수 정의 등)을 찾기 위해 사용되는 "검색 도우미" 역할을 합니다.

- **주 역할**:
    
    - 사용자가 정의한 여러 "Pattern"에 해당하는 AST 노드를 자동으로 탐색
        
    - 패턴에 맞는 노드가 발견될 때마다 해당 matcher와 연결된 콜백(check 함수 등) 호출
        
- **비유**:  
    마치 공장 자동화 라인에서 결함있는 제품을 찾는 "자동 감지센서"와 같습니다.  
    설정된 조건에 일치하는 제품(코드 패턴)을 찾으면 검수 담당자에게 전달합니다.
    

## 주요 메서드

- `addMatcher(Matcher, Callback)`
    
    - 패턴과 콜백 함수를 연결하여 등록
        
    - 예: `functionDecl(isDefinition()).bind("func"), this`
        

## 2. MatchResult란?

**MatchResult**는 '탐색한 패턴에 적합한 AST 노드와 그 관련 환경정보'를 하나의 패키지로 담아  
콜백 함수(예: check)로 전달하는 역할을 합니다.

- **포함 내용**:
    
    - 매칭된 구체 AST 노드 (`MatchResult.Nodes`)
        
    - ASTContext, SourceManager 등 코드 위치와 타입 분석에 필요한 내부 객체
        
- **비유**:  
    "센서"가 감지한 제품과 관련 생산정보, 시간 등을 검수자에게 한 묶음으로 넘기는 것.
    

## 3. 논리적 흐름 (실행 순서)

```cpp
void NoReturnTypeMismatchCheck::registerMatchers(MatchFinder *Finder) {
  Finder->addMatcher(functionDecl(isDefinition()).bind("func"), this);
}
```

1. **registerMatchers**:
    
    - `MatchFinder`에 "함수 정의" 패턴(functionDecl(isDefinition()))을 등록
        
    - "func"라는 키로 바인딩
        
    - 이때 this는 해당 체크의 콜백(아래 check 함수)이 됨
        
2. **ClangTidy의 실행**:
    
    - 전체 C++ AST 노드를 순회하며, 등록된 matcher들이 조건에 맞는 노드를 자동 탐색
        
3. **매칭된 경우**:
    
    - 일치하는 함수 선언이 발견되면, check 콜백이 자동으로 호출됨
        
    - 이때 `const MatchFinder::MatchResult &Result` 인자가 전달됨
        
```cpp
void NoReturnTypeMismatchCheck::check(const MatchFinder::MatchResult &Result) {
  const auto *FD = Result.Nodes.getNodeAs<FunctionDecl>("func");
  // ...
}
```

4. **MatchResult 해석 및 사용**:
    
    - check 함수는 MatchResult로부터 "func"라는 키에 해당하는 FunctionDecl 노드를 추출
        
    - ASTContext 등 보조 정보도 Result에서 사용 가능
        
    - 이후 실제 로직(반환 타입 검사 등) 수행
        

## 4. 전체 논리적 흐름 요약

|단계|역할 및 동작|
|---|---|
|1. 패턴 등록|MatchFinder에 찾고자 하는 코드 패턴(matcher)와 검사 함수(check)를 연결함|
|2. 전체 탐색|AST 전체에서 조건에 맞는 노드를 MatchFinder가 자동 감지|
|3. 매칭 발생|패턴이 일치하는 노드를 찾으면, 콜백(check)이 MatchResult와 함께 호출|
|4. 결과 활용|check에서 MatchResult로 매칭된 AST 노드 등 정보를 받아 상세 검사 및 진단 메시지 출력|

## 한줄 요약

**MatchFinder**는 코드 구조를 자동 탐색해, **MatchResult**(노드 정보 패키지)가 check 함수로 넘어가고, 여기서 원하는 정적 분석이 수행됩니다.  
패턴->탐색->매칭&콜백->상세분석의 순환적 과정이 Clang Tidy의 핵심입니다.