
```cpp
#ifndef DEFENCESTYLE_MULTISTATEMENTPERLINECHECK_H
#define DEFENCESTYLE_MULTISTATEMENTPERLINECHECK_H

#include "../ClangTidyCheck.h"

namespace clang {
namespace tidy {
namespace defenceStyle {

class MultiStatementPerLineCheck : public ClangTidyCheck {
public:
  MultiStatementPerLineCheck(StringRef Name, ClangTidyContext *Context)
      : ClangTidyCheck(Name, Context) {}

  void registerMatchers(ast_matchers::MatchFinder *Finder) override;
  void check(const ast_matchers::MatchFinder::MatchResult &Result) override;
};

} // namespace defenceStyle
} // namespace tidy
} // namespace clang

#endif // DEFENCESTYLE_MULTISTATEMENTPERLINECHECK_H

```


```cpp
#include "MultiStatementPerLineCheck.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/Basic/SourceManager.h"
#include "llvm/ADT/DenseMap.h"

// NOTE : 메크로 확장이나 기타 전처리 작업시 행해지는지 작업에 의하여 AST로 변환되는 시점에 이미
// 문제가 되지 않는 경우는 (실제 사람이 보는 코드에서만 문제가 되는 경우는) 체크하지 않습니다.
// 추후 관련부분을 확장할수 있지만 현재로서는 전역 선언과 함수 본문내 선언만 검사합니다. 
using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace defenceStyle {

void MultiStatementPerLineCheck::registerMatchers(MatchFinder *Finder) {
 
  Finder->addMatcher(translationUnitDecl().bind("tu"), this);
  Finder->addMatcher(compoundStmt().bind("block"), this);
   Finder->addMatcher(recordDecl(isDefinition()).bind("record"), this);
}

void MultiStatementPerLineCheck::check(const MatchFinder::MatchResult &Result) {

  const auto *TU = Result.Nodes.getNodeAs<TranslationUnitDecl>("tu");
  
  if (!TU)
    return;

  ASTContext &Ctx = *Result.Context;
  const SourceManager &SM = Ctx.getSourceManager();

  // 전역 선언 검사 
  llvm::DenseMap<unsigned, std::vector<const Decl *>> declLineMap;
  for (const Decl *D : TU->decls()) 
  {

    SourceLocation loc = D->getBeginLoc();
    if (loc.isInvalid() || !SM.isInMainFile(loc))
      continue;

    unsigned line = SM.getSpellingLineNumber(loc);
    llvm::errs() << "[GlobalDecl] Decl at line " << line << ", addr: " << D << "\n";
    auto &decls = declLineMap[line];

    if (std::find(decls.begin(), decls.end(), D) == decls.end()) // 중복 검사 
    {
      decls.push_back(D);
      llvm::errs() << "  Added Decl addr: " << D << " to line " << line << ". Count now: " << decls.size() << "\n";
    } 
    else 
    {
      llvm::errs() << "  Skipped duplicate Decl addr: " << D << " at line " << line << "\n";
    }

    if (decls.size() == 2) 
    {
      llvm::errs() << "  Line " << line << " has multiple global declarations. Emitting diagnostics.\n";
      for (const Decl *decl : decls) 
      {
        diag(decl->getBeginLoc(),
             u8"한 줄에 여러 개의 전역 선언(변수/함수/타입 등)이 있습니다. 각 선언은 별도의 줄에 작성해야 합니다.");
      }
    }
  }


  // 함수 내부 최상위 명령문만 검사
  for (const Decl *D : TU->decls())
   {
    if (const auto *FD = dyn_cast<FunctionDecl>(D)) 
    {
      if (!FD->hasBody()) // 함수 본문이 없으면 스킵(선언일 경우)
        continue;
      // 함수 본문 내부 검사
      const Stmt *Body = FD->getBody();
      if (const auto *CS = dyn_cast<CompoundStmt>(Body)) 
      {

        llvm::DenseMap<unsigned, std::vector<const Stmt *>> lineCount;
        for (const Stmt *Child : CS->body()) 
        {
          if (!Child)
            continue;

          SourceLocation loc = Child->getBeginLoc();
          if (loc.isInvalid() || !SM.isInMainFile(loc))
            continue;
          llvm::errs() << "[FuncBody] Visiting FunctionDecl addr: " << FD << "\n";

          unsigned line = SM.getSpellingLineNumber(loc);
          llvm::errs() << "  Visiting Stmt at line: " << line << ", addr: " << Child << "\n";
          auto &stmts = lineCount[line];

          if (std::find(stmts.begin(), stmts.end(), Child) == stmts.end()) 
          {
            stmts.push_back(Child);
            llvm::errs() << "    Added Stmt addr: " << Child << " to line " << line << ". Count now: " << stmts.size() << "\n";
          } 
          else 
          {
            llvm::errs() << "    Skipped duplicate Stmt addr: " << Child << " at line " << line << "\n";
          }

          if (stmts.size() == 2) 
          {
            llvm::errs() << "    Line " << line << " has multiple statements. Emitting diagnostics.\n";
            for (const Stmt *stmt : stmts) 
            {
              diag(stmt->getBeginLoc(),
                   u8"한 줄에 여러 개의 명령문(statement)이 있습니다. 각 명령문은 별도의 줄에 작성해야 합니다.");
            }
          }
        }
      }
    }
  }

    // 2. 모든 중괄호 블록 내부 검사
  if (const auto *CS = Result.Nodes.getNodeAs<CompoundStmt>("block")) 
  {
    llvm::DenseMap<unsigned, std::vector<const Stmt *>> lineCount;
    for (const Stmt *Child : CS->body()) 
    {
      if (!Child) continue;
      SourceLocation loc = Child->getBeginLoc();

      if (loc.isInvalid() || !SM.isInMainFile(loc)) continue;
      unsigned line = SM.getSpellingLineNumber(loc);
      auto &stmts = lineCount[line];

      if (std::find(stmts.begin(), stmts.end(), Child) == stmts.end())
        stmts.push_back(Child);

      if (stmts.size() == 2) 
      {
        for (const Stmt *stmt : stmts) 
        {
          diag(stmt->getBeginLoc(),
               u8"한 줄에 여러 개의 명령문(statement)이 있습니다. 각 명령문은 별도의 줄에 작성해야 합니다.");
        }
      }
    }
  }

  // 3. 클래스/구조체 선언부 검사
  if (const auto *RD = Result.Nodes.getNodeAs<RecordDecl>("record"))
   {
    llvm::DenseMap<unsigned, std::vector<const Decl *>> memberLineMap;
    for (const Decl *D : RD->decls()) 
    {
      // 실제 멤버만(필드, 메서드 등)
      if (!isa<FieldDecl>(D) && !isa<CXXMethodDecl>(D) && !isa<VarDecl>(D)) continue;
      SourceLocation loc = D->getBeginLoc();

      if (loc.isInvalid() || !SM.isInMainFile(loc)) continue;
      unsigned line = SM.getSpellingLineNumber(loc);
      auto &decls = memberLineMap[line];

      if (std::find(decls.begin(), decls.end(), D) == decls.end())
        decls.push_back(D);
      if (decls.size() == 2)
       {
        for (const Decl *decl : decls)
        {
          diag(decl->getBeginLoc(),
               u8"한 줄에 여러 개의 멤버 선언이 있습니다. 각 선언은 별도의 줄에 작성해야 합니다.");
        }
      }
    }
  }

}
}



} // namespace defenceStyle
} // namespace tidy
} // namespace clang

```