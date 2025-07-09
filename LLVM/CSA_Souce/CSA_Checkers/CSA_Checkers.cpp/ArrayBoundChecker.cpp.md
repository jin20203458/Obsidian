
> **배열 인덱스가 유효 범위를 벗어났는지**(Out-of-bounds, 버퍼 오버플로)를 동적으로 추적하여 리포트하는 체커


```cpp
//== ArrayBoundChecker.cpp ------------------------------*- C++ -*--==//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file defines ArrayBoundChecker, which is a path-sensitive check
// which looks for an out-of-bound array element access.
//
//===----------------------------------------------------------------------===//

#include "clang/StaticAnalyzer/Checkers/BuiltinCheckerRegistration.h"
#include "clang/StaticAnalyzer/Core/BugReporter/BugType.h"
#include "clang/StaticAnalyzer/Core/Checker.h"
#include "clang/StaticAnalyzer/Core/CheckerManager.h"
#include "clang/StaticAnalyzer/Core/PathSensitive/CheckerContext.h"
#include "clang/StaticAnalyzer/Core/PathSensitive/DynamicExtent.h"
#include "clang/StaticAnalyzer/Core/PathSensitive/ExprEngine.h"

using namespace clang;
using namespace ento;

namespace {
class ArrayBoundChecker : public Checker<check::Location> 
{
  const BugType BT{this, "Out-of-bound array access"};

public:
  void checkLocation(SVal l, bool isLoad, const Stmt* S,
                     CheckerContext &C) const;
};
}

void ArrayBoundChecker::checkLocation(SVal l, bool isLoad, const Stmt* LoadS,
                                      CheckerContext &C) const {
  
    // Check for out of bound array element access.

    // LocSVal이 아니면 리턴(값인 경우)
  const MemRegion *R = l.getAsRegion();
  if (!R)
    return;

   // LocSVal이 ElementRegion이 아닌 경우 리턴
  const ElementRegion *ER = dyn_cast<ElementRegion>(R);
  if (!ER)
    return;

  // Get the index of the accessed element.
  DefinedOrUnknownSVal Idx = ER->getIndex().castAs<DefinedOrUnknownSVal>();

  // 0 인덱스는 배열초과가 불가능하기에 리턴
  if (Idx.isZeroConstant())
    return;

  ProgramStateRef state = C.getState();

  // 배열의 크기 획득
  DefinedOrUnknownSVal ElementCount = getDynamicElementCount(
      state, ER->getSuperRegion(), C.getSValBuilder(), ER->getValueType());

  // 정상 범위와 초과 범위 시뮬레이션을 저장
  ProgramStateRef StInBound, StOutBound;
  std::tie(StInBound, StOutBound) = state->assumeInBoundDual(Idx, ElementCount);

  // 초과할 가능성이 있고, 정상 범위가 없는 경우
  if (StOutBound && !StInBound)
  {
      // 초과하는 부분은 탐색 중단
    ExplodedNode *N = C.generateErrorNode(StOutBound);

    if (!N)
      return;

    // FIXME: It would be nice to eventually make this diagnostic more clear,
    // e.g., by referencing the original declaration or by saying *why* this
    // reference is outside the range.

    // 버그리포트 생성
    auto report = std::make_unique<PathSensitiveBugReport>(
        BT, "Access out-of-bound array element (buffer overflow)", N);

    report->addRange(LoadS->getSourceRange());
    C.emitReport(std::move(report));
    return;
  }

  // Array bound check succeeded.  From this point forward the array bound
  // should always succeed.
  C.addTransition(StInBound);
}

void ento::registerArrayBoundChecker(CheckerManager &mgr) {
  mgr.registerChecker<ArrayBoundChecker>();
}

bool ento::shouldRegisterArrayBoundChecker(const CheckerManager &mgr) {
  return true;
}

```