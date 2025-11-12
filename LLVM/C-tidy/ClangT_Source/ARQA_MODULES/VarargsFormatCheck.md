```cpp
#include "VarargsFormatCheck.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/AST/Expr.h"
#include "clang/AST/Decl.h"
#include "clang/Basic/SourceManager.h"
#include "clang/Lex/Lexer.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/SmallVector.h"
#include <cctype>

using namespace clang;
using namespace clang::ast_matchers;

namespace clang::tidy::defenceStyle {

// =================== 유틸/분류 ===================

enum class Family { Printf, Scanf, Unknown };

// 함수에 GNU 포맷 속성(__attribute__((format)))이 있는지 확인하고 정보 추출
static bool hasGnuFormatAttr(const FunctionDecl *FD, Family &F, unsigned &FmtIdx, unsigned &FirstArg) {
  // 간단 파서: GNU 형식 속성이 있으면 읽는다. 실패 시 false
  for (auto *A : FD->specific_attrs<FormatAttr>())
  {
    StringRef K = A->getType()->getName();
    if (K.equals_insensitive("printf")) F = Family::Printf;
    else if (K.equals_insensitive("scanf")) F = Family::Scanf;
    else continue;

    // 포맷 문자열이 어느 위치에 있는지 
    FmtIdx = A->getFormatIdx() - 1;     // GNU attr는 1-based

    // 첫 번째 가변인자가 어느 위치부터 시작하는지
    FirstArg = A->getFirstArg() - 1;
    return true;
  }
  return false;
}

// GNU 속성이 없는 함수들을 함수명만 보고 Printf/Scanf 계열인지 분류하는 "fallback" 함수
static Family classifyBuiltinFamily(const FunctionDecl *FD, bool StrictWCharFamily) {

  if (!FD) return Family::Unknown;

  StringRef N = FD->getName();
  if (N.empty()) return Family::Unknown;

  auto isOneOf = [&](std::initializer_list<const char*> names){
    for (auto s: names) if ((N == s)) return true; return false;
  };

  if (isOneOf({"printf","fprintf","sprintf","snprintf","vprintf","vfprintf","vsnprintf","dprintf"}))
    return Family::Printf;
  if (StrictWCharFamily &&
      isOneOf({"wprintf","fwprintf","swprintf","vwprintf","vswprintf","vfwprintf"}))
    return Family::Printf; // wide도 printf 규칙(지시자 차이만), 군 일관성을 별도 검사
  if (isOneOf({"scanf","fscanf","sscanf","vscanf","vfscanf","vsscanf"}))
    return Family::Scanf;
  if (StrictWCharFamily &&
      isOneOf({"wscanf","fwscanf","swscanf","vwscanf","vfwscanf","vswscanf"}))
    return Family::Scanf;
  return Family::Unknown;
}

// =================== 포맷 파서 ===================

enum class LenMod { None, HH, H, L, LL, J, Z, T, CapL, MS_I32, MS_I64 };

//  포맷 문자열을 각각 토큰화시켜 저장하는 구조체
struct Token {
  // 변환 지시자 하나를 나타냄 (%d 등)
  unsigned Pos = 0;        // 위치 인자(1-based), 0이면 비위치
  bool FromStar = false;   // 폭/정밀도 '*' 자리
  char Conv = 0;           // 'd','s','f','p','c','n','%','[', ...
  LenMod Len = LenMod::None;
  bool ForWidth = false;   // '*'가 폭인지
  bool ForPrec = false;    // '*'가 정밀도인지
  unsigned RawOffset = 0;  // 디버그용
};

// 전체 포맷 문자열을 파싱한 결과를 담는 구조체
struct Parsed {
  bool Positional = false; // 위치 인자 사용 여부
  llvm::SmallVector<Token, 16> Items; // '*'도 토큰으로 넣음
  std::string Error;
};

// 문자가 숫자인지 판별
static bool isDigit(char c){ return c>='0' && c<='9'; }

//  printf 포맷 문자열 파서
static Parsed parsePrintfLike(StringRef S, bool HonorMSVCExt) {
  Parsed P;
  const char *p=S.begin(), *e=S.end();

  auto readUint = [&](unsigned &out)->bool{
    const char* s=p; unsigned v=0; if (p==e||!isDigit(*p)) return false;
    while (p!=e && isDigit(*p)) { v = v*10 + (*p - '0'); ++p; }
    out=v; return p>s;
  };

  while (p!=e) {
    if (*p!='%') { ++p; continue; }
    const char* start = p++; // consume '%'
    if (p!=e && *p=='%') { ++p; continue; } // "%%"
    Token T{}; T.RawOffset = (unsigned)(start - S.begin());

    // (1) 위치 인자 "%3$"
    {
      const char* save=p; unsigned n=0;
      if (readUint(n) && p!=e && *p=='$') { T.Pos = n; ++p; P.Positional=true; }
      else p=save;
    }
    // (2) 플래그들: "-+ #0" (무시)
    while (p!=e && strchr("-+ #0", *p)) ++p;

    // (3) 폭: 숫자 또는 '*'
    if (p!=e && *p=='*') { Token W; W.FromStar=true; W.ForWidth=true; ++p; P.Items.push_back(W); }
    else { unsigned dummy; readUint(dummy); }

    // (4) 정밀도: .숫자 또는 .* 
    if (p!=e && *p=='.') {
      ++p;
      if (p!=e && *p=='*') { Token Prec; Prec.FromStar=true; Prec.ForPrec=true; ++p; P.Items.push_back(Prec); }
      else { unsigned dummy; readUint(dummy); }
    }

    // (5) 길이 수식자
    if (p!=e) {
      if (*p=='h'){ ++p; T.Len = (p!=e && *p=='h') ? (++p,LenMod::HH):LenMod::H; }
      else if (*p=='l'){ ++p; T.Len = (p!=e && *p=='l') ? (++p,LenMod::LL):LenMod::L; }
      else if (*p=='j'){ ++p; T.Len = LenMod::J; }
      else if (*p=='z'){ ++p; T.Len = LenMod::Z; }
      else if (*p=='t'){ ++p; T.Len = LenMod::T; }
      else if (*p=='L'){ ++p; T.Len = LenMod::CapL; }
      else if (HonorMSVCExt && *p=='I'){ // I32/I64
        ++p; if (p+1<=e && (p!=e)&&isdigit(*p)) {
          if (p+1<e && p[0]=='3' && p[1]=='2') { p+=2; T.Len=LenMod::MS_I32; }
          else if (p+1<e && p[0]=='6' && p[1]=='4') { p+=2; T.Len=LenMod::MS_I64; }
        }
      }
    }

    // (6) 변환 문자
    if (p==e) { P.Error="끝난 % 지시자"; break; }
    T.Conv = *p++;
    P.Items.push_back(T);
  }
  return P;
}

// =================== 기대 타입/대조 ===================

enum class K { Any, Int, UInt, SChar, UChar, Short, UShort, Long, ULong, LongLong, ULongLong,
               SizeT, IntMaxT, PtrDiffT, WIntT, Ptr, CharPtr, WCharPtr, Double, LongDouble, Char, WChar };

static K expectedPrintfKind(const Token &T) {
  // https://en.cppreference.com/w/c/io/fprintf 의 표에 근거
  switch (T.Conv) {
    case 'd': case 'i':
      switch (T.Len) {
        case LenMod::HH: return K::SChar;
        case LenMod::H:  return K::Short;
        case LenMod::L:  return K::Long;
        case LenMod::LL: return K::LongLong;
        case LenMod::J:  return K::IntMaxT;
        case LenMod::Z:  return K::SizeT;      // (주의) 표준은 %zd: ptrdiff_t(부호 있는) → K::PtrDiffT
        case LenMod::T:  return K::PtrDiffT;
        default:         return K::Int;        // 기본승격 후 int로
      }
    case 'u': case 'o': case 'x': case 'X':
      switch (T.Len) {
        case LenMod::HH: return K::UChar;
        case LenMod::H:  return K::UShort;
        case LenMod::L:  return K::ULong;
        case LenMod::LL: return K::ULongLong;
        case LenMod::J:  return K::IntMaxT;    // uintmax_t 별도 구분 가능
        case LenMod::Z:  return K::SizeT;      // %zu
        case LenMod::T:  return K::PtrDiffT;   // 이 행은 구현차 존재. 엄밀히는 부호 없는 ptrdiff_t가 없음 → 경고 유도도 가능
        default:         return K::UInt;
      }
    case 'c': return (T.Len==LenMod::L) ? K::WChar : K::Int; // 기본승격: char → int
    case 's': return (T.Len==LenMod::L) ? K::WCharPtr : K::CharPtr;
    case 'p': return K::Ptr;
    case 'f': case 'F': case 'e': case 'E': case 'g': case 'G': case 'a': case 'A':
      return (T.Len==LenMod::CapL) ? K::LongDouble : K::Double;
    case 'n': return K::Ptr; // int* (정확히는 int*/short*/long long* 등 길이 수식자 반영)
    case '[': // %[...s]는 scanf에만, printf에는 없음
    default:  return K::Any;
  }
}

static K expectedScanfKind(const Token &T) {
  // scanf는 포인터 요구 (할당 생략 *면 예외)
  // 여기서는 FromStar=false 지시자만 검사하고, 실제 대조 시 포인터인지 확인
  switch (T.Conv) {
    case 'd': case 'i':
      switch (T.Len) {
        case LenMod::HH: return K::SChar;
        case LenMod::H:  return K::Short;
        case LenMod::L:  return K::Long;
        case LenMod::LL: return K::LongLong;
        case LenMod::J:  return K::IntMaxT;
        case LenMod::Z:  return K::SizeT;     // %zd → ptrdiff_t*, 엄밀히는 부호있음
        case LenMod::T:  return K::PtrDiffT;
        default:         return K::Int;
      }
    case 'u': case 'o': case 'x': case 'X':
      switch (T.Len) {
        case LenMod::HH: return K::UChar;
        case LenMod::H:  return K::UShort;
        case LenMod::L:  return K::ULong;
        case LenMod::LL: return K::ULongLong;
        case LenMod::J:  return K::IntMaxT;   // uintmax_t 별도 구분 고려
        case LenMod::Z:  return K::SizeT;
        case LenMod::T:  return K::PtrDiffT;
        default:         return K::UInt;
      }
    case 'c': return (T.Len==LenMod::L) ? K::WCharPtr : K::CharPtr; // 배열/포인터
    case 's': return (T.Len==LenMod::L) ? K::WCharPtr : K::CharPtr;
    case 'p': return K::Ptr;   // void**
    case '[': return K::CharPtr; // scanset → char* (또는 wchar_t* with 'l')
    case 'n': return K::Ptr;   // int* 계열
    case 'f': case 'F': case 'e': case 'E': case 'g': case 'G': case 'a': case 'A':
      return (T.Len==LenMod::CapL) ? K::LongDouble : K::Double;
    default:  return K::Any;
  }
}

static bool isSignedIntType(QualType QT){ return QT->isIntegerType() && QT->isSignedIntegerType(); }
static bool isUnsignedIntType(QualType QT){ return QT->isIntegerType() && QT->isUnsignedIntegerType(); }

static bool typeMatchesPrintf(K need, QualType QT, const ASTContext &Ctx) {
  QT = QT.getCanonicalType();
  // default promotions: float -> double, char/signed char/short -> int
  switch (need) {
    case K::Int:      return isSignedIntType(QT);
    case K::UInt:     return isUnsignedIntType(QT);
    case K::SChar:    return QT->isSpecificBuiltinType(BuiltinType::Int); // 승격 후 int
    case K::UChar:    return QT->isSpecificBuiltinType(BuiltinType::Int); // 경고 완화 가능
    case K::Short:    return QT->isSpecificBuiltinType(BuiltinType::Int);
    case K::UShort:   return QT->isSpecificBuiltinType(BuiltinType::Int);
    case K::Long:     return QT->isSpecificBuiltinType(BuiltinType::Long);
    case K::ULong:    return QT->isSpecificBuiltinType(BuiltinType::ULong);
    case K::LongLong: return QT->isSpecificBuiltinType(BuiltinType::LongLong);
    case K::ULongLong:return QT->isSpecificBuiltinType(BuiltinType::ULongLong);
    case K::SizeT:    return QT->isSpecificBuiltinType(BuiltinType::ULong) || QT->isSpecificBuiltinType(BuiltinType::UInt128) || QT->isSpecificBuiltinType(BuiltinType::UInt);
    case K::PtrDiffT: return isSignedIntType(QT); // 근사
    case K::IntMaxT:  return isSignedIntType(QT);
    case K::Ptr:      return QT->isPointerType();
    case K::CharPtr:  return QT->isPointerType() && QT->getPointeeType()->isAnyCharacterType();
    case K::WCharPtr: return QT->isPointerType() && QT->getPointeeType()->isWideCharType();
    case K::Double:   return QT->isSpecificBuiltinType(BuiltinType::Double);
    case K::LongDouble:return QT->isSpecificBuiltinType(BuiltinType::LongDouble);
    case K::Char:     return QT->isSpecificBuiltinType(BuiltinType::Int);
    case K::WChar:    return QT->isSpecificBuiltinType(BuiltinType::Int); // wint_t → 승격 후 int(플랫폼에 따라)
    case K::Any:      return true;
  }
  return true;
}

static bool typeMatchesScanf(K need, QualType QT, const ASTContext &Ctx, bool RequireExact) {
  QT = QT.getCanonicalType();
  // scanf는 원칙적으로 "정확 포인터" 요구 (RequireExact=true 권장)
  if (need==K::CharPtr || need==K::WCharPtr) {
    if (!QT->isPointerType()) return false;
    return (need==K::CharPtr) ? QT->getPointeeType()->isAnyCharacterType()
                              : QT->getPointeeType()->isWideCharType();
  }
  if (need==K::Ptr) {
    return QT->isPointerType(); // %p → void** (엄밀하게 void** 체크도 가능)
  }
  // 정수/부동은 포인터 요구
  if (!QT->isPointerType()) return false;
  QualType PT = QT->getPointeeType().getCanonicalType();
  auto same = [&](BuiltinType::Kind K){ return PT->isSpecificBuiltinType(K); };

  switch (need) {
    case K::Int:      return same(BuiltinType::Int);
    case K::UInt:     return same(BuiltinType::UInt);
    case K::Short:    return same(BuiltinType::Short);
    case K::UShort:   return same(BuiltinType::UShort);
    case K::SChar:    return same(BuiltinType::SChar) || (!RequireExact && same(BuiltinType::Char_S));
    case K::UChar:    return same(BuiltinType::UChar) || (!RequireExact && same(BuiltinType::Char_U));
    case K::Long:     return same(BuiltinType::Long);
    case K::ULong:    return same(BuiltinType::ULong);
    case K::LongLong: return same(BuiltinType::LongLong);
    case K::ULongLong:return same(BuiltinType::ULongLong);
    case K::SizeT:    return same(BuiltinType::UInt) || same(BuiltinType::ULong) || same(BuiltinType::ULongLong);
    case K::PtrDiffT: return same(BuiltinType::Int) || same(BuiltinType::Long);
    case K::IntMaxT:  return same(BuiltinType::LongLong) || same(BuiltinType::Int128);
    case K::Double:   return same(BuiltinType::Double);
    case K::LongDouble:return same(BuiltinType::LongDouble);
    default:          return true;
  }
}

// =================== 매처 등록 ===================

void VarargsFormatCheck::registerMatchers(MatchFinder *F) {
    // 함수 호출부, 그리고 해당하는 선언 바인딩
  F->addMatcher(
    callExpr(
      isExpansionInMainFile(),
      callee(functionDecl().bind("callee")))
      .bind("call"),
    this);
}

// =================== 본 검사 ===================

void VarargsFormatCheck::check(const MatchFinder::MatchResult &R) {
  const auto *CE = R.Nodes.getNodeAs<CallExpr>("call");
  const auto *FD = R.Nodes.getNodeAs<FunctionDecl>("callee");
  if (!CE || !FD) return;

  const SourceManager &SM = *R.SourceManager;
  SourceLocation L = CE->getExprLoc();
  if (OnlyMainFile && !SM.isInMainFile(SM.getSpellingLoc(L))) return;
  if (IgnoreMacroExpansions && SM.getSpellingLoc(L).isMacroID()) return;

  // 어떤 패밀리인가?
  Family Fam = Family::Unknown;
  unsigned FmtIdx = 0, FirstArg = 1;
  if (!hasGnuFormatAttr(FD, Fam, FmtIdx, FirstArg))
    Fam = classifyBuiltinFamily(FD, StrictWCharFamily);

  if ((Fam==Family::Printf && !EnablePrintf) || (Fam==Family::Scanf && !EnableScanf) || Fam==Family::Unknown)
    return;

  if (CE->getNumArgs() <= FmtIdx) return;
  const Expr *FmtArg = CE->getArg(FmtIdx)->IgnoreParenImpCasts();

  // 형식 문자열 추출
  StringRef Fmt;
  if (const auto *SL = dyn_cast<StringLiteral>(FmtArg)) {
    Fmt = SL->getString();
  } else {
    if (!AllowDynamicFormat) return; // 동적 포맷은 소음 방지
    // 약경고를 주고 끝낼 수도 있음
    return;
  }

  // 파싱
  Parsed P = parsePrintfLike(Fmt, HonorMSVCExt);
  if (!P.Error.empty()) {
    diag(FmtArg->getExprLoc(), u8"형식 문자열 구문 오류: %0") << P.Error;
    return;
  }

  // '%n' 보안 금지
  if (WarnOnPercentN) {
    for (auto &T : P.Items) if (!T.FromStar && T.Conv=='n')
      diag(CE->getExprLoc(), u8"보안상 위험한 '%%n' 지시자 사용을 금지합니다.");
  }

  // 개수 대조(비위치 모드)
  if (!P.Positional) {
    // '*'는 폭/정밀도 자리 → 각각 int 필요
    unsigned wants = 0;
    for (auto &T : P.Items) if (!T.FromStar) ++wants;
    unsigned provided = (unsigned)CE->getNumArgs() - (FmtIdx + 1);
    if (provided != wants) {
      diag(CE->getExprLoc(),
           u8"포맷 문자열은 %0개의 인자를 요구하지만 실제로는 %1개가 전달되었습니다.")
        << wants << provided;
      // 계속해서 타입 진단도 수행(가능한 범위 내)
    }
  }

  // 타입 대조
  const ASTContext &Ctx = *R.Context;
  auto getArgQT = [&](unsigned oneBased)->QualType {
    unsigned idx = FmtIdx + oneBased;
    return (idx < CE->getNumArgs()) ? CE->getArg(idx)->getType() : QualType();
  };

  unsigned pos = 1;
  for (auto &T : P.Items) {
    if (T.FromStar) {
      if (PrintfZeroWidthStarAsInt && Fam==Family::Printf) {
        QualType QT = getArgQT(pos);
        if (QT.isNull() || !QT->isIntegerType())
          diag(CE->getExprLoc(), u8"'*'에는 int 계열 인자가 필요합니다(인자 #%0).") << pos;
      }
      ++pos; continue;
    }
    K need = (Fam==Family::Printf) ? expectedPrintfKind(T) : expectedScanfKind(T);
    QualType QT = getArgQT(pos);
    if (QT.isNull()) {
      diag(CE->getExprLoc(), u8"인자 #%0이 누락되었습니다.") << pos;
      ++pos; continue;
    }

    bool ok = (Fam==Family::Printf) ? typeMatchesPrintf(need, QT, Ctx)
                                    : typeMatchesScanf(need, QT, Ctx, ScanfRequirePointerExact);
    if (!ok) {
      if (Fam==Family::Printf) {
        diag(CE->getExprLoc(),
             u8"포맷 지시자 '%0'와 인자 #%1의 타입이 일치하지 않습니다.")
          << StringRef((const char*)&T.Conv,1) << pos;
      } else {
        diag(CE->getExprLoc(),
             u8"scanf 지시자 '%0'에 맞는 포인터 타입이 아닙니다(인자 #%1).")
          << StringRef((const char*)&T.Conv,1) << pos;
      }
    }
    ++pos;
  }
}

void VarargsFormatCheck::storeOptions(ClangTidyOptions::OptionMap &Opts) {
  Options.store(Opts, "EnablePrintf", EnablePrintf);
  Options.store(Opts, "EnableScanf", EnableScanf);
  Options.store(Opts, "AllowDynamicFormat", AllowDynamicFormat);
  Options.store(Opts, "HonorMSVCExt", HonorMSVCExt);
  Options.store(Opts, "IgnoreMacroExpansions", IgnoreMacroExpansions);
  Options.store(Opts, "OnlyMainFile", OnlyMainFile);
  Options.store(Opts, "WarnOnPercentN", WarnOnPercentN);
  Options.store(Opts, "StrictWCharFamily", StrictWCharFamily);
  Options.store(Opts, "ScanfRequirePointerExact", ScanfRequirePointerExact);
  Options.store(Opts, "PrintfZeroWidthStarAsInt", PrintfZeroWidthStarAsInt);
}

} // namespace clang::tidy::defenceStyle

```