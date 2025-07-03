>Clang Static Analyzer의 커스텀 체크에서 자주 쓰이는 **메모리 상태 추적 헬퍼** 헤더

```cpp
//===--- AllocationState.h ------------------------------------- *- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_CLANG_LIB_STATICANALYZER_CHECKERS_ALLOCATIONSTATE_H
#define LLVM_CLANG_LIB_STATICANALYZER_CHECKERS_ALLOCATIONSTATE_H

// 버그 리포트에 추가적인 진단 정보를 제공하는 "방문자(visitor)" 클래스들을 정의
#include "clang/StaticAnalyzer/Core/BugReporter/BugReporterVisitors.h"

// ProgramStateRef는 프로그램의 상태를 나타내는 객체로, 심볼(Symbol)과 메모리
// 영역(MemRegion)을 관리
#include "clang/StaticAnalyzer/Core/PathSensitive/ProgramState.h"

namespace clang {
namespace ento {

namespace allocation_state {

// 메모리 해제(markReleased) 상태를 ProgramState에 기록하는 함수
ProgramStateRef markReleased(ProgramStateRef State, SymbolRef Sym,
                             const Expr *Origin);

// AF_InnerBuffer 심볼(예: std::vector 내부 버퍼)을 잘못 사용해 발생하는 메모리
// 에러의 원인을 BugReport에 추가로 설명해주는 Visitor를 만들어 반환
std::unique_ptr<BugReporterVisitor> getInnerPointerBRVisitor(SymbolRef Sym);

// Sym이 "컨테이너 객체의 내부 버퍼"를 가리키는 포인터일 때,
// 그 컨테이너 객체의 메모리 Region을 DanglingInternalBufferChecker의 상태
// 맵에서 찾아서 반환
const MemRegion *getContainerObjRegion(ProgramStateRef State, SymbolRef Sym);

} // end namespace allocation_state

} // end namespace ento
} // end namespace clang

#endif

```