참조:
[[Clang-tidy]]
[[ASTContext]]
[[AST-based Checking]]

## 1. **목적**

Clang-Tidy는 C/C++ 소스 코드에서 코딩 스타일, 문법, 간단한 버그, 코드 관례 위반 등을  
**소스코드 구조(AST)를 기반으로 정적으로 검사하고 자동으로 리팩토링 제언도 제공하는 도구**입니다.

## 2. **분석 과정 개요**

Clang-Tidy의 동작 흐름은 다음과 같습니다.

|단계|설명|
|---|---|
|1. **코드 파싱**|Clang 프론트엔드가 소스 코드를 구문 분석해 AST(Abstract Syntax Tree)를 생성|
|2. **ASTMatcher 등록**|검사할 문법/패턴(예: switch문, goto문, 변수명 등)에 대한 AST 매처(ASTMatcher)를 정의|
|3. **AST 트리 순회**|만들어진 AST를 순회하며, 모든 노드가 매처 조건에 맞는지 검사|
|4. **check() 콜백**|조건에 합당한 AST 노드가 발견될 때마다 해당 검사(check) 함수 호출하여 진단 수행|
|5. **진단 결과 출력**|위반 사항을 진단(diagnostic) 메시지로 출력하며, IDE 혹은 터미널에 경고/오류를 표시|
|6. **자동 리팩터링**|경우에 따라 코드 자동 수정을 위한 Fix-it 제안 및 적용 가능|

## 3. **핵심 개념 요약**

|개념|설명|
|---|---|
|**AST**|소스의 문법 구조 및 코드 구성 트리|
|**ASTMatcher**|특정 구문 패턴(노드)을 찾기 위한 매칭 도구|
|**check() 함수**|매칭된 노드를 대상으로 검사를 수행하는 콜백 함수|
|**진단 메시지(diag)**|위반 지점과 내용을 사용자에게 알리는 경고 또는 오류 메시지|
|**Fix-it Hint**|진단과 함께 제안하는 코드 자동 수정 제안|

## 4. **예시 (goto 사용 금지 체크)**

```cpp
void NoGotoCheck::registerMatchers(MatchFinder *Finder) {
  Finder->addMatcher(gotoStmt().bind("goto"), this);
}

void NoGotoCheck::check(const MatchFinder::MatchResult &Result) {
  const auto *GS = Result.Nodes.getNodeAs<GotoStmt>("goto");
  if (GS) {
    diag(GS->getGotoLoc(), u8"goto 문은 사용하지 마십시오.");
  }
}
```

- `gotoStmt()`를 AST에서 찾아내고,
    
- 발견 시 `"goto 문은 사용하지 마십시오."`라는 경고를 출력함
    

## 5. **장점 / 단점**

|장점|단점|
|---|---|
|빠른 검사 속도 및 경량화|실행 경로나 변수 상태 추적 불가|
|확장성이 뛰어나 커스텀 검사 쉽고, 다양한 규칙 추가 가능|복잡한 런타임 버그 탐지는 제한적|
|코드 스타일 일관성 자동 점검 가능|false negative 가능성 내포|
|자동 Fix-it 적용으로 일관된 코드 스타일 유지에 도움||

## 6. **결론**

Clang-Tidy는 클랭 기반 AST를 활용하여 코드를 구조적으로 분석하는 **린터 스타일 정적 검사기**입니다.

- 코드 전체의 AST를 한 번 탐색하여 사용자가 정의한 규칙에 맞게 특정 구문 노드를 찾아내고
    
- 위반 사항을 진단 메시지로 출력하며, 필요시 자동 수정 제안까지 제공합니다.

증분빌드 명령어
```
cmake --build . --config Release --target clang-tidy --parallel 12
```
체커사용 예시
```
clang-tidy .\*.cpp -checks="-*,defence-style-no-out-of-range-assignment" -- -std=c++17 -w


```


