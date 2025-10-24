
>이 코드는 Clang Static Analyzer의 **방문 통계**(Analyzer visitation statistics)를 수집하고 보고하는 체크커(Checker)인 `AnalyzerStatsChecker`의 구현입니다.  
>
 분석이 끝나는 시점(`check::EndAnalysis` 콜백)에서 각 함수(또는 블록)의 **제어 흐름 그래프(CFG) 블록**통계를 수집하고, 결과를 리포트로 출력합니다.


```cpp
//==--AnalyzerStatsChecker.cpp - Analyzer visitation statistics --*- C++ -*-==//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
// This file reports various statistics about analyzer visitation.
//===----------------------------------------------------------------------===//
#include "clang/AST/DeclObjC.h"
#include "clang/Basic/SourceManager.h"
#include "clang/StaticAnalyzer/Checkers/BuiltinCheckerRegistration.h"
#include "clang/StaticAnalyzer/Core/BugReporter/BugReporter.h"
#include "clang/StaticAnalyzer/Core/Checker.h"
#include "clang/StaticAnalyzer/Core/CheckerManager.h"
#include "clang/StaticAnalyzer/Core/PathSensitive/ExplodedGraph.h"
#include "clang/StaticAnalyzer/Core/PathSensitive/ExprEngine.h"

#include "llvm/ADT/STLExtras.h"
#include "llvm/ADT/SmallPtrSet.h"
#include "llvm/ADT/SmallString.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/Support/raw_ostream.h"
#include <optional>

using namespace clang;
using namespace ento;

#define DEBUG_TYPE "StatsChecker"

// CFG당 블록 수와 도달하지 못한 블록 수를 통계로 기록합니다.

STATISTIC(NumBlocks,
          "The # of blocks in top level functions");
STATISTIC(NumBlocksUnreachable,
          "The # of unreachable blocks in analyzing top level functions");

namespace {
class AnalyzerStatsChecker : public Checker<check::EndAnalysis> {
public:
  void checkEndAnalysis(ExplodedGraph &G, BugReporter &B,ExprEngine &Eng) const;
};
}

void AnalyzerStatsChecker::checkEndAnalysis(ExplodedGraph &G,
                                            BugReporter &B,
                                            ExprEngine &Eng) const {
  const CFG *C = nullptr;

  const SourceManager &SM = B.getSourceManager();
  llvm::SmallPtrSet<const CFGBlock*, 32> reachable;

  // Root node should have the location context of the top most function.
  const ExplodedNode *GraphRoot = *G.roots_begin();
  const LocationContext *LC = GraphRoot->getLocation().getLocationContext();

  const Decl *D = LC->getDecl();

  // ExplodedNode를 순회하면서 도달 가능한 CFGBlock을 수집합니다.
  // 최상위 서브 그래프의 노드들을 뽑는 과정
  for (const ExplodedNode &N : G.nodes())
  {
    const ProgramPoint &P = N.getLocation();

    // 최상위 함수에 대해서만 확인(계층구조 모두 파악x) (optimization).
    if (D != P.getLocationContext()->getDecl())
      continue;

    if (std::optional<BlockEntrance> BE = P.getAs<BlockEntrance>()) 
    {
      const CFGBlock *CB = BE->getBlock();
      reachable.insert(CB);
    }
  }

  // 최상위 함수의 CFG를 가져옵니다.
  C = LC->getCFG();

  // int 축약형
  unsigned total = 0, unreachable = 0;

  // Find CFGBlocks that were not covered by any node
  for (CFG::const_iterator I = C->begin(); I != C->end(); ++I) 
  {
    const CFGBlock *CB = *I;
    ++total;
    // Check if the block is unreachable
    if (!reachable.count(CB))
    {
      ++unreachable;
    }
  }

  //(Entry, Exit Block은 항상 reachable 취급해야 하므로 통계에서 제외 !)
  unreachable--;
  unreachable--;


  // Generate the warning string
  SmallString<128> buf;
  llvm::raw_svector_ostream output(buf);
  PresumedLoc Loc = SM.getPresumedLoc(D->getLocation());

  if (!Loc.isValid()) return;


  if (isa<FunctionDecl, ObjCMethodDecl>(D)) 
  {
    const NamedDecl *ND = cast<NamedDecl>(D);
    output << *ND;
  } 
  else if (isa<BlockDecl>(D)) 
  {
    output << "block(line:" << Loc.getLine() << ":col:" << Loc.getColumn();
  }

  NumBlocksUnreachable += unreachable;
  NumBlocks += total;
  std::string NameOfRootFunction = std::string(output.str());

  output << " -> Total CFGBlocks: " << total << " | Unreachable CFGBlocks: "
      << unreachable << " | Exhausted Block: "
      << (Eng.wasBlocksExhausted() ? "yes" : "no")
      << " | Empty WorkList: "
      << (Eng.hasEmptyWorkList() ? "yes" : "no");

    // 총 CFG 블록 수와 도달하지 못한 블록 수를 출력합니다.
  B.EmitBasicReport(D, this, "Analyzer Statistics", "Internal Statistics",
                    output.str(), PathDiagnosticLocation(D, SM));

  // Emit warning for each block we bailed out on.
  const CoreEngine &CE = Eng.getCoreEngine();

  // 분석 중단이 발생한 지점에 개별 리포트  
  for (const BlockEdge &BE : make_first_range(CE.exhausted_blocks())) 
  {
    const CFGBlock *Exit = BE.getDst();

    if (Exit->empty())
      continue;

    const CFGElement &CE = Exit->front();
    if (std::optional<CFGStmt> CS = CE.getAs<CFGStmt>())
    {

      SmallString<128> bufI;
      llvm::raw_svector_ostream outputI(bufI);
      outputI << "(" << NameOfRootFunction << ")" <<
                 ": The analyzer generated a sink at this point";
      B.EmitBasicReport(
          D, this, "Sink Point", "Internal Statistics", outputI.str(),
          PathDiagnosticLocation::createBegin(CS->getStmt(), SM, LC));
    }
  }
}

void ento::registerAnalyzerStatsChecker(CheckerManager &mgr) {
  mgr.registerChecker<AnalyzerStatsChecker>();
}

bool ento::shouldRegisterAnalyzerStatsChecker(const CheckerManager &mgr) {
  return true;
}
```