


```cpp
namespace clang::ento::tagged_union_modeling {

// 아래 함수들의 실제 구현은 이 파일과 같은 디렉토리에 있는 StdVariantChecker.cpp에서 확인할 수 있습니다.

bool isCopyConstructorCall(const CallEvent &Call);      // 복사 생성자 호출인지 판별
bool isCopyAssignmentCall(const CallEvent &Call);       // 복사 대입 연산자인지 판별
bool isMoveAssignmentCall(const CallEvent &Call);       // 이동 대입 연산자인지 판별
bool isMoveConstructorCall(const CallEvent &Call);      // 이동 생성자 호출인지 판별
bool isStdType(const Type *Type, const std::string &TypeName); // 표준 타입인지 확인
bool isStdVariant(const Type *Type);                    // std::variant 타입인지 확인

// 메모리 영역(region)을 무효화할 때, 프로그램 상태에 저장된 해당 커스텀 데이터도 함께 무효화해야 합니다.
template <class TypeMap>
ProgramStateRef
removeInformationStoredForDeadInstances(const CallEvent &Call,
                                        ProgramStateRef State,
                                        ArrayRef<const MemRegion *> Regions) {
  // 호출 정보를 알 수 없으면 더 이상 진행하지 않습니다.
  // 만약 호출이 시스템 헤더 안에서 발생했다면, 이는 구현 세부 사항이므로 분석 대상에서 제외합니다.
  if (Call.isInSystemHeader())
    return State;

  for (const MemRegion *Region : Regions)
    State = State->remove<TypeMap>(Region);

  return State;
}

// 생성자 및 대입 연산자를 처리할 때 사용하는 함수입니다.
template <class TypeMap>
void handleConstructorAndAssignment(const CallEvent &Call, CheckerContext &C,
                                    SVal ThisSVal) {
  ProgramStateRef State = Call.getState();

  if (!State)
    return;

  auto ArgSVal = Call.getArgSVal(0);
  const auto *ThisRegion = ThisSVal.getAsRegion();
  const auto *ArgMemRegion = ArgSVal.getAsRegion();

  // 생성자/대입 연산자의 종류에 따라 상태를 변경합니다.
  bool IsCopy = isCopyConstructorCall(Call) || isCopyAssignmentCall(Call);
  bool IsMove = isMoveConstructorCall(Call) || isMoveAssignmentCall(Call);
  // 먼저 복사/이동 연산을 처리합니다.
  if (IsCopy || IsMove) {
    const QualType *OtherQType = State->get<TypeMap>(ArgMemRegion);

    // 복사 생성자/대입 연산자의 인자가 알 수 없는 경우,
    // 복사 대상 객체의 타입 정보도 알 수 없습니다.
    if (!OtherQType) {
      State = State->remove<TypeMap>(ThisRegion);
    } else {
      // 이동 연산의 경우, 이동된 객체는 파괴만 가능하며
      // 그 외의 사용은 정의되지 않은 동작입니다.
      if (IsMove)
        State = State->remove<TypeMap>(ArgMemRegion);

      State = State->set<TypeMap>(ThisRegion, *OtherQType);
    }
  } else {
    // 값 생성자(value constructor) 처리
    auto ArgQType = ArgSVal.getType(C.getASTContext());
    const Type *ArgTypePtr = ArgQType.getTypePtr();

    QualType WoPointer = ArgTypePtr->getPointeeType();
    State = State->set<TypeMap>(ThisRegion, WoPointer);
  }

  C.addTransition(State);
}

} // namespace clang::ento::tagged_union_modeling
```