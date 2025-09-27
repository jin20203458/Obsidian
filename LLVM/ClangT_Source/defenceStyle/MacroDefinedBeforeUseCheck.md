```cpp
#ifndef DEFENCESTYLE_MACRODEFINEDBEFOREUSECHECK_H
#define DEFENCESTYLE_MACRODEFINEDBEFOREUSECHECK_H

#include "../ClangTidyCheck.h"
#include "clang/Lex/PPCallbacks.h"

namespace clang::tidy::defenceStyle {

class MacroDefinedBeforeUseCheck : public ClangTidyCheck {
public:
  MacroDefinedBeforeUseCheck(StringRef Name, ClangTidyContext *Ctx)
      : ClangTidyCheck(Name, Ctx) {}

  void registerPPCallbacks(const SourceManager &SM, Preprocessor *PP,
                           Preprocessor *ModuleExpanderPP) override;
};

} // namespace clang::tidy::defenceStyle
#endif
```

```cpp
#include "MacroDefinedBeforeUseCheck.h"
#include "../KCUtils.h"                    
#include "clang/Lex/PPCallbacks.h"
#include "clang/Lex/Preprocessor.h" 
#include "clang/Basic/SourceLocation.h"

using namespace clang;

namespace clang::tidy::defenceStyle {

namespace {

class DefinedTracker {
private:
  bool PrevWasDefined = false;  // 직전 토큰이 "defined"였나?
  bool InDefinedParens = false; // defined(...) 괄호 안에 있나?

public:
  // defined 키워드 발견 시 호출
  void foundDefined() { PrevWasDefined = true; }

  // '(' 토큰 발견 시 호출
  void foundLeftParen() 
  {
    if (PrevWasDefined)
    {
      InDefinedParens = true;
      PrevWasDefined = false; // defined( 처리됨
    }
  }

  // ')' 토큰 발견 시 호출
  void foundRightParen() { InDefinedParens = false; }

  // 기타 토큰에서 상태 리셋
  void reset()
  {
    PrevWasDefined = false;
    // InDefinedParens는 ')' 만날 때까지 유지
  }

  // 현재 매크로를 건너뛰어야 하는지 판단
  bool shouldSkipMacro() const { return PrevWasDefined || InDefinedParens; }

  // 디버깅용 상태 출력
  void debugPrint(const std::string &context) const {
    llvm::errs() << "[" << context << "] PrevWasDefined=" << PrevWasDefined
                 << ", InDefinedParens=" << InDefinedParens << "\n";
  }
};

/**
 * PPCallbacks를 상속받아 전처리기 조건부 지시문(#if, #elif)을 감지하는 콜백 클래스
 * 전처리기가 #if나 #elif를 만나면 자동으로 해당 메서드들이 호출됨
 */
class MacroPPCallbacks : public PPCallbacks {
public:
    /**
     * 생성자: 필요한 Clang 객체들을 참조로 저장
     * @param SM: 소스코드 위치 정보 관리자 (파일, 라인, 컬럼 등)
     * @param PP: 전처리기 객체 (매크로 정의 상태, 토큰화 등)
     * @param Check: ClangTidy 체커 객체 (경고 메시지 출력용)
     */
  MacroPPCallbacks(const SourceManager &SM, Preprocessor &PP,
                   ClangTidyCheck &Check)
      : SM(SM), PP(PP), Check(Check) { }
    /**
     * #if 지시문을 만났을 때 자동 호출되는 콜백
     * 예: #if SOME_MACRO → 이 함수가 자동으로 호출됨
     * 
     * @param Loc: #if 키워드의 정확한 소스 위치
     * @param ConditionRange: 조건식 부분의 범위 (예: "SOME_MACRO" 부분)
     * @param ConditionValue: 조건식의 평가 결과 (참/거짓/미평가)
     */
    void If(clang::SourceLocation Loc, 
            clang::SourceRange ConditionRange,
            clang::PPCallbacks::ConditionValueKind ConditionValue) override {
        analyseCondition(ConditionRange);
    }

    /**
     * #elif 지시문을 만났을 때 자동 호출되는 콜백  
     * 예: #elif OTHER_MACRO → 이 함수가 자동으로 호출됨
     * 
     * @param Loc: #elif 키워드의 정확한 소스 위치
     * @param ConditionRange: 조건식 부분의 범위 (예: "OTHER_MACRO" 부분)
     * @param ConditionValue: 조건식의 평가 결과 (참/거짓/미평가)
     * @param IfLoc: 대응하는 #if의 위치 (elif는 if와 쌍을 이뤄야 하므로)
     */
    void Elif(clang::SourceLocation Loc, 
              clang::SourceRange ConditionRange,
              clang::PPCallbacks::ConditionValueKind ConditionValue,
              clang::SourceLocation IfLoc) override {
        analyseCondition(ConditionRange);
    }

private:
  const SourceManager &SM;
  Preprocessor &PP;
  ClangTidyCheck &Check;

  enum MacroDefinitionStatus {
    UNDEFINED = 0,         // 정의 안됨
    DEFINED_EMPTY = 1,     // 정의됨 (값 없음)
    DEFINED_WITH_VALUE = 2 // 값까지 정의됨
  };

  MacroDefinitionStatus getMacroDefinitionStatus(IdentifierInfo *II) {
    if (!PP.isMacroDefined(II)) {
      return UNDEFINED; // 0: 아예 정의 안됨
    }

    // 정의는 되어 있는데 값이 있는가?
    const MacroInfo *MI = PP.getMacroInfo(II);
    if (!MI || MI->getNumTokens() == 0) {
      return DEFINED_EMPTY; // 1: 값이 없음
    }

    return DEFINED_WITH_VALUE; // 2: 값까지 있음
  }
  
  void checkMacroDefinition(IdentifierInfo *II, const Token &Tok) {
    SourceLocation DiagLoc = SM.getSpellingLoc(Tok.getLocation());

    MacroDefinitionStatus status = getMacroDefinitionStatus(II);

    switch (status) {
    case UNDEFINED:
      Check.diag(DiagLoc,
                 u8"전처리 조건식에서 매크로 '%0' 가 정의되지 않았습니다.")
          << II->getName();
      break;

    case DEFINED_EMPTY:
      Check.diag(DiagLoc,
                 u8"전처리 조건식에서 매크로 '%0' 가 값 없이 정의되었습니다.")
          << II->getName();
      break;

    case DEFINED_WITH_VALUE:
      // 정상 상태 - 경고 없음
      break;
    }
  }

  /// CondRange 의 토큰을 재‑lex 해서 미정의 식별자 탐색
  void analyseCondition(SourceRange CondRange) {
    
    if (CondRange.isInvalid())
      return;

    SourceLocation StartLoc = CondRange.getBegin();
    if (!StartLoc.isValid() || !SM.isInMainFile(StartLoc)) 
      return; // 시스템 헤더는 아예 처리 안함
    
    StringRef Buffer =
        Lexer::getSourceText(CharSourceRange::getTokenRange(CondRange), SM,
                             PP.getLangOpts(), /*Invalid=*/nullptr);
    if (Buffer.empty())
      return;

    // 공백과 개행 문자 추가로 토큰 경계 명확히
    std::string SafeBuffer = Buffer.str() + " \n";

    Lexer Lex(SM.getExpansionLoc(CondRange.getBegin()), PP.getLangOpts(),
              SafeBuffer.c_str(), SafeBuffer.c_str(),
              SafeBuffer.c_str() + SafeBuffer.size());

    Token Tok;
    DefinedTracker tracker;

    while (!Lex.LexFromRawLexer(Tok))
    {

      if (Tok.is(tok::eof) || Tok.is(tok::unknown)) 
        break;
      

      if (Tok.is(tok::raw_identifier))
       {
        IdentifierInfo *II = PP.getIdentifierInfo(Tok.getRawIdentifier());
        Tok.setIdentifierInfo(II);
        Tok.setKind(II->getTokenID());


        if (II->getName() == "defined") {
          tracker.foundDefined();
          continue;
        }

        if (tracker.shouldSkipMacro()) {
          tracker.reset();
          continue;
        }

        checkMacroDefinition(II, Tok);

      }
       else if (Tok.is(tok::l_paren)) {
        tracker.foundLeftParen();
      }
       else if (Tok.is(tok::r_paren)) {
        tracker.foundRightParen();
      } 
      else {
        tracker.reset();
      }
    }

  }
};

} // namespace
void MacroDefinedBeforeUseCheck::registerPPCallbacks(const SourceManager &SM,
                                                     Preprocessor *PP,
                                                     Preprocessor *) {
  // 시스템 헤더까지 검사하려면 isInMainFile() 조건 제거
  PP->addPPCallbacks(std::make_unique<MacroPPCallbacks>(SM, *PP, *this));
}

} // namespace clang::tidy::defenceStyle

```