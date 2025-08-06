

```cpp
#ifndef DEFENCESTYLE_SWITCHSTYLECHECK_H
#define DEFENCESTYLE_SWITCHSTYLECHECK_H

#include "../ClangTidyCheck.h"

namespace clang {
namespace tidy {
namespace defenceStyle {

class SwitchStyleCheck : public ClangTidyCheck {
public:
  SwitchStyleCheck(StringRef Name, ClangTidyContext *Context)
      : ClangTidyCheck(Name, Context) {}

  void registerMatchers(ast_matchers::MatchFinder *Finder) override;
  void check(const ast_matchers::MatchFinder::MatchResult &Result) override;
};

} // namespace defenceStyle
} // namespace tidy
} // namespace clang

#endif // DEFENCESTYLE_SWITCHSTYLECHECK_H

```

```cpp
#include "SwitchStyleCheck.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace defenceStyle {

void SwitchStyleCheck::registerMatchers(MatchFinder *Finder) {
  Finder->addMatcher(switchStmt().bind("switch"), this);
}

void SwitchStyleCheck::check(const MatchFinder::MatchResult &Result) {
  const auto *SS = Result.Nodes.getNodeAs<SwitchStmt>("switch");
  if (!SS) return;

  const CompoundStmt *Body = dyn_cast_or_null<CompoundStmt>(SS->getBody());
  if (!Body) return;

  // 1. switch 블록에서 첫 번째 label 전 코드가 있으면 경고
  for (const Stmt *S : Body->body()) 
  {
    if (isa<CaseStmt>(S) || isa<DefaultStmt>(S))
    {
      break;
    }
    if (!isa<NullStmt>(S)) 
    {
      diag(S->getBeginLoc(),
           u8"switch 블록의 첫 번째 case/default label 전에는 코드가 올 수 없습니다.");
      break;
    }
  }

  // 2. default label 포함 여부 검사
  bool hasDefault = false;
  for (const Stmt *S : Body->body()) 
  {
    if (isa<DefaultStmt>(S))
    {
      hasDefault = true;
      break;
    }
  }
  if (!hasDefault) 
  {
    diag(SS->getSwitchLoc(),
         u8"switch문에는 반드시 default label이 포함되어야 합니다.");
  }
}

} // namespace defenceStyle
} // namespace tidy
} // namespace clang

```