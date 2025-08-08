

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
  void checkFallThrough(const SwitchStmt *SS, const SourceManager &SM);
};

} // namespace defenceStyle
} // namespace tidy
} // namespace clang

#endif // DEFENCESTYLE_SWITCHSTYLECHECK_H

```

```cpp
#include "SwitchStyleCheck.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "../KCUtils.h"

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace defenceStyle {

class FallThroughChecker : public RecursiveASTVisitor<FallThroughChecker> {
public:
    ClangTidyCheck *Check;
    const SourceManager &SM;
    
    const SwitchCase *CurrentCase = nullptr;
    bool needsBreak = false;
    bool hasStatements = false;
    SourceLocation lastStatementLoc; // 마지막 실행문 위치 추가
    
    FallThroughChecker(ClangTidyCheck *Check, const SourceManager &SM) 
        : Check(Check), SM(SM) {}
    
    bool VisitSwitchCase(SwitchCase *SC) {
        // 이전 case 체크 전에 fall-through 주석 확인
        if (needsBreak && CurrentCase) 
        {
            bool hasFallThroughComment = checkFallThroughComment(CurrentCase, SC);
            
            if (!hasFallThroughComment) {
                Check->diag(CurrentCase->getColonLoc(),
                           u8"case문 끝에 break, return, throw 등의 제어문이 필요합니다. "
                           u8"의도적인 fall-through라면 주석으로 명시하세요.");
            }
        }
        
        CurrentCase = SC;
        needsBreak = true;
        hasStatements = false;
        lastStatementLoc = SourceLocation(); // 리셋
        
        return true;
    }
    
    bool VisitStmt(Stmt *S) {
        if (isa<SwitchCase>(S)) return true;
        
        if (needsBreak && !isa<NullStmt>(S)) 
        {
            // 의미있는 실행문만 체크 (성능 최적화)
            if (!isa<ImplicitCastExpr>(S) && !isa<StringLiteral>(S) && 
                !isa<IntegerLiteral>(S) && !isa<ConstantExpr>(S) && 
                !isa<DeclRefExpr>(S) && !isa<CharacterLiteral>(S)) {
                
                hasStatements = true;
                lastStatementLoc = S->getEndLoc(); // 마지막 실행문 위치 기록
                
                // 제어문 체크
                if (isa<BreakStmt>(S) || isa<ReturnStmt>(S) || 
                    isa<CXXThrowExpr>(S) || isa<GotoStmt>(S)) 
                {
                    needsBreak = false;
                    hasStatements = false;
                }
            }
        }
        
        return true;
    }
    
    void checkLastCase() {
        if (needsBreak && CurrentCase && !isa<DefaultStmt>(CurrentCase)) {
            // 마지막 case는 주석 체크 없이 경고 (다음 case가 없으므로)
            Check->diag(CurrentCase->getColonLoc(),
                       u8"case문 끝에 break, return, throw 등의 제어문이 필요합니다.");
        }
    }

private:
    // 개선된 fall-through 주석 검사 함수
    bool checkFallThroughComment(const SwitchCase *fromCase, const SwitchCase *toCase) {
        
        // 검사 범위: fromCase 시작부터 toCase 시작 전까지
        SourceLocation startLoc = fromCase->getColonLoc();
        SourceLocation endLoc = toCase->getBeginLoc();
        
        if (startLoc.isInvalid() || endLoc.isInvalid()) 
            return false;
        
        
        // 마지막 실행문이 있으면 그 위치부터 검사
        if (lastStatementLoc.isValid()) {
            startLoc = lastStatementLoc;
        }
        
        unsigned startLine = SM.getSpellingLineNumber(startLoc);
        unsigned endLine = SM.getSpellingLineNumber(endLoc);
        
        // 해당 범위의 모든 라인 검사
        for (unsigned line = startLine; line < endLine; ++line)
         {
            std::string lineText = getLineText(line);
            std::string lowerText = lineText;
            std::transform(lowerText.begin(), lowerText.end(), lowerText.begin(), ::tolower);
            
            // 다양한 fall-through 패턴 검사
            if (hasFallThroughPattern(lowerText)) 
            {
                return true;
            }
        }
        
        return false;
    }
    
    bool hasFallThroughPattern(const std::string &lowerText) {
        // 주석 표시 확인 (중요: 주석이 아니면 무시)
        size_t commentPos = lowerText.find("//");
        if (commentPos == std::string::npos) 
        {
            commentPos = lowerText.find("/*");
        }
        if (commentPos == std::string::npos) 
        {
            return false; // 주석이 없으면 패턴 매칭 안함
        }
        
        // 주석 부분만 추출
        std::string commentText = lowerText.substr(commentPos);
        
        // 다양한 fall-through 패턴들
        return (commentText.find("fall") != std::string::npos && 
                commentText.find("through") != std::string::npos) ||
               commentText.find("fallthrough") != std::string::npos ||
               commentText.find("fall-through") != std::string::npos ||
               commentText.find("falls through") != std::string::npos ||
               commentText.find("drop through") != std::string::npos ||
               commentText.find("continue to") != std::string::npos;
    }
    
    std::string getLineText(unsigned lineNumber) {
        FileID FID = SM.getMainFileID();
        bool Invalid = false;
        llvm::StringRef Buffer = SM.getBufferData(FID, &Invalid);
        if (Invalid) return "";
        
        unsigned currentLine = 1;
        size_t pos = 0;
        
        while (pos < Buffer.size() && currentLine < lineNumber) 
        {
            if (Buffer[pos] == '\n')
            {
                currentLine++;
            }
            pos++;
        }
        
        if (currentLine == lineNumber) 
        {
            size_t lineEnd = Buffer.find('\n', pos);
            if (lineEnd == llvm::StringRef::npos) 
            {
                lineEnd = Buffer.size();
            }
            return Buffer.substr(pos, lineEnd - pos).str();
        }
        
        return "";
    }
  };

void SwitchStyleCheck::registerMatchers(MatchFinder *Finder) {
     Finder->addMatcher(
        switchStmt(isExpansionInMainFile()).bind("switch"), 
        this);
}

void SwitchStyleCheck::check(const MatchFinder::MatchResult &Result) {
  const auto *switchStatement = Result.Nodes.getNodeAs<SwitchStmt>("switch");

  if (!switchStatement)
    return;

  const CompoundStmt *CompoundBody =
      dyn_cast_or_null<CompoundStmt>(switchStatement->getBody());

  if (!CompoundBody) 
  {
    diag(switchStatement->getSwitchLoc(),
         u8"switch문은 반드시 중괄호 블록을 사용해야 합니다. "
         u8"수정후 재검사가 필요합니다. ");
    return;
  }

  bool hasDefault = false;
    for (const SwitchCase *SC = switchStatement->getSwitchCaseList(); 
         SC; SC = SC->getNextSwitchCase()) 
    {
        if (isa<DefaultStmt>(SC)) 
        {
            hasDefault = true;
            break;
        }
    }

  bool foundFirstLabel = false;
  for (const Stmt *S : CompoundBody->body()) 
  {
    if (isa<CaseStmt>(S) || isa<DefaultStmt>(S)) 
    {
      foundFirstLabel = true;
    } 
    else if (!foundFirstLabel && !isa<NullStmt>(S)) 
    {
      diag(S->getBeginLoc(), u8"switch 블록의 첫 번째 case/default label "
                             u8"전에는 코드가 올 수 없습니다.");
      break; 
    }
  }

  if (!hasDefault) 
  {
    diag(switchStatement->getSwitchLoc(),
         u8"switch문에는 반드시 default label이 포함되어야 합니다.");
  }

  // 3. fall-through 검사
  checkFallThrough(switchStatement, *Result.SourceManager);
}

void SwitchStyleCheck::checkFallThrough(const SwitchStmt *SS, const SourceManager &SM) {
    
    const CompoundStmt *Body = dyn_cast<CompoundStmt>(SS->getBody());
    if (!Body) {
        return;
    }
    
    FallThroughChecker checker(this, SM);
    checker.TraverseStmt(const_cast<CompoundStmt*>(Body));
    checker.checkLastCase();
    
}

} // namespace defenceStyle
} // namespace tidy
} // namespace clang

```