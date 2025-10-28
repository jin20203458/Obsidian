[[Clang AST에서의 배열 타입 저장 방식]]
[[다차원 배열 차원 해석 & AST 타입 계층]]
```cpp
int data[2][3][4]  // int 4개를 가진 배열을 3개 가진 배열을 2개 가진 배열
```
```scss
data[0]:        // 첫 번째 [3][4] 배열
  ├── [0]: int[4]  // int 4개
  ├── [1]: int[4]  // int 4개  
  └── [2]: int[4]  // int 4개

data[1]:        // 두 번째 [3][4] 배열
  ├── [0]: int[4]  // int 4개
  ├── [1]: int[4]  // int 4개
  └── [2]: int[4]  // int 4개

ConstantArrayTypeLoc[2]     ← 1회차: 가장 바깥쪽
└── ConstantArrayTypeLoc[3] ← 2회차: 중간
    └── ConstantArrayTypeLoc[4] ← 3회차: 안쪽
        └── BuiltinTypeLoc(int) ← 4회차: 가장 안쪽

Type (기본 클래스)
├── ArrayType (배열 기본 클래스)
│   ├── ConstantArrayType      // [N] 크기 명시 배열
│   ├── IncompleteArrayType    // [] 크기 생략 배열
│   └── VariableArrayType      // VLA (Variable Length Array)
└── BuiltinType               // int, char 등 기본 타입
```


```shell
Checks: '-*,defence-style-char-array-string-size'
CheckOptions:
  - key: defence-style-char-array-string-size.RequireExactSize
    value: 'false'   # 기본 false. true면 정확히 len+1 강제
  - key: defence-style-char-array-string-size.OfferFixIt
    value: 'true'    # [N]에서 N이 정수 리터럴일 때 자동 교체 제안

ex)
clang-tidy .\*.cpp -checks="-*,defence-style-char-array-string-size" -- -std=c++17 -w

clang-tidy .\*.cpp -checks="-*,defence-style-char-array-string-size" -config="{CheckOptions: {defence-style-char-array-string-size.RequireExactSize: true}}" -- -std=c++17 -w

```


```cpp
#ifndef DEFENCESTYLE_CHARARRAYSTRINGSIZECHECK_H
#define DEFENCESTYLE_CHARARRAYSTRINGSIZECHECK_H

#include "../ClangTidyCheck.h"

namespace clang {
namespace tidy {
namespace defenceStyle {

/// 문자열 리터럴로 초기화되는 char 배열의 크기가
/// (문자 수 + 1)보다 작은 경우를 진단.
/// 옵션 RequireExactSize=true 이면 정확히 == (문자 수 + 1)도 강제.
class CharArrayStringSizeCheck : public ClangTidyCheck {
public:
  CharArrayStringSizeCheck(StringRef Name, ClangTidyContext *Context)
      : ClangTidyCheck(Name, Context),
        RequireExactSize(Options.get("RequireExactSize", false)),
        OfferFixIt(Options.get("OfferFixIt", true)) {}

  void registerMatchers(ast_matchers::MatchFinder *Finder) override;
  void check(const ast_matchers::MatchFinder::MatchResult &Result) override;

  void storeOptions(ClangTidyOptions::OptionMap &Opts) override {
    Options.store(Opts, "RequireExactSize", RequireExactSize);
    Options.store(Opts, "OfferFixIt",       OfferFixIt);
  }

private:
  const bool RequireExactSize; // true: 크기를 정확히 len+1로 강제
  const bool OfferFixIt;       // true: 단순 정수 리터럴일 때 고쳐주기
};

} // namespace defenceStyle
} // namespace tidy
} // namespace clang

#endif // DEFENCESTYLE_CHARARRAYSTRINGSIZECHECK_H
```
