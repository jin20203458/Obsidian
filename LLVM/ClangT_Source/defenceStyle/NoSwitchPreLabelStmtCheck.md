

```cpp
#ifndef DEFENCESTYLE_NOSWITCHPRELABELSTMT_H
#define DEFENCESTYLE_NOSWITCHPRELABELSTMT_H

  
#include "../ClangTidyCheck.h"


namespace clang {
namespace tidy {
namespace defenceStyle {


class NoSwitchPreLabelStmtCheck : public ClangTidyCheck {

public:
  NoSwitchPreLabelStmtCheck(StringRef Name, ClangTidyContext *Context)
      : ClangTidyCheck(Name, Context) {}

  void registerMatchers(ast_matchers::MatchFinder *Finder) override;
  void check(const ast_matchers::MatchFinder::MatchResult &Result) override;
};
 

} // namespace defenceStyle
} // namespace tidy
} // namespace clang


#endif // DEFENCESTYLE_NOSWITCHPRELABELSTMT_H
```

```cpp
#include "NoSwitchPreLabelStmtCheck.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace defenceStyle {

void NoSwitchPreLabelStmtCheck::registerMatchers(MatchFinder *Finder) {
  Finder->addMatcher(switchStmt().bind("switch"), this);
}

void NoSwitchPreLabelStmtCheck::check(const MatchFinder::MatchResult &Result) {
  const auto *SS = Result.Nodes.getNodeAs<SwitchStmt>("switch");
  if (!SS)
    return;

  const CompoundStmt *Body = dyn_cast_or_null<CompoundStmt>(SS->getBody());
  if (!Body)
    return;

  // switch 문을 순회하며 case 또는 default label 이전에 오는 구문 검사
  for (const Stmt *S : Body->body()) {
    if (isa<CaseStmt>(S) || isa<DefaultStmt>(S)) {
      break;
    }

    // label 전의 statement 탐지
    if (!isa<NullStmt>(S)) {
      diag(S->getBeginLoc(), u8"switch 블록의 첫 번째 case/default label "
                             u8"전에는 코드가 올 수 없습니다.");
      break;
    }
  }
}

} // namespace defenceStyle
} // namespace tidy
} // namespace clang
```