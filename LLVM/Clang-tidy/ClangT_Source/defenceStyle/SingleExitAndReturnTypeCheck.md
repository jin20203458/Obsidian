```cpp
#ifndef DEFENCESTYLE_SINGLEEXITANDRETURNTYPECHECK_H
#define DEFENCESTYLE_SINGLEEXITANDRETURNTYPECHECK_H

#include "../ClangTidyCheck.h"

namespace clang {
namespace tidy {
namespace defenceStyle {

class SingleExitAndReturnTypeCheck : public ClangTidyCheck {
public:
  SingleExitAndReturnTypeCheck(StringRef Name, ClangTidyContext *Context)
      : ClangTidyCheck(Name, Context) {}

  void registerMatchers(ast_matchers::MatchFinder *Finder) override;
  void check(const ast_matchers::MatchFinder::MatchResult &Result) override;
};

} // namespace defenceStyle
} // namespace tidy
} // namespace clang

#endif // DEFENCESTYLE_SINGLEEXITANDRETURNTYPECHECK_H

```

```cpp
#include "SingleExitAndReturnTypeCheck.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/AST/RecursiveASTVisitor.h"

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace defenceStyle {

// 함수 내 모든 return문을 재귀적으로 방문
class ReturnVisitor : public clang::RecursiveASTVisitor<ReturnVisitor> {
public:
  ReturnVisitor(const clang::QualType &RetType, ClangTidyCheck *Check, clang::ASTContext &Ctx)
      : RetType(RetType), Check(Check), Ctx(Ctx), HasReturn(false), ReturnCount(0) {}

  bool VisitReturnStmt(clang::ReturnStmt *RS) {

    if (!RS) return true;
    HasReturn = true;
    ReturnCount++;

    if (RetType->isVoidType()) 
    {
      if (RS->getRetValue() != nullptr) 
      {
        Check->diag(RS->getBeginLoc(),
            u8"void 함수에서는 값을 반환할 수 없습니다.");
      }
    } 
    else 
    {
      if (RS->getRetValue() == nullptr) 
      {
        Check->diag(RS->getBeginLoc(),
            u8"반환값이 필요한 함수에서 값을 반환하지 않았습니다.");
      } 
      else 
      {
        clang::QualType ReturnType = RetType.getUnqualifiedType();
        clang::QualType ValueType = RS->getRetValue()->getType().getUnqualifiedType();

        if (!Ctx.hasSameType(ReturnType, ValueType)) 
        {
          Check->diag(RS->getBeginLoc(),
              u8"함수 선언 반환형(%0)과 반환값 타입(%1)이 일치하지 않습니다.")
              << ReturnType.getAsString() << ValueType.getAsString();
        }
      }
    }
    return true;
  }

  bool HasReturn;
  int ReturnCount;
private:
  clang::QualType RetType;
  ClangTidyCheck *Check;
  clang::ASTContext &Ctx;
};

void SingleExitAndReturnTypeCheck::registerMatchers(MatchFinder *Finder) {
    Finder->addMatcher(
        functionDecl(
            isDefinition(),
            isExpansionInMainFile()  
        ).bind("func"), 
        this);
}

void SingleExitAndReturnTypeCheck::check(const MatchFinder::MatchResult &Result) {
  const auto *FD = Result.Nodes.getNodeAs<FunctionDecl>("func");
  if (!FD) return;

  // main 함수,생성,소멸자는 예외 (필요시 주석 해제)
  // if (FD->isMain()) return; 
  // if (isa<CXXConstructorDecl>(FD)) return; 
  // if (isa<CXXDestructorDecl>(FD)) return;
  clang::QualType retType = FD->getReturnType().getCanonicalType();

  const clang::Stmt *Body = FD->getBody();
  if (!Body) return;

  ReturnVisitor Visitor(retType, this, *Result.Context);
  Visitor.TraverseStmt(const_cast<clang::Stmt *>(Body));

  // 비-void 함수인데 return문이 아예 없는 경우
  if (!retType->isVoidType() && !Visitor.HasReturn) 
  {
    diag(FD->getLocation(),
         u8"반환값이 필요한 함수에 return문이 없습니다.");
  }
  
  // return문이 2개 이상이면 exit point 위반 경고
  if (Visitor.ReturnCount > 1) 
  {
    diag(FD->getLocation(),
         u8"함수 내에 exit point(return문)가 2개 이상 존재합니다. 함수는 하나의 exit point만 가져야 합니다.");
  }
}

} // namespace defenceStyle
} // namespace tidy
} // namespace clang

```

