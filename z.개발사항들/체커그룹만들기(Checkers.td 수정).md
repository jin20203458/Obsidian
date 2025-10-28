
## 🎯 핵심 원리

Clang Static Analyzer의 체커 그룹은 `Checkers.td` 파일에 정의된 **`Package`** 에 의해 결정됩니다. 새로운 `Package`를 선언하고, 당신의 체커들이 그 패키지에 속하도록 지정하면 됩니다.

---

## 📝 나만의 체커 그룹 만들기: 3단계

## 1단계: `Checkers.td` 파일에 새 패키지 정의

`Checkers.td` 파일의 패키지 정의 섹션에 당신의 커스텀 패키지를 추가합니다.

**예시: `ARQA` 라는 새로운 최상위 그룹 만들기**

```cpp
// clang/include/clang/StaticAnalyzer/Checkers/Checkers.td

// ... 기존 패키지 정의들 ...
def WebKit : Package<"webkit">;
def WebKitAlpha : Package<"webkit">, ParentPackage<Alpha>;

// ========================================================
// ✅ 1. 당신만의 새로운 패키지 추가
// ========================================================
def ARQA : Package<"arqa">;

// (선택) 실험적인 체커를 위한 alpha 하위 그룹 생성
def ARQAAlpha : Package<"arqa">, ParentPackage<Alpha>;
// ========================================================

//===----------------------------------------------------------------------===//
// Core Checkers.
//===----------------------------------------------------------------------===//
// ...
```

- **`def ARQA : Package<"arqa">;`**: `arqa` 라는 이름의 최상위 그룹을 만듭니다. 이제 `-analyzer-checker=arqa` 처럼 사용할 수 있습니다.
    
- **`def ARQAAlpha : ...`**: `alpha.arqa` 라는 실험용 하위 그룹을 만듭니다.
    

## 2단계: 체커를 새 패키지에 할당

이제 당신이 만든 커스텀 체커들을 새로운 `ARQA` 패키지에 포함시킵니다.

```cpp
// clang/include/clang/StaticAnalyzer/Checkers/Checkers.td

// ...

// ========================================================
// ✅ 2. 당신의 체커들을 새로운 패키지 아래에 정의
// ========================================================
let ParentPackage = ARQA in {

  def MyRepresentableCastChecker : Checker<"RepresentableCast">,
    HelpText<"Checks for integer casts that may lose information">,
    Documentation<HasDocumentation>,
    DescFile<"MyRepresentableCastChecker.cpp">;

  def MyUnreachableCodeChecker : Checker<"UnreachableCode">,
    HelpText<"Finds unreachable code blocks using CFG">,
    Documentation<HasDocumentation>,
    DescFile<"MyUnreachableCodeChecker.cpp">;

} // 'arqa' 패키지 끝

// (선택) 실험적인 체커는 ARQAAlpha 패키지에 할당
let ParentPackage = ARQAAlpha in {

  def MyExperimentalChecker : Checker<"ExperimentalFeature">,
    HelpText<"This is a new experimental checker">,
    Documentation<NotDocumented>,
    DescFile<"MyExperimentalChecker.cpp">;

} // 'alpha.arqa' 패키지 끝
```

- **`let ParentPackage = ARQA in { ... }`**: 이 블록 안의 모든 체커는 `arqa` 그룹에 속하게 됩니다.
    

## 3단계: 빌드 후 확인

LLVM/Clang을 다시 빌드한 후, 다음 명령어로 당신의 커스텀 그룹이 잘 추가되었는지 확인할 수 있습니다.

```cpp
# 모든 체커 도움말 보기 (arqa 그룹이 보여야 함)
clang -cc1 -analyzer-checker-help

# 당신의 그룹에 속한 모든 체커 활성화
clang --analyze -Xclang -analyzer-checker=arqa test.cpp

# 특정 체커만 활성화
clang --analyze -Xclang -analyzer-checker=arqa.RepresentableCast test.cpp

# 실험적인 체커 활성화
clang --analyze -Xclang -analyzer-checker=alpha.arqa.ExperimentalFeature test.cpp
```

---

## 💡 실전 예시: `ARQA` 그룹을 위한 전체 구조

## `Checkers.td`

```cpp
// ...
// 패키지 정의
def ARQA : Package<"arqa">;
def ARQAAlpha : Package<"arqa">, ParentPackage<Alpha>;

// ...
// 체커 정의
let ParentPackage = ARQA in {
  def MyRepresentableCastChecker : Checker<"RepresentableCast">,
    HelpText<"ARQA checker for representable casts">;
  def MyUnreachableCodeChecker : Checker<"UnreachableCode">,
    HelpText<"ARQA checker for unreachable code">;
}
```

## `CMakeLists.txt`

```cpp
# clang/lib/StaticAnalyzer/Checkers/CMakeLists.txt
add_clang_library(clangStaticAnalyzerCheckers
  # ...
  MyRepresentableCastChecker.cpp
  MyUnreachableCodeChecker.cpp
  # ...
)
```

## 체커 소스 파일

각 체커 파일에는 등록을 위한 함수가 필요합니다.[clang-analyzer.llvm](https://clang-analyzer.llvm.org/checker_dev_manual.html)​

```cpp
// MyRepresentableCastChecker.cpp
#include "clang/StaticAnalyzer/Checkers/BuiltinCheckerRegistration.h"

// ... 체커 구현 ...

// TableGen이 이 함수를 호출하여 체커를 등록
void ento::registerMyRepresentableCastChecker(CheckerManager &mgr) {
  mgr.registerChecker<MyRepresentableCastChecker>();
}
```

**결론적으로, `Checkers.td` 파일에 `Package`를 선언하고, `let ParentPackage = ...` 블록을 사용하여 당신의 체커들을 그 패키지에 할당하면, 자신만의 체커 그룹을 완벽하게 만들 수 있습니다.** 🎯
