
```cpp
#ifndef DEFENCESTYLE_NOMEANINGLESEXPRCHECK_H
#define DEFENCESTYLE_NOMEANINGLESEXPRCHECK_H

#include "../ClangTidyCheck.h"

namespace clang {
namespace tidy {
namespace defenceStyle {

class NoMeaninglessExprCheck : public ClangTidyCheck {
public:
  NoMeaninglessExprCheck(StringRef Name, ClangTidyContext *Context)
      : ClangTidyCheck(Name, Context) {}

  void registerMatchers(ast_matchers::MatchFinder *Finder) override;
  void check(const ast_matchers::MatchFinder::MatchResult &Result) override;
};

} // namespace defenceStyle
} // namespace tidy
} // namespace clang

#endif // DEFENCESTYLE_NOMEANINGLESEXPRCHECK_H

```


```cpp
#include "NoMeaninglessExprCheck.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace defenceStyle {

void NoMeaninglessExprCheck::registerMatchers(MatchFinder *Finder) {
  Finder->addMatcher(
      expr(isExpansionInMainFile(),
           anyOf(binaryOperator(
                     anyOf(hasOperatorName("=="), hasOperatorName("!="),
                           hasOperatorName(">"), hasOperatorName("<"),
                           hasOperatorName("<="), hasOperatorName(">="),
                           hasOperatorName("+"), hasOperatorName("-"),
                           hasOperatorName("*"), hasOperatorName("/"),
                           hasOperatorName("%"), hasOperatorName("&&"),
                           hasOperatorName("||"), hasOperatorName("&"),
                           hasOperatorName("|"), hasOperatorName("^"),
                           hasOperatorName("<<"), hasOperatorName(">>"))),
                 unaryOperator(unless(
                     anyOf(hasOperatorName("++"), hasOperatorName("--"))))),
           unless(anyOf(hasDescendant(callExpr()),
                        hasDescendant(binaryOperator(hasOperatorName("="))),
                        hasAncestor(binaryOperator(hasOperatorName("="))),
                        hasAncestor(ifStmt()),  hasAncestor(whileStmt()),
                        hasAncestor(forStmt()), hasAncestor(callExpr()),
                        hasAncestor(doStmt()),  hasAncestor(returnStmt()), 
                        hasAncestor(conditionalOperator()), 
                        hasAncestor(arraySubscriptExpr()))))
          .bind("deadExpr"),
      this);
}

void NoMeaninglessExprCheck::check(const MatchFinder::MatchResult &Result) {
 const auto *expression = Result.Nodes.getNodeAs<Expr>("deadExpr");

 if (!expression) return;

  diag(expression->getExprLoc(),
         u8"side effect(부작용) 없는, 의미 없는 구문은 사용하지 마십시오.");
}

} // namespace defenceStyle
} // namespace tidy
} // namespace clang

```