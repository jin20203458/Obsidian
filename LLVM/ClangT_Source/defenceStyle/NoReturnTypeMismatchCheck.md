```cpp
#ifndef DEFENCESTYLE_NORETURNTYPEMISMATCHCHECK_H
#define DEFENCESTYLE_NORETURNTYPEMISMATCHCHECK_H

#include "../ClangTidyCheck.h"

namespace clang {
namespace tidy {
namespace defenceStyle {

class NoReturnTypeMismatchCheck : public ClangTidyCheck {
public:
  NoReturnTypeMismatchCheck(StringRef Name, ClangTidyContext *Context)
      : ClangTidyCheck(Name, Context) {}

  void registerMatchers(ast_matchers::MatchFinder *Finder) override;
  void check(const ast_matchers::MatchFinder::MatchResult &Result) override;
};

} // namespace defenceStyle
} // namespace tidy
} // namespace clang

#endif // DEFENCESTYLE_NORETURNTYPEMISMATCHCHECK_H
```

```cpp
#include "NoReturnTypeMismatchCheck.h"

#include "clang/ASTMatchers/ASTMatchFinder.h"

#include "clang/AST/RecursiveASTVisitor.h"

  

using namespace clang::ast_matchers;

  

namespace clang {

namespace tidy {

namespace defenceStyle {

  

// 모든 return문을 재귀적으로 방문

class ReturnVisitor : public clang::RecursiveASTVisitor<ReturnVisitor> {

public:

  explicit ReturnVisitor(const clang::QualType &RetType, ClangTidyCheck *Check)

      : RetType(RetType), Check(Check), HasReturn(false) {}

  

  bool VisitReturnStmt(clang::ReturnStmt *RS) {

    HasReturn = true;

    if (RetType->isVoidType()) {

      if (RS->getRetValue() != nullptr) {

        Check->diag(RS->getBeginLoc(),

            u8"void 함수에서는 값을 반환할 수 없습니다.");

      }

    } else {

      if (RS->getRetValue() == nullptr) {

        Check->diag(RS->getBeginLoc(),

            u8"반환값이 필요한 함수에서 값을 반환하지 않았습니다.");

      }

    }

    return true;

  }

  

  bool HasReturn;

private:

  clang::QualType RetType;

  ClangTidyCheck *Check;

};

  

void NoReturnTypeMismatchCheck::registerMatchers(MatchFinder *Finder) {

  Finder->addMatcher(functionDecl(isDefinition()).bind("func"), this);

}

  

void NoReturnTypeMismatchCheck::check(const MatchFinder::MatchResult &Result) {

  const auto *FD = Result.Nodes.getNodeAs<FunctionDecl>("func");

  if (!FD) return;

  if (FD->isMain()) return; // main 함수는 예외로 처리

  

  clang::QualType retType = FD->getReturnType().getCanonicalType();

  

  const clang::Stmt *Body = FD->getBody();

  if (!Body) return;

  

  ReturnVisitor Visitor(retType, this);

  Visitor.TraverseStmt(const_cast<clang::Stmt *>(Body));

  

  // 비-void 함수인데 return문이 아예 없는 경우

  if (!retType->isVoidType() && !Visitor.HasReturn) {

    diag(FD->getLocation(),

         u8"반환값이 필요한 함수에 return문이 없습니다.");

  }

}

  

} // namespace defenceStyle

} // namespace tidy

} // namespace clang
```

