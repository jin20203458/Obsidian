
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

  

// 함수 내부 명령문 검사 Visitor

class LineCountVisitor : public clang::RecursiveASTVisitor<LineCountVisitor> {

public:

  LineCountVisitor(clang::ASTContext &Ctx, ClangTidyCheck *Check)

      : Ctx(Ctx), Check(Check) {}

  

bool VisitStmt(clang::Stmt *S) {

  // 중괄호(CompoundStmt)와 NullStmt 제외

  if (isa<CompoundStmt>(S) || isa<NullStmt>(S)) return true;

  

  clang::SourceLocation loc = S->getBeginLoc();

  if (loc.isInvalid() || !Ctx.getSourceManager().isInMainFile(loc)) return true;

  

  unsigned line = Ctx.getSourceManager().getSpellingLineNumber(loc);

  llvm::errs() << "Visiting Stmt at line: " << line << " addr: " << S << "\n";

  

  auto &vec = lineCount[line];

  if (std::find(vec.begin(), vec.end(), S) == vec.end()) {

    vec.push_back(S);

    llvm::errs() << "  Added Stmt addr: " << S << " to line " << line << ". Count now: " << vec.size() << "\n";

  } else {

    llvm::errs() << "  Skipped duplicate Stmt addr: " << S << " at line " << line << "\n";

  }

  

  if (vec.size() == 2) {

    llvm::errs() << "  Line " << line << " has multiple statements. Emitting diagnostics.\n";

    for (auto *stmt : vec) {

      Check->diag(stmt->getBeginLoc(),

        u8"한 줄에 여러 개의 명령문(statement)이 있습니다. 각 명령문은 별도의 줄에 작성해야 합니다.");

    }

  }

  

  return true;

}

  
  

private:

  clang::ASTContext &Ctx;

  ClangTidyCheck *Check;

  llvm::DenseMap<unsigned, std::vector<const clang::Stmt *>> lineCount;

};

  
  

void MultiStatementPerLineCheck::registerMatchers(MatchFinder *Finder) {

  Finder->addMatcher(translationUnitDecl().bind("tu"), this);

}

  

void MultiStatementPerLineCheck::check(const MatchFinder::MatchResult &Result) {

  const auto *TU = Result.Nodes.getNodeAs<TranslationUnitDecl>("tu");

  if (!TU) return;

  auto &Ctx = *Result.Context;

  auto &SM = Ctx.getSourceManager();

  

  // 전역 선언 라인별 그룹핑

  llvm::DenseMap<unsigned, std::vector<const Decl *>> declLineMap;

  

  for (auto *D : TU->decls()) {

    clang::SourceLocation loc = D->getBeginLoc();

    if (loc.isInvalid() || !SM.isInMainFile(loc)) continue;

  

    unsigned line = SM.getSpellingLineNumber(loc);

  

    // 중복 등록 방지

    auto &v = declLineMap[line];

    if (std::find(v.begin(), v.end(), D) == v.end())

      v.push_back(D);

  

    if (declLineMap[line].size() == 2) {

      for (auto *decl : declLineMap[line]) {

        diag(decl->getBeginLoc(),

          u8"한 줄에 여러 개의 전역 선언(변수/함수/타입 등)이 있습니다. 각 선언은 별도의 줄에 작성해야 합니다.");

      }

    }

  

    // 함수 내부 명령문 검사

    if (auto *FD = llvm::dyn_cast<FunctionDecl>(D)) {

      if (FD->hasBody()) {

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