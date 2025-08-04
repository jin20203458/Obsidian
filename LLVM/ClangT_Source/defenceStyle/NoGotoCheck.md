
```cpp
#ifndef DEFENCESTYLE_NOGOTOCHECK_H
#define DEFENCESTYLE_NOGOTOCHECK_H

#include "../ClangTidyCheck.h"

  
namespace clang {
namespace tidy {
namespace defenceStyle {

class NoGotoCheck : public ClangTidyCheck {

public:
  NoGotoCheck(StringRef Name, ClangTidyContext *Context)
      : ClangTidyCheck(Name, Context) {}

  void registerMatchers(ast_matchers::MatchFinder *Finder) override;
  void check(const ast_matchers::MatchFinder::MatchResult &Result) override;
};


} // namespace defenceStyle
} // namespace tidy
} // namespace clang

#endif // DEFENCESTYLE_NOGOTOCHECK_H
```

```cpp
#include "NoGotoCheck.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace defenceStyle {

void NoGotoCheck::registerMatchers(MatchFinder *Finder) {
  Finder->addMatcher(gotoStmt().bind("goto"), this);
}

void NoGotoCheck::check(const MatchFinder::MatchResult &Result) {
  const auto *GS = Result.Nodes.getNodeAs<GotoStmt>("goto");
  if (GS) 
  {
    diag(GS->getGotoLoc(), u8"goto 문은 사용하지 마십시오.");
  }
}

} // namespace defenceStyle
} // namespace tidy
} // namespace clang
```
