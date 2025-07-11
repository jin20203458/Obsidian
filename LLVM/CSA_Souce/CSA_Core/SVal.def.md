```
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
```




