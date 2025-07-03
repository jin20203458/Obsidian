
> **C/C++ 프로그램에서 `errno`라는 전역 시스템 값을 정적으로 추적**하기 위한  
  **Clang Static Analyzer용 “errno 상태 관리 API”**를 정의한 헤더입니다.
  
- errno는 시스템 콜이나 c표준라이브러리 함수들이 주로 사용
```cpp
//=== ErrnoModeling.h - Tracking value of 'errno'. -----------------*- C++ -*-//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// Defines inter-checker API for using the system value 'errno'.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_CLANG_LIB_STATICANALYZER_CHECKERS_ERRNOMODELING_H
#define LLVM_CLANG_LIB_STATICANALYZER_CHECKERS_ERRNOMODELING_H

#include "clang/StaticAnalyzer/Core/PathSensitive/CheckerContext.h"
#include "clang/StaticAnalyzer/Core/PathSensitive/ProgramState.h"
#include "clang/StaticAnalyzer/Core/PathSensitive/SVals.h"
#include <optional>

namespace clang {
namespace ento {
namespace errno_modeling {

/// 체커가 errno를 읽고 쓰는 상황을 어떻게 처리하는지 구분하는 값
enum ErrnoCheckState : unsigned {
  /// errno에 대해 아무 정보도 모르는 상태. 읽기/쓰기가 항상 허용됨
  Irrelevant = 0,

  /// 이전 함수 호출이 실패했는지 확인하려면 반드시 errno를 체크해야 함.
  /// 이 상태일 때 다음 표준 함수 호출 또는 errno 값이 덮어써지기 전에
  /// errno를 읽지 않으면 경고가 발생
  MustBeChecked = 1,

  /// errno의 값이 확실하지 않으므로, 덮어써지거나 무효화되기 전까지는
  /// errno를 읽으면 안 됨
  MustNotBeChecked = 2
};

/// AST에서 'errno'가 발견되면 'errno' 값을 반환합니다.
std::optional<SVal> getErrnoValue(ProgramStateRef State);

/// 'errno'가 발견되지 않은 경우 오류 검사 상태 \\c Errno_Irlated를 반환합니다
/// (이 값은 이뿐만이 아닙니다).
ErrnoCheckState getErrnoState(ProgramStateRef State);


/// errno 값이 저장된 메모리 영역(Loc)을 반환. AST에서 찾지 못하면 nullopt
std::optional<Loc> getErrnoLoc(ProgramStateRef State);

/// Set value of 'errno' to any SVal, if possible.
/// The errno check state is set always when the 'errno' value is set.
ProgramStateRef setErrnoValue(ProgramStateRef State,
                              const LocationContext *LCtx, SVal Value,
                              ErrnoCheckState EState);

/// Set value of 'errno' to a concrete (signed) integer, if possible.
/// The errno check state is set always when the 'errno' value is set.
ProgramStateRef setErrnoValue(ProgramStateRef State, CheckerContext &C,
                              uint64_t Value, ErrnoCheckState EState);

/// Set the errno check state, do not modify the errno value.
ProgramStateRef setErrnoState(ProgramStateRef State, ErrnoCheckState EState);

/// Clear state of errno (make it irrelevant).
ProgramStateRef clearErrnoState(ProgramStateRef State);

/// 특정 함수 호출이 errno의 주소를 반환하는 시스템 내부 함수인지 판별
bool isErrnoLocationCall(const CallEvent &Call);

/// errno 메모리 영역이 관심 대상으로 표시된 경우, 메세지를 붙이는 NoteTag를 생성
const NoteTag *getErrnoNoteTag(CheckerContext &C, const std::string &Message);

/// 표준 함수 호출이 성공한 경우, errno 상태를 '읽지 말 것'으로 바꾼다
ProgramStateRef setErrnoForStdSuccess(ProgramStateRef State, CheckerContext &C);

/// (표준 함수가 실패한 경우 errno를 0이 아닌 값(에러코드)으로 만들고,
/// 상태는 Irrelevant로 세팅해서 이후 분석에서 더 이상 체크/경고가 발생하지 않게 함)
ProgramStateRef setErrnoForStdFailure(ProgramStateRef State, CheckerContext &C,
                                      NonLoc ErrnoSym);

/// 표준 함수가 실패를 오직 errno 값을 통해서만 알릴 때,
/// errno 상태를 반드시 체크해야(MustBeChecked) 하는 상태로 만든다.
ProgramStateRef setErrnoStdMustBeChecked(ProgramStateRef State,
                                         CheckerContext &C, const Expr *InvalE);

} // namespace errno_modeling
} // namespace ento
} // namespace clang

#endif // LLVM_CLANG_LIB_STATICANALYZER_CHECKERS_ERRNOMODELING_H

```

- C 언어에서 `errno`는 사실상 전역 변수입니다.
    
- Static Analyzer는 분석 중에  
    `errno`가 저장된 메모리 영역(=MemoryRegion)이  
    **언제나(프로그램 어디서나) 접근 가능**하다는 걸 알고 있으니  
    "글로벌 메모리 공간"에 위치한 것으로 간주합니다.