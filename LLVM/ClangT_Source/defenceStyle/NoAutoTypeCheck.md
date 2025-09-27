
```cpp
#ifndef DEFENCESTYLE_NOAUTOTYPECHECK_H
#define DEFENCESTYLE_NOAUTOTYPECHECK_H

#include "../ClangTidyCheck.h"

namespace clang {
namespace tidy {
namespace defenceStyle {

class NoAutoTypeCheck : public ClangTidyCheck {
public:
  NoAutoTypeCheck(StringRef Name, ClangTidyContext *Context)
      : ClangTidyCheck(Name, Context) {}

  void registerMatchers(ast_matchers::MatchFinder *Finder) override;
  void check(const ast_matchers::MatchFinder::MatchResult &Result) override;
};


} // namespace defenceStyle
} // namespace tidy
} // namespace clang

#endif // DEFENCESTYLE_NOAUTOTYPECHECK_H
```

```cpp
#include "NoAutoTypeCheck.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace defenceStyle {

void NoAutoTypeCheck::registerMatchers(MatchFinder *Finder) {
  // 변수 선언에서 auto
   Finder->addMatcher(
        varDecl(
            isExpansionInMainFile(), 
            hasType(autoType())
        ).bind("autoVar"), 
        this);
  // 함수 선언에서 auto 반환형 (C++14 이상 지원)
  Finder->addMatcher(
        functionDecl(
            isExpansionInMainFile(), 
            returns(autoType())
        ).bind("autoFunc"), 
        this);
}

void NoAutoTypeCheck::check(const MatchFinder::MatchResult &Result) {

  if (const auto *VD = Result.Nodes.getNodeAs<VarDecl>("autoVar")) 
  {
    diag(VD->getBeginLoc(),
         u8"변수 선언 시 'auto' 사용을 금지합니다. 타입을 명시하세요.");
    return;
  }
  if (const auto *FD = Result.Nodes.getNodeAs<FunctionDecl>("autoFunc")) 
  {
    diag(FD->getBeginLoc(), u8"함수 선언 시 'auto' 반환 타입 사용을 "
                            u8"금지합니다. 반환 타입을 명시하세요.");
    return;
  }
}

} // namespace defenceStyle
} // namespace tidy
} // namespace clang

```