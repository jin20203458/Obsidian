
```cpp
#ifndef DEFENCESTYLE_IFSTYLECHECK_H

#define DEFENCESTYLE_IFSTYLECHECK_H

  

#include "../ClangTidyCheck.h"

  

namespace clang {

namespace tidy {

namespace defenceStyle {

  

class IfStyleCheck : public ClangTidyCheck {

public:

  IfStyleCheck(StringRef Name, ClangTidyContext *Context)

      : ClangTidyCheck(Name, Context) {}

  

  void registerMatchers(ast_matchers::MatchFinder *Finder) override;

  void check(const ast_matchers::MatchFinder::MatchResult &Result) override;

};

  

} // namespace defenceStyle

} // namespace tidy

} // namespace clang

  

#endif // DEFENCESTYLE_IFSTYLECHECK_H
```


```cpp
#include "IfStyleCheck.h"

#include "clang/ASTMatchers/ASTMatchFinder.h"

  

using namespace clang::ast_matchers;

  

namespace clang {

namespace tidy {

namespace defenceStyle {

  

void IfStyleCheck::registerMatchers(MatchFinder *Finder) {

Finder->addMatcher(

  ifStmt(

    unless(hasElse(stmt()))

  ).bind("ifWithoutElse"),

  this);

}

  

void IfStyleCheck::check(const MatchFinder::MatchResult &Result) {

  if (const auto *IS = Result.Nodes.getNodeAs<IfStmt>("ifWithoutElse"))

   {

    diag(IS->getIfLoc(),

         u8"if 문에는 반드시 else 블록이 필요합니다.");

   }

}

  
  

} // namespace defenceStyle

} // namespace tidy

} // namespace clang
```