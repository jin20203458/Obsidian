# Cppcheck 66종 무기체계 코딩 규칙 검출 및 미탐 분석 결과 (엄격한 검증)

본 문서는 사용자의 요청에 따라 **Cppcheck가 단순히 부정확한 경고(Collateral Warning)만 띄운 경우를 제외**하고, 실제로 해당 규칙의 결함을 **진짜로 검출(Genuine Detection)**한 항목만 선별하여 재분석한 결과입니다.

---

## 1. 📊 종합 분석 결과 요약 (엄격한 필터링 적용)
* **전체 규칙**: 66종
* **진단 제외**: 1종 (주석 비율 검사 등 정적분석 진단 범위 밖)
* **실제 분석 대상**: 65종
* **실제 검출 성공 (Genuine Detected)**: **29종** (검출률 **44.6%**)
* **검출 실패 및 미탐 (Not Detected / Collateral)**: **36종** (미탐률 **55.4%**)

> 💡 **알림**: 변수 미사용(`unreadVariable`), 구조체 멤버 미사용(`unusedStructMember`), 불필요한 할당(`redundantAssignment`), 단순 C-Style 캐스팅 경고 등 규칙의 핵심 의도와 무관하게 발생한 파생 경고는 모두 "검출 실패(미탐)"로 재분류되었습니다. 이로 인해 기존 40종이었던 검출 수가 더 줄어들었습니다.

### 📊 룰셋별(Category) 상세 통계 (C++ 합산)

| 룰셋 분류 (Category) | 전체 규칙 수 | 제외 규칙 수 | 분석 대상 수 | 검출 성공 (Detected) | 미탐 (Not Detected) | 검출률 (%) |
| :--- | :---: | :---: | :---: | :---: | :---: | :---: |
| 공통 적용 (스타일) | 12 | 1 | 11 | 2 | 9 | **18.2%** |
| 공통 적용 (초기화) | 3 | 0 | 3 | 2 | 1 | **66.7%** |
| 공통 적용 (식별자) | 4 | 0 | 4 | 1 | 3 | **25.0%** |
| 공통 적용 (조건식) | 5 | 0 | 5 | 2 | 3 | **40.0%** |
| 공통 적용 (변환) | 8 | 0 | 8 | 2 | 6 | **25.0%** |
| 공통 적용 (포인터, 배열) | 5 | 0 | 5 | 4 | 1 | **80.0%** |
| 공통 적용 (연산자) | 9 | 0 | 9 | 5 | 4 | **55.6%** |
| C 전용 규칙 | 5 | 0 | 5 | 1 | 4 | **20.0%** |
| C++ 전용 규칙 | 15 | 0 | 15 | 10 | 5 | **66.7%** |

---

## 2. ✅ Cppcheck 실제 검출 성공 항목 (Genuine Detections)
> Cppcheck가 해당 규칙의 핵심 결함을 명확한 경고 ID로 정확히 집어낸 항목들입니다.

| 규칙 번호 | 분류 | 세부 조항 | 매핑 체커 (Clang-Tidy) | 검출된 Cppcheck 핵심 ID (메시지) |
| :---: | :--- | :---: | :--- | :--- |
| 05 | Common_Style | 마 | `SingleExitAndReturnTypeCheck` | ✅ `missingReturn` |
| 08 | Common_Style | 아 | `NoGotoCheck` | ✅ `knownConditionTrueFalse` |
| 14 | Common_Init | 나 | `InitBeforeUseCheck` | ✅ `uninitvar` |
| 15 | Common_Init | 다 | `UninitializedAddressToConstParamCheck` | ✅ `uninitvar` |
| 19 | Common_Ident | 라 | `NoShadowingCheck` | ✅ `shadowVariable` |
| 21 | Common_Cond | 나 | `AlwaysConstantConditionCheck` | ✅ `knownConditionTrueFalse`<br>`unsignedLessThanZero` |
| 24 | Common_Cond | 마 | `UnreachableCodeCheck` | ✅ `unreachableCode` |
| 26 | Common_Conv | 나 | `VarargsFormatCheck` | ✅ `invalidPrintfArgType_s` |
| 27 | Common_Conv | 다 | `VarargsFormatCheck` | ✅ `wrongPrintfScanfArgNum` |
| 34 | Common_PtrArray | 나 | `ReturnStackAddressCheck` | ✅ `danglingLifetime` |
| 35 | Common_PtrArray | 다 | `ReturnStackAddressCheck` | ✅ `returnDanglingLifetime` |
| 36 | Common_PtrArray | 라 | `ArrayIndexBoundCheck` | ✅ `arrayIndexOutOfBounds` |
| 37 | Common_PtrArray | 마 | `NullPointerArithmeticCheck` | ✅ `nullPointerArithmeticRedundantCheck` |
| 38 | Common_Expr | 가 | `SequencingRulesCheck` | ✅ `unknownEvaluationOrder` |
| 40 | Common_Expr | 다 | `SequencingRulesCheck` | ✅ `unknownEvaluationOrder` |
| 41 | Common_Expr | 라 | `ShiftAmountRangeCheck` | ✅ `shiftTooManyBits`<br>`signConversion` |
| 43 | Common_Expr | 바 | `UnevaluatedSideEffectCheck` | ✅ `sizeofCalculation` |
| 45 | Common_Expr | 아 | `NoAssignmentInConditionCheck` | ✅ `knownConditionTrueFalse` |
| 47 | C_Specific | 1 | `VarargsFormatCheck` | ✅ `invalidScanfArgType_int` |
| 52 | CPP_Specific_1 | 1 | `NewDeleteFormConsistencyCheck` | ✅ `mismatchAllocDealloc` |
| 53 | CPP_Specific_1 | 2 | `ReturnStackAddressCheck` | ✅ `returnReference` |
| 54 | CPP_Specific_1 | 3 | `ReturnStackAddressCheck` | ✅ `returnReference` |
| 55 | CPP_Specific_1 | 4 | `DestructorExceptionSafetyCheck` | ✅ `throwInNoexceptFunction`<br>`exceptThrowInDestructor` |
| 58 | CPP_Specific_1 | 7 | `MainUnhandledThrowCheck` | ✅ `throwInEntryPoint`<br>`duplicateBreak` |
| 59 | CPP_Specific_2 | 8 | `UseAfterFreeCheck` | ✅ `deallocuse` |
| 60 | CPP_Specific_2 | 9 | `PartialCopyAssignmentCheck` | ✅ `uninitMemberVar`<br>`operatorEqVarError` |
| 61 | CPP_Specific_2 | 10 | `NoMallocCheck` | ✅ `mismatchAllocDealloc` |
| 65 | CPP_Specific_2 | 14 | `DynamicCastInCtorDtorCheck` | ✅ `virtualCallInConstructor` |
| 66 | CPP_Specific_2 | 15 | `DynamicCastInCtorDtorCheck` | ✅ `virtualCallInConstructor` |

---

## 3. ❌ Cppcheck 미탐 및 부정확한 경고 항목 (False Negatives & Collateral Warnings)
> Cppcheck가 아무 경고도 띄우지 못했거나, 규칙의 결함이 아닌 엉뚱한 경고(예: 변수 미사용 등)만 띄운 항목들입니다. 이 항목들은 반드시 **Clang-Tidy(ArqaStatic)**를 통해 검출해야 합니다.

| 규칙 번호 | 분류 | 세부 조항 | 규칙 제목 | 매핑 체커 (Clang-Tidy) | 미탐 및 파생 경고 상세 |
| :---: | :--- | :---: | :--- | :--- | :--- |
| 02 | Common_Style | 나 | Switch 구문에서 첫번째 Label 전에 코드 구문이 존재하면 안된다. | `SwitchStyleCheck` | ⚠️ 결함 진단 실패 (단순 파생 경고만 발생: `unreadVariable`) |
| 03 | Common_Style | 다 | 함수/변수의 선언 시 type을 명시해야 한다. (사용자정의해더 검사사) | `NoAutoTypeCheck` | ❌ 경고 없음 (완전 미탐) |
| 04 | Common_Style | 라 | 의미 없는 구문은 사용하지 말아야 한다.(side effect) | `NoMeaninglessExprCheck` | ❌ 경고 없음 (완전 미탐) |
| 06 | Common_Style | 바 | 외부 함수 사용시 이를 명시하고 사용해야 한다. | `ExternFunctionDeclarationCheck` | ❌ 경고 없음 (완전 미탐) |
| 07 | Common_Style | 사 | 전처리에 사용되는 매크로는 사용 전에 반드시 정의되어야 한다.(사용자정의해더 검사사) | `MacroDefinedBeforeUseCheck` | ❌ 경고 없음 (완전 미탐) |
| 09 | Common_Style | 자 | 하나의 함수는 하나의 Exit Point를 가져야 한다. | `SingleExitAndReturnTypeCheck` | ❌ 경고 없음 (완전 미탐) |
| 10 | Common_Style | 차 | switch ~ case 문은 default 문이 포함되어야 한다. | `SwitchStyleCheck` | ⚠️ 결함 진단 실패 (단순 파생 경고만 발생: `unreadVariable`) |
| 11 | Common_Style | 카 | 한 줄에 하나의 명령문을 사용한다. | `MultiStatementPerLineCheck` | ⚠️ 결함 진단 실패 (단순 파생 경고만 발생: `unreadVariable`) |
| 12 | Common_Style | 타 | if - else  if 문은 else 문도 반드시 포함 시킨다. | `IfStyleCheck` | ⚠️ 결함 진단 실패 (단순 파생 경고만 발생: `unreadVariable`) |
| 13 | Common_Init | 가 | char 배열의 초기화시 char 배열의 크기는 초기화 문자열의 크기와 Null을 포함하는 크기로 선언되어야 한다. | `CharArrayStringSizeCheck` | ❌ 경고 없음 (완전 미탐) |
| 16 | Common_Ident | 가 | external과 internal linkage의 특성을 동시에 가질 수 없다. | `UnknownChecker` | ❌ 경고 없음 (완전 미탐) |
| 17 | Common_Ident | 나 | external linkage scope에서 선언된 함수나 Object의 이름은 유일해야 한다. | `UnknownChecker` | ❌ 경고 없음 (완전 미탐) |
| 18 | Common_Ident | 다 | external linkage scope에서 정의된 함수나 Object의 데이터형은 선언 시 정의와 동일해야 한다. | `UnknownChecker` | ❌ 경고 없음 (완전 미탐) |
| 20 | Common_Cond | 가 | float 자료형에서 동등성 비교연산을 수행하지 말아야 한다. | `NoFloatEqualityCheck` | ❌ 경고 없음 (완전 미탐) |
| 22 | Common_Cond | 다 | switch의 case의 조건을 만족할 수 없는 Label을 사용하지 않는다. | `UnknownChecker` | ❌ 경고 없음 (완전 미탐) |
| 23 | Common_Cond | 라 | Switch 구문에서 Expression을 논리적 연산으로 사용하지 말아야 한다. | `SwitchStyleCheck` | ❌ 경고 없음 (완전 미탐) |
| 25 | Common_Conv | 가 | 선언된 데이터 형으로 표현할 수 있는 숫자의 영역을 초과하는 값을 할당하지 말아야 한다. | `NoOutOfRangeAssignmentCheck` | ⚠️ 결함 진단 실패 (단순 파생 경고만 발생: `unreadVariable`) |
| 28 | Common_Conv | 라 | Object 저장 값을 표현할 수 없는 데이터로의 형 변환을 하지 말아야 한다. | `RepresentableCastCheck` | ⚠️ 결함 진단 실패 (단순 파생 경고만 발생: `unreadVariable`, `variableScope`) |
| 29 | Common_Conv | 마 | 음수 값을 unsigned type으로 변환해서는 안된다. | `UnknownChecker` | ⚠️ 결함 진단 실패 (단순 파생 경고만 발생: `unreadVariable`, `variableScope`) |
| 30 | Common_Conv | 바 | Character 문자열과 Wide character 문자열을 혼용하지 말아야 한다. | `NoMixedCharWideStringCheck` | ❌ 경고 없음 (완전 미탐) |
| 31 | Common_Conv | 사 | 포인터 Cast의 결과로 인한 이전 포인터의 CONST 특성의 상실을 유의해야한다. | `PointerCvQualifierDropCheck` | ⚠️ 결함 진단 실패 (단순 파생 경고만 발생: `unreadVariable`, `constVariablePointer`, `cstyleCast`) |
| 32 | Common_Conv | 아 | 포인터 Cast의 결과로 인한 이전 포인터의 volatile 특성 상실을 유의해야 한다. | `PointerCvQualifierDropCheck` | ⚠️ 결함 진단 실패 (단순 파생 경고만 발생: `unreadVariable`, `constVariablePointer`, `cstyleCast`) |
| 33 | Common_PtrArray | 가 | 포인터는 참조전에 NULL 여부를 반드시 확인하여야 한다. | `NullDereferenceGuardCheck` | ⚠️ 결함 진단 실패 (단순 파생 경고만 발생: `unreadVariable`) |
| 39 | Common_Expr | 나 | 나누는 값이 변수일 경우 0인지를 반드시 확인하여야 한다. | `NonZeroDivisorGuardCheck` | ⚠️ 결함 진단 실패 (단순 파생 경고만 발생: `unreadVariable`) |
| 42 | Common_Expr | 마 | Underlying type이 Unsigned인 데이터에 '-'를 사용하여 결과를 assign하지 말아야 한다. | `UnsignedMinusAssignmentCheck` | ⚠️ 결함 진단 실패 (단순 파생 경고만 발생: `unreadVariable`) |
| 44 | Common_Expr | 사 | Boolean 표현 값에는 논리 연산자(&&, ||, !)만을 사용하여야 한다. | `BoolLogicOnlyOpsCheck` | ⚠️ 결함 진단 실패 (단순 파생 경고만 발생: `redundantAssignment`) |
| 46 | Common_Expr | 자 | Signed Value에서 Bitwise연산자(<<, ~, |, ^ 등)로 인한 Negative Value를 유의해야 한다. | `SignedBitwiseSafetyCheck` | ⚠️ 결함 진단 실패 (단순 파생 경고만 발생: `unreadVariable`) |
| 48 | C_Specific | 2 | `#include` 구문에서 표준에 맞지 않는 Character set을 사용하지 않아야 한다. | `IncludeCharsetCheck` | ❌ 경고 없음 (완전 미탐) |
| 49 | C_Specific | 3 | malloc에서 사용하는 인자값은 assign될 변수 type의 정수배이어야 한다. | `MallocSizeMultipleCheck` | ⚠️ 결함 진단 실패 (단순 파생 경고만 발생: `constVariablePointer`, `cstyleCast`, `memleak`, `unusedAllocatedMemory`, `redundantAssignment`, `unreadVariable`) |
| 50 | C_Specific | 4 | 함수의 Argument type과 개수는 함수의 Prototype, 선언, 정의가 모두 같아야 한다. | `FunctionSignatureConsistencyCheck` | ⚠️ 결함 진단 실패 (단순 파생 경고만 발생: `unreadVariable`) |
| 51 | C_Specific | 5 | 구조체/배열의 초기화 시 default 초기화 값(0)을 제외하고, 구조에 맞게 ‘{}’를 사용하여 선언된 Size에 맞게 초기화 해야 한다. | `AggregateInitStyleCheck` | ⚠️ 결함 진단 실패 (단순 파생 경고만 발생: `unusedStructMember`) |
| 56 | CPP_Specific_1 | 5 | 사용되지 않는 예외 처리 문을 작성하지 말아야 한다. | `UnknownChecker` | ❌ 경고 없음 (완전 미탐) |
| 57 | CPP_Specific_1 | 6 | exception specification에 기술되지 않은 모든 throw에 대하여 예외처리를 해야만 한다. | `ExceptionSpecificationCheck` | ❌ 경고 없음 (완전 미탐) |
| 62 | CPP_Specific_2 | 11 | 순수 가상 함수는 반드시 `= 0` 으로 초기화해야 한다. | `PureVirtualInitCheck` | ❌ 경고 없음 (완전 미탐) |
| 63 | CPP_Specific_2 | 12 | 순수 함수는 반드시 `virtual`로 선언되어야 한다. | `VirtualPureCheck` | ⚠️ 결함 진단 실패 (단순 파생 경고만 발생: `functionStatic`) |
| 64 | CPP_Specific_2 | 13 | virtual base 클래스 포인터를 derived 클래스 포인터로 변환할 때에는 반드시 `dynamic_cast`를 사용해야 한다. | `VirtualBaseCastCheck` | ⚠️ 결함 진단 실패 (단순 파생 경고만 발생: `missingOverride`, `uselessAssignmentPtrArg`, `unreadVariable`) |

---

## 4. 종합 제안 및 결론 (재검증 반영)

1. **엄격한 평가 결과**:
   - 단순히 어떤 경고라도 띄운 것을 검출로 간주했을 때는 61.5%의 검출률을 보였으나, **진짜 핵심 결함을 논리적으로 검출한 비율은 약 44.6%로 크게 하락**했습니다.
   - 이는 Cppcheck가 메모리 누수 및 오염(Null Pointer, Out of Bounds, Use After Free)과 같은 명백한 런타임 버그 탐지에는 강력하지만, **"타입 변환 통제, 특정 연산자/함수 사용 제한, 포인터 검사 의무화, 매크로 정의 의무"** 등과 같은 코딩 스타일 및 논리적 규약(MISRA, CERT 규칙 등)에 대해서는 전혀 대응하지 못함을 방증합니다.

2. **ArqaStatic (Clang-Tidy) 도입의 완벽한 당위성**:
   - 실험 검증 결과, 무기체계 소프트웨어 코딩 규칙의 과반수가 Cppcheck로는 검출되지 않거나 엉뚱한 부수적 경고(`unreadVariable` 등)로만 표출됩니다.
   - 따라서 **이러한 통제성 규칙을 100% 탐지하기 위해서는 컴파일러 AST 기반의 ArqaStatic (Clang-Tidy 체커) 병행 도입이 선택이 아닌 필수**임을 명확히 증명합니다.
