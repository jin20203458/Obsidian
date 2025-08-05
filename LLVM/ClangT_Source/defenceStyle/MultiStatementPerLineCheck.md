
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
#include "MultiStatementPerLineCheck.h"

#include "clang/ASTMatchers/ASTMatchFinder.h"

#include "clang/AST/RecursiveASTVisitor.h"

#include "clang/Basic/SourceManager.h"

  

using namespace clang::ast_matchers;

  

namespace clang {

namespace tidy {

namespace defenceStyle {

  

// 모든 statement를 line 단위로 카운트하는 Visitor

class LineCountVisitor : public clang::RecursiveASTVisitor<LineCountVisitor> {

public:

  LineCountVisitor(clang::ASTContext &Ctx, ClangTidyCheck *Check)

      : Ctx(Ctx), Check(Check) {}

  

  bool VisitStmt(clang::Stmt *S) {

    // 중괄호(CompoundStmt) 자체, NullStmt는 제외

    if (isa<CompoundStmt>(S) || isa<NullStmt>(S)) return true;

  

    // 현재 statement의 시작 위치를 기준으로 라인 번호를 가져옴

    clang::SourceLocation loc = S->getBeginLoc();

    if (loc.isInvalid() || !Ctx.getSourceManager().isInMainFile(loc)) return true;

  

    // 라인 번호를 기준으로 statement를 카운트

    unsigned line = Ctx.getSourceManager().getSpellingLineNumber(loc);

    lineCount[line].push_back(S);

  

    // 2개 이상 statement가 있는 줄은 진단

    if (lineCount[line].size() == 2)

    { // 첫 번째는 허용, 두 번째부터 경고

      for (auto *stmt : lineCount[line])

      {

        Check->diag(stmt->getBeginLoc(),

          u8"한 줄에 여러 개의 명령문(statement)이 있습니다. 각 명령문은 별도의 줄에 작성해야 합니다.");

      }

    }

    return true;

  }

  

private:

  clang::ASTContext &Ctx;

  ClangTidyCheck *Check;

  llvm::DenseMap<unsigned, std::vector<clang::Stmt *>> lineCount;

};

  
  

void MultiStatementPerLineCheck::registerMatchers(MatchFinder *Finder) {

  Finder->addMatcher(translationUnitDecl().bind("tu"), this);

}

  
  

void MultiStatementPerLineCheck::check(const MatchFinder::MatchResult &Result) {

  const auto *TU = Result.Nodes.getNodeAs<TranslationUnitDecl>("tu");

  if (!TU) return;

  auto &Ctx = *Result.Context;

  

  // 1. 전역 선언(변수/함수/타입 등): 한 줄에 2개 이상 있으면 경고

  llvm::DenseMap<unsigned, std::vector<const Decl *>> declLineMap;

  for (auto *D : TU->decls())

  {

    clang::SourceLocation loc = D->getBeginLoc();

    if (loc.isInvalid() || !Ctx.getSourceManager().isInMainFile(loc)) continue;

  

    unsigned line = Ctx.getSourceManager().getSpellingLineNumber(loc);

    declLineMap[line].push_back(D);

  

    if (declLineMap[line].size() == 2)

    {

      for (auto *decl : declLineMap[line])

      {

        diag(decl->getBeginLoc(),

          u8"한 줄에 여러 개의 전역 선언(변수/함수/타입 등)이 있습니다. 각 선언은 별도의 줄에 작성해야 합니다.");

      }

    }

  

    // 2. 함수 정의 내부 명령문도 별도 검사

    if (auto *FD = llvm::dyn_cast<FunctionDecl>(D))

     {

      if (FD->hasBody())

      {

        const Stmt *Body = FD->getBody();

        LineCountVisitor Visitor(Ctx, this);

        Visitor.TraverseStmt(const_cast<Stmt *>(Body));

      }

    }

  }

}

  

} // namespace defenceStyle

} // namespace tidy

} // namespace clang
```