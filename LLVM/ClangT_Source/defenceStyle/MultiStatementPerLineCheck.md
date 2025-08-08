
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

  void checkGlobalDecls(const TranslationUnitDecl *TU, const SourceManager &SM);
  void checkFunctionBodyStmt(const CompoundStmt *CS, const SourceManager &SM);
  void checkCompoundStmt(const CompoundStmt *CS, const SourceManager &SM);
  void checkRecordDeclMembers(const RecordDecl *RD, const SourceManager &SM);
};

} // namespace defenceStyle
} // namespace tidy
} // namespace clang

#endif // DEFENCESTYLE_MULTISTATEMENTPERLINECHECK_H
```


```cpp
#include "MultiStatementPerLineCheck.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/SmallPtrSet.h" 
#include "../KCUtils.h" 

using namespace clang::ast_matchers;

// NOTE : 매크로 확장이나 기타 전처리 작업시 행해지는지 작업에 의하여 AST로
// 변환되는 시점에 이미 문제가 되지 않는 경우는 (실제 사람이 보는 코드에서만
// 문제가 되는 경우는) 체크하지 않습니다. 추후 관련부분을 확장할수 있지만
// 현재로서는 전역 선언과 함수 본문내 선언만 검사합니다.
namespace clang {
namespace tidy {
namespace defenceStyle {

void MultiStatementPerLineCheck::registerMatchers(MatchFinder *Finder) {
    Finder->addMatcher(translationUnitDecl().bind("tu"), this);

    Finder->addMatcher(
        functionDecl(isDefinition(), 
        isExpansionInMainFile()
      ).bind("function"), 
        this);


    Finder->addMatcher(
        compoundStmt(
            isExpansionInMainFile(),
            unless(hasParent(functionDecl()))
        ).bind("block"), 
        this);

      Finder->addMatcher(
        recordDecl(isDefinition(), 
        isExpansionInMainFile()
      ).bind("record"), 
        this);
}

// 전역 선언 검사
void MultiStatementPerLineCheck::checkGlobalDecls(const TranslationUnitDecl *TU,
                                                  const SourceManager &SM) {
  llvm::DenseMap<unsigned, llvm::SmallPtrSet<const Decl*, 8>> declLineMap;

  for (const Decl *D : TU->decls()) 
  {
    if (!kcutils::isInMainFile(SM, D)) continue; 

    unsigned line = kcutils::getLineNumber(SM, D);
    declLineMap[line].insert(D); // 자동으로 중복 제거됨
  }

  for (const auto &lineDeclPair : declLineMap) 
  {
    const auto &decls = lineDeclPair.second;
    if (decls.size() < 2)
      continue;

    std::string lineText = kcutils::getSourceLineText(SM, (*decls.begin())->getBeginLoc());
    unsigned commaCount = kcutils::countDeclarationSeparatorCommas(lineText);

    // 콤마 개수 + 1 >= 선언 개수면 오탐 방지
    if (commaCount + 1 >= decls.size())
      continue;

    for (const Decl *decl : decls)
      diag(decl->getBeginLoc(),
           u8"한 줄에 여러 개의 전역 선언(변수/함수/타입 등)이 있습니다. 각 선언은 별도의 줄에 작성해야 합니다.");
  }
}

// 함수 본문 내부 명령문 검사 (최상위 명령문만)
void MultiStatementPerLineCheck::checkFunctionBodyStmt(const CompoundStmt *CS,
                                                      const SourceManager &SM) {
    if (!CS)
        return;
        
    llvm::DenseMap<unsigned, llvm::SmallPtrSet<const Stmt*, 8>> lineStmtMap;
    
    for (const Stmt *Child : CS->body()) 
    {
        if (!Child) continue;
            
        unsigned line = kcutils::getLineNumber(SM, Child);   
        lineStmtMap[line].insert(Child);
    }
    
    // 경고 생성
    for (const auto &lineStmtPair : lineStmtMap) 
    {
        const auto &stmts = lineStmtPair.second;
        if (stmts.size() >= 2) 
            for (const Stmt *stmt : stmts) 
            
                diag(stmt->getBeginLoc(),
                     u8"한 줄에 여러 개의 명령문(statement)이 있습니다. 각 명령문은 별도의 줄에 작성해야 합니다.");
            
    }
}


// 중괄호 블록 내 모든 명령문 검사
void MultiStatementPerLineCheck::checkCompoundStmt(const CompoundStmt *CS,
                                                  const SourceManager &SM) {
  if (!CS)
    return;
    
  llvm::DenseMap<unsigned, llvm::SmallPtrSet<const Stmt*, 8>> lineStmtMap;
  for (const Stmt *Child : CS->body()) 
  {
    if (!Child)
      continue;

    unsigned line = kcutils::getLineNumber(SM, Child);
    lineStmtMap[line].insert(Child); // 자동으로 중복 제거됨
  }

  // 한 줄에 2개 이상의 명령문이 있는 경우만 경고
  for (const auto &lineStmtPair : lineStmtMap) 
  {
    const auto &stmts = lineStmtPair.second;

    if (stmts.size() >= 2) 
      for (const Stmt *stmt : stmts)
        diag(stmt->getBeginLoc(),
             u8"한 줄에 여러 개의 명령문(statement)이 있습니다. 각 명령문은 별도의 줄에 작성해야 합니다.");
    
  }
}

// 레코드 내부 검사
void MultiStatementPerLineCheck::checkRecordDeclMembers(const RecordDecl *RD,
                                                       const SourceManager &SM) {
    if (!RD)
        return;

    unsigned classLine = kcutils::getLineNumber(SM, RD);
    llvm::DenseMap<unsigned, llvm::SmallPtrSet<const Decl*, 8>> memberLineMap;

    for (const Decl *D : RD->decls()) 
    {
        unsigned line = kcutils::getLineNumber(SM, D);
        
        // 클래스 선언과 같은 라인 처리
        if (line == classLine) 
        {
            // 암시적 멤버들 제외 (컴파일러 자동 생성)
            if (D->isImplicit()) 
                continue;
            
            // 클래스 선언 자체 제외
            if (isa<RecordDecl>(D)) 
                continue;
        }

        // 대상 멤버 타입 필터링
        if (!isa<FieldDecl>(D) && 
            !isa<CXXMethodDecl>(D) && 
            !isa<VarDecl>(D) &&
            !isa<EnumDecl>(D) &&          
            !isa<RecordDecl>(D) &&         
            !isa<TypedefDecl>(D) &&      
            !isa<UsingDecl>(D) &&
            !isa<TypeAliasDecl>(D)) 
            continue;
        
        memberLineMap[line].insert(D);
    }

    // 라인별 다중 선언 검사
    for (const auto &linePair : memberLineMap) 
    {
        const auto &decls = linePair.second;
        
        if (decls.size() < 2) 
            continue;

        std::string lineText = kcutils::getSourceLineText(SM, (*decls.begin())->getBeginLoc());
        unsigned commaCount = kcutils::countDeclarationSeparatorCommas(lineText);

        // 오탐 방지: 콤마로 구분된 정상 선언은 건너뛰기
        if (commaCount + 1 >= decls.size())
            continue;

        // 경고 생성
        for (const Decl *decl : decls) 
            diag(decl->getBeginLoc(),
                 u8"한 줄에 여러 개의 멤버 선언이 있습니다. 각 선언은 별도의 줄에 작성해야 합니다.");
    }
}




void MultiStatementPerLineCheck::check(const MatchFinder::MatchResult &Result) {
    const SourceManager &SM = Result.Context->getSourceManager();
    
    // 전역 선언만 검사 (함수 선언 포함, 함수 본문은 제외)
    if (const auto *TU = Result.Nodes.getNodeAs<TranslationUnitDecl>("tu"))
     {
        checkGlobalDecls(TU, SM);
     }

    // 함수 본문
    if (const auto *FD = Result.Nodes.getNodeAs<FunctionDecl>("function")) 
    {
      if (FD->hasBody()) 
      {
        if (const auto *CS = dyn_cast<CompoundStmt>(FD->getBody()))
        {
          checkFunctionBodyStmt(CS, SM);
        }
      }
    } 
    
    // 일반 CompoundStmt (함수 본문이 아닌)
    if (const auto *CS = Result.Nodes.getNodeAs<CompoundStmt>("block")) 
    {
      checkCompoundStmt(CS, SM);
    }
    
    // 클래스/구조체 멤버
    if (const auto *RD = Result.Nodes.getNodeAs<RecordDecl>("record")) 
    {
      checkRecordDeclMembers(RD, SM);
    }
}


} // namespace defenceStyle
} // namespace tidy
} // namespace clang

```