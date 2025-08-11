```shell
PS C:\Users\user\Desktop\ErrorTest\ErrorTest> clang-tidy .\*.cpp -checks="-*,defence-style-extern-function-declaration" -- -std=c++17 -w


clang-tidy .\*.cpp -checks="-*,defence-style-extern-function-declaration" -config="{CheckOptions: [{key: defence-style-extern-function-declaration.RequireExplicitExtern, value: true}]}" -- -std=c++17 -w
```


```cpp
#ifndef DEFENCESTYLE_EXTERNFUNCTIONDECLARATIONCHECK_H
#define DEFENCESTYLE_EXTERNFUNCTIONDECLARATIONCHECK_H

#include "../ClangTidyCheck.h"

namespace clang {
namespace tidy {
namespace defenceStyle {

class ExternFunctionDeclarationCheck : public ClangTidyCheck {
public:
  ExternFunctionDeclarationCheck(StringRef Name, ClangTidyContext *Context)
      : ClangTidyCheck(Name, Context),
        RequireExplicitExtern(
            Options.get("RequireExplicitExtern", false)) {}

  void registerMatchers(ast_matchers::MatchFinder *Finder) override;
  void check(const ast_matchers::MatchFinder::MatchResult &Result) override;

  void storeOptions(ClangTidyOptions::OptionMap &Opts) override {
    Options.store(Opts, "RequireExplicitExtern", RequireExplicitExtern);
  }

private:
  const bool RequireExplicitExtern; // 옵션: 외부 함수 선언에 extern 강제
};

} // namespace defenceStyle
} // namespace tidy
} // namespace clang

#endif // DEFENCESTYLE_EXTERNFUNCTIONDECLARATIONCHECK_H

```

```cpp
#include "ExternFunctionDeclarationCheck.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/AST/ASTContext.h"
#include "clang/AST/Decl.h"
#include "clang/Basic/SourceManager.h"
#include "../KCUtils.h"

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace defenceStyle {

// 시스템/빌트인 심볼 오탐 방지
static bool isFromSystemOrBuiltin(const FunctionDecl *FD,
                                  const SourceManager &SM) {
  if (!FD) return false;

  SourceLocation L = FD->getLocation();
  if (L.isInvalid()) return false;

  if (SM.isInSystemHeader(L) || SM.isInSystemMacro(L)) return true;
#if defined(__clang__)
  if (FD->getBuiltinID() != 0) return true;
#endif
  return false;
}

// 한 줄에 여러 함수가 선언된 경우 감지
bool isMultipleDeclarationLine(const SourceManager& SM, const FunctionDecl* FD) {
    if (!kcutils::isInMainFile(SM, FD)) return false;
    
    std::string lineText = kcutils::getSourceLineText(SM, FD->getBeginLoc());
    return kcutils::countDeclarationSeparatorCommas(lineText) > 0;
}

void ExternFunctionDeclarationCheck::registerMatchers(MatchFinder *Finder) {
  // 메인 소스에서 나타나는 "일반 함수 호출"만 검사
  Finder->addMatcher(
      callExpr(isExpansionInMainFile(),
               callee(functionDecl(unless(cxxMethodDecl()))))
          .bind("call"),
      this);
}

void ExternFunctionDeclarationCheck::check(
    const MatchFinder::MatchResult &Result) {

  const auto *CE = Result.Nodes.getNodeAs<CallExpr>("call");
  if (!CE) return;

  const auto &SM = *Result.SourceManager;
  const auto &LO = Result.Context->getLangOpts();

  const FunctionDecl *FD = CE->getDirectCallee();
  if (!FD) return;                      // 함수 포인터/람다 등 제외
  if (isFromSystemOrBuiltin(FD, SM)) return;

  SourceLocation CallLoc = CE->getExprLoc();
  if (CallLoc.isInvalid() || !SM.isInMainFile(CallLoc)) return;

  // C 모드에서의 "암묵 선언(implicit function declaration)" 탐지
  if (!LO.CPlusPlus && FD->isImplicit())
  {
    diag(CallLoc,
         u8"외부 함수 '%0'가 선언 없이 사용되었습니다. 사용 전에 선언(예: 'extern %1 %0(...);')하십시오.")
        << kcutils::getNodeName(FD)
        << FD->getReturnType().getAsString();
    return;
  }

  // 호출 지점 이전에 보이는 선언(프로토타입 또는 정의)이 있는지
  bool HasAnyDeclBefore = false;
  for (const FunctionDecl *Redecl : FD->redecls())
  {
    SourceLocation DLoc = Redecl->getBeginLoc();
    if (DLoc.isInvalid()) continue;
    if (SM.isBeforeInTranslationUnit(DLoc, CallLoc))
    {
      HasAnyDeclBefore = true;
      break;
    }
  }

  if (!HasAnyDeclBefore)
  {
    // 헤더 없이 호출하거나, 아래쪽 정의만 있는 호출
    diag(CallLoc,
         u8"함수 '%0'를 선언(프로토타입) 없이 사용했습니다. "
         u8"호출 이전에 선언을 제공하거나 헤더를 포함하십시오.")
        << kcutils::getNodeName(FD);
    // 이하 extern 강제는 "선언은 있으나 extern 미표기" 상황에만 의미가 있으므로 여기선 종료
    return;
  }

  // ▼ 옵션: 외부 함수라면 'extern' 명시를 강제 (기본 OFF)
  if (RequireExplicitExtern) 
  {
    // 이 파일(메인 파일) 안에 정의가 있으면 "외부"가 아니라고 판단 → 제외
    const FunctionDecl *Def = nullptr;
    bool HasDefInMainFile = (FD->hasBody(Def) && 
                             kcutils::isInMainFile(SM, Def)); 

    if (!HasDefInMainFile)
    {
      // 호출 이전의 선언들 중 하나라도 'extern'이 붙어 있어야 함
      bool PriorDeclHasExtern = false;
      const FunctionDecl *FirstPriorDecl = nullptr;

      for (const FunctionDecl *Redecl : FD->redecls()) 
      {
        SourceLocation DLoc = Redecl->getBeginLoc();
        if (!kcutils::hasValidLocation(Redecl)) continue; 
        if (!SM.isBeforeInTranslationUnit(DLoc, CallLoc)) continue;

        if (!FirstPriorDecl) FirstPriorDecl = Redecl;

        if (Redecl->getStorageClass() == SC_Extern)
        {
          PriorDeclHasExtern = true;
          break;
        }
      }

      if (!PriorDeclHasExtern && FirstPriorDecl) 
      {
        // 경고 위치: 호출 지점에 한 번만
        auto D = diag(CallLoc, u8"외부 함수 '%0'는 호출 이전 선언에서 "
                               u8"'extern'을 명시해야 합니다.")
                   << kcutils::getNodeName(FD); 

        // (선택) 간단한 FixIt: 첫 선언 앞에 "extern " 삽입
        //   - 복잡한 선언(속성/스토리지 조합)에서는 부정확할 수 있어 옵션화
        //   가능
        SourceLocation Ins = FirstPriorDecl->getBeginLoc();

        if (!kcutils::isComplexDeclaration(FirstPriorDecl) &&
            !isMultipleDeclarationLine(SM, FirstPriorDecl)) 
          D << FixItHint::CreateInsertion(Ins, "extern ");
        
      }
    }
  }

}

} // namespace defenceStyle
} // namespace tidy
} // namespace clang

```