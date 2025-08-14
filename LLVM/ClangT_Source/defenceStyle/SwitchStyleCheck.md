

```cpp
#ifndef DEFENCESTYLE_SWITCHSTYLECHECK_H
#define DEFENCESTYLE_SWITCHSTYLECHECK_H

#include "../ClangTidyCheck.h"

namespace clang {
namespace tidy {
namespace defenceStyle {

/// Switch 문의 스타일 및 안전성을 검사하는 클래스
///
/// 검사 항목:
/// 1. Switch 문에서 논리 연산자 사용 금지
/// 2. Default label 필수 포함
/// 3. 첫 번째 label 이전 코드 금지  
/// 4. Fall-through 검사 (break문 또는 주석 필요)
/// 5. Switch 문 기본 구조 검증
class SwitchStyleCheck : public ClangTidyCheck {
public:
    SwitchStyleCheck(StringRef Name, ClangTidyContext *Context)
        : ClangTidyCheck(Name, Context) {}

    void registerMatchers(ast_matchers::MatchFinder *Finder) override;
    void check(const ast_matchers::MatchFinder::MatchResult &Result) override;

private:
    // 주요 검사 메서드들
    
    /// Switch 문의 기본 구조를 검증 (중괄호 블록 여부)
    /// @return 구조가 유효하면 true, 그렇지 않으면 false
    bool validateSwitchStructure(const SwitchStmt *SS);
    
    /// Switch 조건식에서 논리 연산자 사용 검사 (&&, ||, !, ?:)
    void checkLogicalExpressionInSwitch(const SwitchStmt *SS);
    
    /// Default label 존재 여부 검사
    void checkDefaultLabel(const SwitchStmt *SS);
    
    /// 첫 번째 case/default label 이전에 코드가 있는지 검사
    void checkCodeBeforeFirstLabel(const SwitchStmt *SS);
    
    /// Fall-through 검사 (break문 또는 fall-through 주석 확인)
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
#include <algorithm>
#include <string>

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace defenceStyle {

// ============================================================================
// 상수 및 설정
// ============================================================================
namespace {
    // Fall-through 패턴들을 상수로 정의
    const std::vector<std::string> FALLTHROUGH_PATTERNS = {
        "fallthrough",
        "fall-through", 
        "falls through",
        "fall through",
        "drop through",
        "continue to"
    };
    
    // 무시할 표현식 타입들
    template<typename T>
    bool isIgnorableExpr(const Stmt* S) {
        return isa<T>(S);
    }
    
    bool shouldIgnoreStatement(const Stmt* S) {
        return isIgnorableExpr<ImplicitCastExpr>(S) ||
               isIgnorableExpr<StringLiteral>(S) ||
               isIgnorableExpr<IntegerLiteral>(S) ||
               isIgnorableExpr<ConstantExpr>(S) ||
               isIgnorableExpr<DeclRefExpr>(S) ||
               isIgnorableExpr<CharacterLiteral>(S);
    }
    
    bool isControlFlowStatement(const Stmt* S) {
        return isa<BreakStmt>(S) || isa<ReturnStmt>(S) || 
               isa<CXXThrowExpr>(S) || isa<GotoStmt>(S);
    }
}

// ============================================================================
// LogicalExpressionChecker - 논리 연산자 검사 전담 클래스
// ============================================================================
class LogicalExpressionChecker : public RecursiveASTVisitor<LogicalExpressionChecker> {
private:
    ClangTidyCheck* Check;
    
public:
    explicit LogicalExpressionChecker(ClangTidyCheck* Check) : Check(Check) {}
    
    bool VisitBinaryOperator(BinaryOperator* BO) {
        if (BO->isLogicalOp()) {
            Check->diag(BO->getOperatorLoc(),
                       u8"switch문의 조건식에서 논리 연산자(%0)를 사용해서는 안됩니다. "
                       u8"if-else 구문을 사용하세요.")
                << BO->getOpcodeStr();
            return false; // 더 이상 순회하지 않음
        }
        return true;
    }
    
    bool VisitUnaryOperator(UnaryOperator* UO) {
        if (UO->getOpcode() == UO_LNot) {
            Check->diag(UO->getOperatorLoc(),
                       u8"switch문의 조건식에서 논리 부정 연산자(!)를 사용해서는 안됩니다. "
                       u8"if-else 구문을 사용하세요.");
            return false;
        }
        return true;
    }
    
    bool VisitConditionalOperator(ConditionalOperator* CO) {
        Check->diag(CO->getQuestionLoc(),
                   u8"switch문의 조건식에서 삼항 연산자(?:)를 사용해서는 안됩니다. "
                   u8"if-else 구문을 사용하세요.");
        return false;
    }
};

// ============================================================================
// SourceAnalyzer - 소스 코드 분석 유틸리티 클래스
// ============================================================================
class SourceAnalyzer {
public:
const SourceManager& SM;
    explicit SourceAnalyzer(const SourceManager& SM) : SM(SM) {}
    
    std::string getLineText(unsigned lineNumber) const {
        FileID FID = SM.getMainFileID();
        bool Invalid = false;
        llvm::StringRef Buffer = SM.getBufferData(FID, &Invalid);
        if (Invalid) return "";
        
        unsigned currentLine = 1;
        size_t pos = 0;
        
        // 목표 라인까지 이동
        while (pos < Buffer.size() && currentLine < lineNumber) {
            if (Buffer[pos] == '\n') {
                currentLine++;
            }
            pos++;
        }
        
        // 라인 텍스트 추출
        if (currentLine == lineNumber) {
            size_t lineEnd = Buffer.find('\n', pos);
            if (lineEnd == llvm::StringRef::npos) {
                lineEnd = Buffer.size();
            }
            return Buffer.substr(pos, lineEnd - pos).str();
        }
        
        return "";
    }
    
    bool hasFallThroughPattern(const std::string& text) const {
        std::string lowerText = text;
        std::transform(lowerText.begin(), lowerText.end(), 
                      lowerText.begin(), ::tolower);
        
        // 주석 확인
        size_t commentPos = std::min(lowerText.find("//"), lowerText.find("/*"));
        if (commentPos == std::string::npos) {
            return false;
        }
        
        // 주석 부분만 추출
        std::string commentText = lowerText.substr(commentPos);
        
        // 패턴 매칭
        for (const auto& pattern : FALLTHROUGH_PATTERNS) {
            if (commentText.find(pattern) != std::string::npos) {
                return true;
            }
        }
        
        // 복합 패턴 (fall + through)
        return (commentText.find("fall") != std::string::npos && 
                commentText.find("through") != std::string::npos);
    }
};

// ============================================================================
// FallThroughChecker - Fall-through 검사 전담 클래스 
// ============================================================================
class FallThroughChecker : public RecursiveASTVisitor<FallThroughChecker> {
private:
    ClangTidyCheck* Check;
    SourceAnalyzer Analyzer;
    
    // 상태 변수들
    const SwitchCase* CurrentCase = nullptr;
    bool needsBreak = false;
    bool hasStatements = false;
    SourceLocation lastStatementLoc;
    
    bool checkFallThroughComment(const SwitchCase* fromCase, 
                                const SwitchCase* toCase) {
        if (!fromCase || !toCase) return false;
        
        SourceLocation startLoc = fromCase->getColonLoc();
        SourceLocation endLoc = toCase->getBeginLoc();
        
        if (startLoc.isInvalid() || endLoc.isInvalid()) {
            return false;
        }
        
        // 마지막 실행문이 있으면 그 위치부터 검사
        if (lastStatementLoc.isValid()) {
            startLoc = lastStatementLoc;
        }
        
        unsigned startLine = Analyzer.SM.getSpellingLineNumber(startLoc);
        unsigned endLine = Analyzer.SM.getSpellingLineNumber(endLoc);
        
        // 해당 범위의 모든 라인 검사
        for (unsigned line = startLine; line < endLine; ++line) {
            std::string lineText = Analyzer.getLineText(line);
            if (Analyzer.hasFallThroughPattern(lineText)) {
                return true;
            }
        }
        
        return false;
    }
    
public:
    FallThroughChecker(ClangTidyCheck* Check, const SourceManager& SM) 
        : Check(Check), Analyzer(SM) {}
    
    bool VisitSwitchCase(SwitchCase* SC) {
        // 이전 case 검사
        if (needsBreak && CurrentCase) {
            bool hasFallThroughComment = checkFallThroughComment(CurrentCase, SC);
            
            if (!hasFallThroughComment) {
                Check->diag(CurrentCase->getColonLoc(),
                           u8"case문 끝에 break, return, throw 등의 제어문이 필요합니다. "
                           u8"의도적인 fall-through라면 주석으로 명시하세요.");
            }
        }
        
        // 새로운 case 시작
        CurrentCase = SC;
        needsBreak = true;
        hasStatements = false;
        lastStatementLoc = SourceLocation();
        
        return true;
    }
    
    bool VisitStmt(Stmt* S) {
        if (isa<SwitchCase>(S)) return true;
        
        if (needsBreak && !isa<NullStmt>(S)) {
            // 무시할 수 있는 문장들 제외
            if (!shouldIgnoreStatement(S)) {
                hasStatements = true;
                lastStatementLoc = S->getEndLoc();
                
                // 제어문 체크
                if (isControlFlowStatement(S)) {
                    needsBreak = false;
                    hasStatements = false;
                }
            }
        }
        
        return true;
    }
    
    void checkLastCase() {
        if (needsBreak && CurrentCase && !isa<DefaultStmt>(CurrentCase)) {
            Check->diag(CurrentCase->getColonLoc(),
                       u8"case문 끝에 break, return, throw 등의 제어문이 필요합니다.");
        }
    }
};

// ============================================================================
// SwitchStyleCheck 메인 클래스
// ============================================================================

void SwitchStyleCheck::registerMatchers(MatchFinder* Finder) {
    Finder->addMatcher(
        switchStmt(isExpansionInMainFile()).bind("switch"), 
        this);
}

void SwitchStyleCheck::check(const MatchFinder::MatchResult& Result) {
    const auto* switchStatement = Result.Nodes.getNodeAs<SwitchStmt>("switch");
    
    if (!switchStatement || 
        !kcutils::isInMainFile(*Result.SourceManager, switchStatement)) {
        return;
    }
    
    // 1. 기본 구조 검사
    if (!validateSwitchStructure(switchStatement)) {
        return; // 구조가 잘못되면 다른 검사 생략
    }
    
    // 2. 논리 연산자 검사
    checkLogicalExpressionInSwitch(switchStatement);
    
    // 3. Default label 검사
    checkDefaultLabel(switchStatement);
    
    // 4. 첫 번째 label 이전 코드 검사
    checkCodeBeforeFirstLabel(switchStatement);
    
    // 5. Fall-through 검사
    checkFallThrough(switchStatement, *Result.SourceManager);
}

bool SwitchStyleCheck::validateSwitchStructure(const SwitchStmt* SS) {
    const CompoundStmt* CompoundBody = dyn_cast_or_null<CompoundStmt>(SS->getBody());
    
    if (!CompoundBody) {
        diag(SS->getSwitchLoc(),
             u8"switch문은 반드시 중괄호 블록을 사용해야 합니다. "
             u8"수정후 재검사가 필요합니다.");
        return false;
    }
    return true;
}

void SwitchStyleCheck::checkLogicalExpressionInSwitch(const SwitchStmt* SS) {
    const Expr* condition = SS->getCond();
    if (!condition) return;
    
    LogicalExpressionChecker checker(this);
    checker.TraverseStmt(const_cast<Expr*>(condition));
}

void SwitchStyleCheck::checkDefaultLabel(const SwitchStmt* SS) {
    bool hasDefault = false;
    
    for (const SwitchCase* SC = SS->getSwitchCaseList(); 
         SC; SC = SC->getNextSwitchCase()) {
        if (isa<DefaultStmt>(SC)) {
            hasDefault = true;
            break;
        }
    }
    
    if (!hasDefault) {
        diag(SS->getSwitchLoc(),
             u8"switch문에는 반드시 default label이 포함되어야 합니다.");
    }
}

void SwitchStyleCheck::checkCodeBeforeFirstLabel(const SwitchStmt* SS) {
    const CompoundStmt* CompoundBody = dyn_cast<CompoundStmt>(SS->getBody());
    if (!CompoundBody) return;
    
    bool foundFirstLabel = false;
    for (const Stmt* S : CompoundBody->body()) {
        if (isa<CaseStmt>(S) || isa<DefaultStmt>(S)) {
            foundFirstLabel = true;
        } else if (!foundFirstLabel && !isa<NullStmt>(S)) {
            diag(S->getBeginLoc(), 
                 u8"switch 블록의 첫 번째 case/default label "
                 u8"전에는 코드가 올 수 없습니다.");
            break;
        }
    }
}

void SwitchStyleCheck::checkFallThrough(const SwitchStmt* SS, 
                                       const SourceManager& SM) {
    const CompoundStmt* Body = dyn_cast<CompoundStmt>(SS->getBody());
    if (!Body) return;
    
    FallThroughChecker checker(this, SM);
    checker.TraverseStmt(const_cast<CompoundStmt*>(Body));
    checker.checkLastCase();
}

} // namespace defenceStyle
} // namespace tidy
} // namespace clang

```