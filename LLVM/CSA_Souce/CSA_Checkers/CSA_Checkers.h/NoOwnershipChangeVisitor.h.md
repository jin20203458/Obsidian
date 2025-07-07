
>**메모리 자원 소유권의 변경이 발생하지 않은 버그**를 감지하고 보고하는 데 사용
- 어떤 함수가 특정 자원의 소유권을 변경했어야 함에도 불구하고 그렇게 하지 않았을 때 이를 찾아내는 역할


```cpp
//===--------------------------------------------------------------*- C++ -*--//
//
// LLVM 프로젝트의 일부로, Apache License v2.0 및 LLVM 예외 조항에 따라 배포됩니다.
// 라이선스 정보는 https://llvm.org/LICENSE.txt 를 참고하세요.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
namespace clang {
namespace ento {

class NoOwnershipChangeVisitor : public NoStateChangeFuncVisitor {
protected:
  // 우리가 소유권 변화(또는 변화 없음)를 추적하고자 하는 심볼입니다.
  SymbolRef Sym;
  const CheckerBase &Checker;

  // 함수 진입 노드에서 함수 이름을 추출하는 유틸리티 함수입니다.
  LLVM_DUMP_METHOD static std::string
  getFunctionName(const ExplodedNode *CallEnterN);

  /// 이 함수가 리소스 해제를 의도했는지(예: free, close 등) 문법적으로 추정합니다.
  /// 경로별로 실제 실행 정보를 알 수 없기 때문에, 코드 구조만 보고 판단합니다.
  virtual bool doesFnIntendToHandleOwnership(const Decl *Callee,
                                             ASTContext &ACtx) = 0;

  // 함수 호출 전후로 리소스 상태(소유권 등)가 바뀌었는지 확인하는 함수입니다.
  virtual bool hasResourceStateChanged(ProgramStateRef CallEnterState,
                                       ProgramStateRef CallExitEndState) = 0;

  // 함수 내에서 리소스가 변경되었는지 최종적으로 판단하는 래퍼 함수입니다.
  bool wasModifiedInFunction(const ExplodedNode *CallEnterN,
                             const ExplodedNode *CallExitEndN) final;

  // 분석 결과에 추가할 설명(노트)을 생성하는 함수입니다.
  virtual PathDiagnosticPieceRef emitNote(const ExplodedNode *N) = 0;

  // Objective-C의 self 포인터 관련 노트(미구현, 추후 구현 예정)
  PathDiagnosticPieceRef maybeEmitNoteForObjCSelf(PathSensitiveBugReport &R,
                                                  const ObjCMethodCall &Call,
                                                  const ExplodedNode *N) final {
    // TODO: 구현 예정.
    return nullptr;
  }

  // C++의 this 포인터 관련 노트(미구현, 추후 구현 예정)
  PathDiagnosticPieceRef maybeEmitNoteForCXXThis(PathSensitiveBugReport &R,
                                                 const CXXConstructorCall &Call,
                                                 const ExplodedNode *N) final {
    // TODO: 구현 예정.
    return nullptr;
  }

  // 이 함수는 final로 선언되어 있고, 실질적으로 emitNote로 분기(dispatch)됩니다.
  PathDiagnosticPieceRef
  maybeEmitNoteForParameters(PathSensitiveBugReport &R, const CallEvent &Call,
                             const ExplodedNode *N) final;

public:
  using OwnerSet = llvm::SmallPtrSet<const MemRegion *, 8>;

private:
  // 특정 노드에서 리소스 소유자 집합을 추출하는 함수입니다.
  OwnerSet getOwnersAtNode(const ExplodedNode *N);

public:
  // 생성자: 추적할 심볼과 체크러를 받아 초기화합니다.
  NoOwnershipChangeVisitor(SymbolRef Sym, const CheckerBase *Checker)
      : NoStateChangeFuncVisitor(bugreporter::TrackingKind::Thorough), Sym(Sym),
        Checker(*Checker) {}

  // FoldingSet에 넣을 때 고유 식별자를 만드는 함수입니다.
  void Profile(llvm::FoldingSetNodeID &ID) const override {
    static int Tag = 0;
    ID.AddPointer(&Tag);
    ID.AddPointer(Sym);
  }
};

} // namespace ento
} // namespace clang
```