```cpp
//===--- SValVisitor.h - Visitor for SVal subclasses ------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// 이 파일은 SValVisitor, SymExprVisitor, 그리고 MemRegionVisitor를 정의합니다
// 인터페이스와 FullSValVisitor는 세 가지 계층을 모두 방문합니다.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_CLANG_STATICANALYZER_CORE_PATHSENSITIVE_SVALVISITOR_H
#define LLVM_CLANG_STATICANALYZER_CORE_PATHSENSITIVE_SVALVISITOR_H

/// 메모리의 "구역"들을 표현하는 추상 모델
#include "clang/StaticAnalyzer/Core/PathSensitive/MemRegion.h"

/// 프로그램 분석 중 등장하는 값(value)들을 모델링
#include "clang/StaticAnalyzer/Core/PathSensitive/SVals.h"

/// Symbolic Execution 중 추상화된 값(심볼)을 표현하고 관리
#include "clang/StaticAnalyzer/Core/PathSensitive/SymbolManager.h"

namespace clang {

namespace ento {

/// SValVisitor - 이 클래스는 SVal을 위한 간단한 방문자를 구현합니다
/// 하위 클래스.
template <typename ImplClass, typename RetTy = void> class SValVisitor {
  ImplClass &derived() { return *static_cast<ImplClass *>(this); }

public:
  RetTy Visit(SVal V)  // 심볼릭 객체 V 가 들어오면 각기 다른 VisitXXX 함수를 호출해서 결과 반환
  {
    // Dispatch to VisitFooVal for each FooVal.
    switch (V.getKind()) {
#define BASIC_SVAL(Id, Parent)                                                 \
  case SVal::Id##Kind:                                                         \
    return derived().Visit##Id(V.castAs<Id>());
#define LOC_SVAL(Id, Parent)                                                   \
  case SVal::Loc##Id##Kind:                                                    \
    return derived().Visit##Id(V.castAs<loc::Id>());
#define NONLOC_SVAL(Id, Parent)                                                \
  case SVal::NonLoc##Id##Kind:                                                 \
    return derived().Visit##Id(V.castAs<nonloc::Id>());
#include "clang/StaticAnalyzer/Core/PathSensitive/SVals.def"
    }
    llvm_unreachable("Unknown SVal kind!");
  }

  // Dispatch to the more generic handler as a default implementation.
#define BASIC_SVAL(Id, Parent)                                                 \
  RetTy Visit##Id(Id V) { return derived().Visit##Parent(V.castAs<Id>()); }
#define ABSTRACT_SVAL(Id, Parent) BASIC_SVAL(Id, Parent)
#define LOC_SVAL(Id, Parent)                                                   \
  RetTy Visit##Id(loc::Id V) { return derived().VisitLoc(V.castAs<Loc>()); }
#define NONLOC_SVAL(Id, Parent)                                                \
  RetTy Visit##Id(nonloc::Id V) {                                              \
    return derived().VisitNonLoc(V.castAs<NonLoc>());                          \
  }
#include "clang/StaticAnalyzer/Core/PathSensitive/SVals.def"

  // Base case, ignore it. :)
  RetTy VisitSVal(SVal V) { return RetTy(); }
};

/// SymExprVisitor - this class implements a simple visitor for SymExpr
/// subclasses.
template <typename ImplClass, typename RetTy = void> class SymExprVisitor {
public:

#define DISPATCH(CLASS) \
    return static_cast<ImplClass *>(this)->Visit ## CLASS(cast<CLASS>(S))

  RetTy Visit(SymbolRef S) {
    // Dispatch to VisitSymbolFoo for each SymbolFoo.
    switch (S->getKind()) {
#define SYMBOL(Id, Parent) \
    case SymExpr::Id ## Kind: DISPATCH(Id);
#include "clang/StaticAnalyzer/Core/PathSensitive/Symbols.def"
    }
    llvm_unreachable("Unknown SymExpr kind!");
  }

  // If the implementation chooses not to implement a certain visit method, fall
  // back on visiting the superclass.
#define SYMBOL(Id, Parent) RetTy Visit ## Id(const Id *S) { DISPATCH(Parent); }
#define ABSTRACT_SYMBOL(Id, Parent) SYMBOL(Id, Parent)
#include "clang/StaticAnalyzer/Core/PathSensitive/Symbols.def"

  // Base case, ignore it. :)
  RetTy VisitSymExpr(SymbolRef S) { return RetTy(); }

#undef DISPATCH
};

/// MemRegionVisitor - this class implements a simple visitor for MemRegion
/// subclasses.
template <typename ImplClass, typename RetTy = void> class MemRegionVisitor {
public:

#define DISPATCH(CLASS) \
  return static_cast<ImplClass *>(this)->Visit ## CLASS(cast<CLASS>(R))

  RetTy Visit(const MemRegion *R) {
    // Dispatch to VisitFooRegion for each FooRegion.
    switch (R->getKind()) {
#define REGION(Id, Parent) case MemRegion::Id ## Kind: DISPATCH(Id);
#include "clang/StaticAnalyzer/Core/PathSensitive/Regions.def"
    }
    llvm_unreachable("Unknown MemRegion kind!");
  }

  // If the implementation chooses not to implement a certain visit method, fall
  // back on visiting the superclass.
#define REGION(Id, Parent) \
  RetTy Visit ## Id(const Id *R) { DISPATCH(Parent); }
#define ABSTRACT_REGION(Id, Parent) \
  REGION(Id, Parent)
#include "clang/StaticAnalyzer/Core/PathSensitive/Regions.def"

  // Base case, ignore it. :)
  RetTy VisitMemRegion(const MemRegion *R) { return RetTy(); }

#undef DISPATCH
};

/// FullSValVisitor - a convenient mixed visitor for all three:
/// SVal, SymExpr and MemRegion subclasses.
template <typename ImplClass, typename RetTy = void>
class FullSValVisitor : public SValVisitor<ImplClass, RetTy>,
                        public SymExprVisitor<ImplClass, RetTy>,
                        public MemRegionVisitor<ImplClass, RetTy> {
public:
  using SValVisitor<ImplClass, RetTy>::Visit;
  using SymExprVisitor<ImplClass, RetTy>::Visit;
  using MemRegionVisitor<ImplClass, RetTy>::Visit;
};

} // end namespace ento

} // end namespace clang

#endif

```


X-Macro기법으로 컴파일 이전에 

```cpp
switch (V.getKind()) {
  case SVal::UnknownValKind:
    return derived().VisitUnknownVal(V.castAs<UnknownVal>());
  case SVal::UndefinedValKind:
    return derived().VisitUndefinedVal(V.castAs<UndefinedVal>());
  case SVal::LocMemRegionValKind:
    return derived().VisitMemRegionVal(V.castAs<loc::MemRegionVal>());
  case SVal::NonLocConcreteIntKind:
    return derived().VisitConcreteInt(V.castAs<nonloc::ConcreteInt>());
  // ... 이하 생략 ...
}
```
이러한 코드가 만들어진다.



ex.1)
```cpp
  RetTy Visit(SVal V)  // 심볼릭 객체 V 가 들어오면 각기 다른 VisitXXX 함수를 호출해서 결과 반환
  {
    // Dispatch to VisitFooVal for each FooVal.
    switch (V.getKind()) {
#define BASIC_SVAL(Id, Parent)                                                 \
  case SVal::Id##Kind:                                                         \
    return derived().Visit##Id(V.castAs<Id>());
#define LOC_SVAL(Id, Parent)                                                   \
  case SVal::Loc##Id##Kind:                                                    \
    return derived().Visit##Id(V.castAs<loc::Id>());
#define NONLOC_SVAL(Id, Parent)                                                \
  case SVal::NonLoc##Id##Kind:                                                 \
    return derived().Visit##Id(V.castAs<nonloc::Id>());
#include "clang/StaticAnalyzer/Core/PathSensitive/SVals.def"
    }
    llvm_unreachable("Unknown SVal kind!");
  }
```

ex)
```cpp
  RetTy Visit(SVal V)  // 심볼릭 객체 V 가 들어오면 각기 다른 VisitXXX 함수를 호출해서 결과 반환
  {
    // Dispatch to VisitFooVal for each FooVal.
    switch (V.getKind()) {
#define BASIC_SVAL(Id, Parent)                                                 \
  case SVal::Id##Kind:                                                         \
    return derived().Visit##Id(V.castAs<Id>());
#define LOC_SVAL(Id, Parent)                                                   \
  case SVal::Loc##Id##Kind:                                                    \
    return derived().Visit##Id(V.castAs<loc::Id>());
#define NONLOC_SVAL(Id, Parent)                                                \
  case SVal::NonLoc##Id##Kind:                                                 \
    return derived().Visit##Id(V.castAs<nonloc::Id>());
//===-- SVals.def - Metadata about SVal kinds -------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// The list of symbolic values (SVal kinds) used in the Static Analyzer.
// The distinction between `loc::` and `nonloc::` SVal namespaces is
// currently hardcoded, because it is too peculiar and explicit to be handled
// uniformly. In order to use this information, users of this file must define
// one or more of the following macros:
//
// BASIC_SVAL(Id, Parent) - for specific SVal kinds, which are
// neither in `loc::` nor in `nonloc::` namespace.
//
// ABSTRACT_SVAL(Id, Parent) - for abstract SVal classes which are
// neither in `loc::` nor in `nonloc::` namespace,
//
// LOC_SVAL(Id, Parent) - for values in `loc::` namespace.
//
// NONLOC_SVAL(Id, Parent) - for values in `nonloc::` namespace.
//
// SVAL_RANGE(Id, First, Last) - for defining range of subtypes of
// the abstract class `Id`.
//
//===----------------------------------------------------------------------===//

#ifndef BASIC_SVAL
#define BASIC_SVAL(Id, Parent)
#endif

#ifndef ABSTRACT_SVAL
#define ABSTRACT_SVAL(Id, Parent)
#endif

#ifndef LOC_SVAL
#define LOC_SVAL(Id, Parent)
#endif

#ifndef NONLOC_SVAL
#define NONLOC_SVAL(Id, Parent)
#endif

#ifndef SVAL_RANGE
#define SVAL_RANGE(Id, First, Last)
#endif

BASIC_SVAL(UndefinedVal, SVal)
ABSTRACT_SVAL(DefinedOrUnknownSVal, SVal)
  BASIC_SVAL(UnknownVal, DefinedOrUnknownSVal)
  ABSTRACT_SVAL(DefinedSVal, DefinedOrUnknownSVal)
    ABSTRACT_SVAL(Loc, DefinedSVal)
      LOC_SVAL(ConcreteInt, Loc)
      LOC_SVAL(GotoLabel, Loc)
      LOC_SVAL(MemRegionVal, Loc)
      SVAL_RANGE(Loc, ConcreteInt, MemRegionVal)
    ABSTRACT_SVAL(NonLoc, DefinedSVal)
      NONLOC_SVAL(CompoundVal, NonLoc)
      NONLOC_SVAL(ConcreteInt, NonLoc)
      NONLOC_SVAL(LazyCompoundVal, NonLoc)
      NONLOC_SVAL(LocAsInteger, NonLoc)
      NONLOC_SVAL(SymbolVal, NonLoc)
      NONLOC_SVAL(PointerToMember, NonLoc)
      SVAL_RANGE(NonLoc, CompoundVal, PointerToMember)

#undef SVAL_RANGE
#undef NONLOC_SVAL
#undef LOC_SVAL
#undef ABSTRACT_SVAL
#undef BASIC_SVAL
    }
    llvm_unreachable("Unknown SVal kind!");
  }

```


```cpp
RetTy Visit(SVal V) {
  switch (V.getKind()) {

    // BASIC_SVAL(UndefinedVal, SVal)
    case SVal::UndefinedValKind:
      return derived().VisitUndefinedVal(V.castAs<UndefinedVal>());

    // ABSTRACT_SVAL(DefinedOrUnknownSVal, SVal)
    // (아무것도 확장 안 됨! 추상타입이므로.)

      // BASIC_SVAL(UnknownVal, DefinedOrUnknownSVal)
      case SVal::UnknownValKind:
        return derived().VisitUnknownVal(V.castAs<UnknownVal>());

      // ABSTRACT_SVAL(DefinedSVal, DefinedOrUnknownSVal)
      // (아무것도 확장 안 됨)

        // ABSTRACT_SVAL(Loc, DefinedSVal)
        // (아무것도 확장 안 됨)

          // LOC_SVAL(ConcreteInt, Loc)
          case SVal::LocConcreteIntKind:
            return derived().VisitConcreteInt(V.castAs<loc::ConcreteInt>());
          // LOC_SVAL(GotoLabel, Loc)
          case SVal::LocGotoLabelKind:
            return derived().VisitGotoLabel(V.castAs<loc::GotoLabel>());
          // LOC_SVAL(MemRegionVal, Loc)
          case SVal::LocMemRegionValKind:
            return derived().VisitMemRegionVal(V.castAs<loc::MemRegionVal>());

        // ABSTRACT_SVAL(NonLoc, DefinedSVal)
        // (아무것도 확장 안 됨)

          // NONLOC_SVAL(CompoundVal, NonLoc)
          case SVal::NonLocCompoundValKind:
            return derived().VisitCompoundVal(V.castAs<nonloc::CompoundVal>());
          // NONLOC_SVAL(ConcreteInt, NonLoc)
          case SVal::NonLocConcreteIntKind:
            return derived().VisitConcreteInt(V.castAs<nonloc::ConcreteInt>());
          // NONLOC_SVAL(LazyCompoundVal, NonLoc)
          case SVal::NonLocLazyCompoundValKind:
            return derived().VisitLazyCompoundVal(V.castAs<nonloc::LazyCompoundVal>());
          // NONLOC_SVAL(LocAsInteger, NonLoc)
          case SVal::NonLocLocAsIntegerKind:
            return derived().VisitLocAsInteger(V.castAs<nonloc::LocAsInteger>());
          // NONLOC_SVAL(SymbolVal, NonLoc)
          case SVal::NonLocSymbolValKind:
            return derived().VisitSymbolVal(V.castAs<nonloc::SymbolVal>());
          // NONLOC_SVAL(PointerToMember, NonLoc)
          case SVal::NonLocPointerToMemberKind:
            return derived().VisitPointerToMember(V.castAs<nonloc::PointerToMember>());
  }
  llvm_unreachable("Unknown SVal kind!");
}

```