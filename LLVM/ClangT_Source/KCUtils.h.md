> 공용 유틸 모음 KOCOME Utils
```cpp

#pragma once

#include <string>
#include <algorithm>
#include "clang/Basic/SourceManager.h"
#include "clang/Lex/Lexer.h"

// 콤마 개수 계산
inline unsigned countCommas(const std::string &text) 
{
    return std::count(text.begin(), text.end(), ',');
}


// 한 라인 전체 소스코드 텍스트 추출
inline std::string getSourceLineText(const clang::SourceManager &SM, clang::SourceLocation Loc) 
{
    if (Loc.isInvalid() || !SM.isInMainFile(Loc))
        return "";

    bool Invalid = false;
    clang::SourceLocation LineBegin = SM.getExpansionLoc(Loc);
    unsigned lineNo = SM.getSpellingLineNumber(LineBegin);
    LineBegin = SM.translateLineCol(SM.getFileID(LineBegin), lineNo, 1);

    const char *LineData = SM.getCharacterData(LineBegin, &Invalid);
    if (Invalid) return "";

    unsigned maxRead = 512, length = 0;
    const char *Ptr = LineData;
    
    for (; length < maxRead; ++length, ++Ptr)
        if (*Ptr == '\n' || *Ptr == '\r' || *Ptr == '\0') break;

    return std::string(LineData, length);
}
```


```cpp
#include "MultiStatementPerLineCheck.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "llvm/ADT/DenseMap.h"
#include "../KCUtils.h" // 공통 유틸 헤더

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace defenceStyle {

// 전역 선언 검사
void MultiStatementPerLineCheck::checkGlobalDecls(const TranslationUnitDecl *TU,
                                                  const SourceManager &SM) {
  llvm::DenseMap<unsigned, std::vector<const Decl *>> declLineMap;

  for (const Decl *D : TU->decls()) {
    SourceLocation loc = D->getBeginLoc();
    if (loc.isInvalid() || !SM.isInMainFile(loc))
      continue;

    unsigned line = SM.getSpellingLineNumber(loc);
    auto &decls = declLineMap[line];
    if (std::find(decls.begin(), decls.end(), D) == decls.end())
      decls.push_back(D);
  }

  for (const auto &lineDeclPair : declLineMap) {
    unsigned line = lineDeclPair.first;
    const auto &decls = lineDeclPair.second;
    if (decls.size() < 2)
      continue;

    std::string lineText = getSourceLineText(SM, decls.front()->getBeginLoc());
    unsigned commaCount = countCommas(lineText);

    llvm::errs() << "[Debug] Line " << line << " has " << decls.size()
                 << " decls, comma count: " << commaCount << "\n";

    // 콤마 개수 + 1 >= 선언 개수면 오탐 방지
    if (commaCount + 1 >= decls.size())
      continue;

    for (const Decl *decl : decls) {
      diag(decl->getBeginLoc(),
           u8"한 줄에 여러 개의 전역 선언(변수/함수/타입 등)이 있습니다. 각 선언은 별도의 줄에 작성해야 합니다.");
    }
  }
}

// 함수 본문 내부 명령문 검사 (최상위 명령문만)
void MultiStatementPerLineCheck::checkFunctionBodyStmts(const TranslationUnitDecl *TU,
                                                       const SourceManager &SM) {
  for (const Decl *D : TU->decls()) {
    if (const auto *FD = dyn_cast<FunctionDecl>(D)) {
      if (!FD->hasBody())
        continue;

      const Stmt *Body = FD->getBody();
      if (const auto *CS = dyn_cast<CompoundStmt>(Body)) {
        llvm::DenseMap<unsigned, std::vector<const Stmt *>> lineCount;

        for (const Stmt *Child : CS->body()) {
          if (!Child)
            continue;

          SourceLocation loc = Child->getBeginLoc();
          if (loc.isInvalid() || !SM.isInMainFile(loc))
            continue;

          unsigned line = SM.getSpellingLineNumber(loc);
          auto &stmts = lineCount[line];

          if (std::find(stmts.begin(), stmts.end(), Child) == stmts.end())
            stmts.push_back(Child);

          if (stmts.size() == 2) {
            for (const Stmt *stmt : stmts) {
              diag(stmt->getBeginLoc(),
                   u8"한 줄에 여러 개의 명령문(statement)이 있습니다. 각 명령문은 별도의 줄에 작성해야 합니다.");
            }
          }
        }
      }
    }
  }
}

// 중괄호 블록 내 모든 명령문 검사
void MultiStatementPerLineCheck::checkCompoundStmt(const CompoundStmt *CS,
                                                  const SourceManager &SM) {
  if (!CS)
    return;
  llvm::DenseMap<unsigned, std::vector<const Stmt *>> lineCount;
  for (const Stmt *Child : CS->body()) {
    if (!Child)
      continue;

    SourceLocation loc = Child->getBeginLoc();
    if (loc.isInvalid() || !SM.isInMainFile(loc))
      continue;

    unsigned line = SM.getSpellingLineNumber(loc);
    auto &stmts = lineCount[line];

    if (std::find(stmts.begin(), stmts.end(), Child) == stmts.end())
      stmts.push_back(Child);

    if (stmts.size() == 2) {
      for (const Stmt *stmt : stmts) {
        diag(stmt->getBeginLoc(),
             u8"한 줄에 여러 개의 명령문(statement)이 있습니다. 각 명령문은 별도의 줄에 작성해야 합니다.");
      }
    }
  }
}

// 클래스/구조체 멤버 선언 검사
void MultiStatementPerLineCheck::checkRecordDeclMembers(const RecordDecl *RD,
                                                       const SourceManager &SM) {
  if (!RD)
    return;

  llvm::DenseMap<unsigned, std::vector<const Decl *>> memberLineMap;

  for (const Decl *D : RD->decls()) {
    if (!isa<FieldDecl>(D) && !isa<CXXMethodDecl>(D) && !isa<VarDecl>(D))
      continue;

    SourceLocation loc = D->getBeginLoc();
    if (loc.isInvalid() || !SM.isInMainFile(loc))
      continue;

    unsigned line = SM.getSpellingLineNumber(loc);
    auto &decls = memberLineMap[line];
    if (std::find(decls.begin(), decls.end(), D) == decls.end())
      decls.push_back(D);
  }

  for (const auto &linePair : memberLineMap) {
    unsigned line = linePair.first;
    const auto &decls = linePair.second;
    if (decls.size() < 2)
      continue;

    std::string lineText = getSourceLineText(SM, decls.front()->getBeginLoc());
    unsigned commaCount = countCommas(lineText);

    llvm::errs() << "[Debug] Line " << line << " has " << decls.size()
                 << " decls and " << commaCount << " commas.\n";

    if (commaCount + 1 >= decls.size())
      continue; // 콤마로 연결된 하나의 선언문으로 판단, 경고 생략

    for (const Decl *decl : decls) {
      diag(decl->getBeginLoc(),
           u8"한 줄에 여러 개의 멤버 선언이 있습니다. 각 선언은 별도의 줄에 작성해야 합니다.");
    }
  }
}

void MultiStatementPerLineCheck::check(const MatchFinder::MatchResult &Result) {
  const auto *TU = Result.Nodes.getNodeAs<TranslationUnitDecl>("tu");
  ASTContext &Ctx = *Result.Context;
  const SourceManager &SM = Ctx.getSourceManager();

  if (TU) {
    checkGlobalDecls(TU, SM);
    checkFunctionBodyStmts(TU, SM);
  }

  if (const auto *CS = Result.Nodes.getNodeAs<CompoundStmt>("block")) {
    checkCompoundStmt(CS, SM);
  }

  if (const auto *RD = Result.Nodes.getNodeAs<RecordDecl>("record")) {
    checkRecordDeclMembers(RD, SM);
  }
}

}  // namespace defenceStyle
}  // namespace tidy
}  // namespace clang

```


임시
```cpp
// 전역 선언 검사
void MultiStatementPerLineCheck::checkGlobalDecls(const TranslationUnitDecl *TU,
                                                  const SourceManager &SM) {
  llvm::DenseMap<unsigned, llvm::SmallPtrSet<const Decl*, 8>> declLineMap;

  for (const Decl *D : TU->decls()) 
  {
    SourceLocation loc = D->getBeginLoc();
    if (loc.isInvalid() || !SM.isInMainFile(loc))
      continue;

    unsigned line = SM.getSpellingLineNumber(loc);
    declLineMap[line].insert(D); // 자동으로 중복 제거됨
  }

  for (const auto &lineDeclPair : declLineMap) 
  {
    unsigned line = lineDeclPair.first;
    const auto &decls = lineDeclPair.second;
    if (decls.size() < 2)
      continue;

    std::string lineText = kcutils::getSourceLineText(SM, (*decls.begin())->getBeginLoc());
    unsigned commaCount = kcutils::countCommas(lineText);

    // 콤마 개수 + 1 >= 선언 개수면 오탐 방지
    if (commaCount + 1 >= decls.size())
      continue;

    for (const Decl *decl : decls)
      diag(decl->getBeginLoc(),
           u8"한 줄에 여러 개의 전역 선언(변수/함수/타입 등)이 있습니다. 각 선언은 별도의 줄에 작성해야 합니다.");
  }
}

```