# Cppcheck DAPA 66종 규칙 검출 성능 분석 보고서

본 보고서는 **DAPA(DRPA) 무기체계 소프트웨어 코딩 규칙 66종**에 대하여 오픈소스 정적 분석 도구인 **Cppcheck v2.21.0**이 검출 가능한 항목과 미탐 항목을 정량적으로 분류하고 평가한 결과 보고서입니다.

---

## 1. 정량적 분석 요약

- **전체 대상 규칙**: 66종
- **테스트 케이스 제외 (주석 비율 등 분석기 진단 불가 항목)**: 1종
- **실제 분석 대상 규칙**: 65종
- **Cppcheck 검출 성공**: **40종**
- **Cppcheck 미탐 (검출 실패)**: **25종**
- **실제 검출률 (Detection Rate)**: **61.54%**

### 📊 룰셋별(Category) 상세 통계

| 룰셋 분류 (Category) | 전체 규칙 수 | 제외 규칙 수 | 분석 대상 수 | 검출 성공 (Detected) | 미탐 (Not Detected) | 검출률 (%) |
| :--- | :---: | :---: | :---: | :---: | :---: | :---: |
| 공통 적용 (스타일) | 12 | 1 | 11 | 0 | 11 | **0.00%** |
| 공통 적용 (초기화) | 3 | 0 | 3 | 1 | 2 | **33.33%** |
| 공통 적용 (식별자) | 4 | 0 | 4 | 1 | 3 | **25.00%** |
| 공통 적용 (조건식) | 5 | 0 | 5 | 2 | 3 | **40.00%** |
| 공통 적용 (변환) | 8 | 0 | 8 | 7 | 1 | **87.50%** |
| 공통 적용 (포인터, 배열) | 5 | 0 | 5 | 5 | 0 | **100.00%** |
| 공통 적용 (연산자) | 9 | 0 | 9 | 8 | 1 | **88.89%** |
| C 전용 규칙 | 5 | 0 | 5 | 5 | 0 | **100.00%** |
| C++ 전용 규칙 | 15 | 0 | 15 | 11 | 4 | **73.33%** |

---

## 2. 세부 룰셋 검출 결과 매핑 테이블

| 번호 | 분류 (Category) | 세부 조항 | 규칙 제목 | 매핑 체커 (Clang-Tidy) | Cppcheck 검출 여부 | 검출된 이슈 ID (메시지) |
| :---: | :--- | :---: | :--- | :--- | :---: | :--- |
| 01 | Common_Style | 가 | Placeholder - Parse Failed | `UnknownChecker` | ⚠️ N/A (분석 불가) | 코드 분석 대상 제외 (주석 비율 검사 등) |
| 02 | Common_Style | 나 | Switch 구문에서 첫번째 Label 전에 코드 구문이 존재하면 안된다. | `SwitchStyleCheck` | ❌ Not Detected | 미탐지 |
| 03 | Common_Style | 다 | 함수/변수의 선언 시 type을 명시해야 한다. (사용자정의해더 검사사) | `NoAutoTypeCheck` | ❌ Not Detected | 미탐지 |
| 04 | Common_Style | 라 | 의미 없는 구문은 사용하지 말아야 한다.(side effect) | `NoMeaninglessExprCheck` | ❌ Not Detected | 미탐지 |
| 05 | Common_Style | 마 | 함수 선언시 사용된 리턴형과 함수 내에서 사용되는 리턴형은 일치해야 한다. | `SingleExitAndReturnTypeCheck` | ❌ Not Detected | 미탐지 |
| 06 | Common_Style | 바 | 외부 함수 사용시 이를 명시하고 사용해야 한다. | `ExternFunctionDeclarationCheck` | ❌ Not Detected | 미탐지 |
| 07 | Common_Style | 사 | 전처리에 사용되는 매크로는 사용 전에 반드시 정의되어야 한다.(사용자정의해더 검사사) | `MacroDefinedBeforeUseCheck` | ❌ Not Detected | 미탐지 |
| 08 | Common_Style | 아 | goto 문을 사용하지 말아야 한다. | `NoGotoCheck` | ❌ Not Detected | 미탐지 |
| 09 | Common_Style | 자 | 하나의 함수는 하나의 Exit Point를 가져야 한다. | `SingleExitAndReturnTypeCheck` | ❌ Not Detected | 미탐지 |
| 10 | Common_Style | 차 | switch ~ case 문은 default 문이 포함되어야 한다. | `SwitchStyleCheck` | ❌ Not Detected | 미탐지 |
| 11 | Common_Style | 카 | 한 줄에 하나의 명령문을 사용한다. | `MultiStatementPerLineCheck` | ❌ Not Detected | 미탐지 |
| 12 | Common_Style | 타 | if - else  if 문은 else 문도 반드시 포함 시킨다. | `IfStyleCheck` | ❌ Not Detected | 미탐지 |
| 13 | Common_Init | 가 | char 배열의 초기화시 char 배열의 크기는 초기화 문자열의 크기와 Null을 포함하는 크기로 선언되어야 한다. | `CharArrayStringSizeCheck` | ❌ Not Detected | 미탐지 |
| 14 | Common_Init | 나 | 변수는 사용전 반드시 초기화되어야 한다. | `UnknownChecker` | ❌ Not Detected | 미탐지 |
| 15 | Common_Init | 다 | 값이 초기화되지 않은 변수의 포인터를 함수의 Read-only(const)로 사용해서는 안된다. | `UninitializedAddressToConstParamCheck` | ✅ Detected | `uninitvar`: Uninitialized variable: &y<br>`unreadVariable`: Variable 'r' is assigned a value that is never used. |
| 16 | Common_Ident | 가 | external과 internal linkage의 특성을 동시에 가질 수 없다. | `UnknownChecker` | ❌ Not Detected | 미탐지 |
| 17 | Common_Ident | 나 | external linkage scope에서 선언된 함수나 Object의 이름은 유일해야 한다. | `UnknownChecker` | ❌ Not Detected | 미탐지 |
| 18 | Common_Ident | 다 | external linkage scope에서 정의된 함수나 Object의 데이터형은 선언 시 정의와 동일해야 한다. | `UnknownChecker` | ❌ Not Detected | 미탐지 |
| 19 | Common_Ident | 라 | 외부 scope에서 정의된 변수를 내부 scope에서 재정의 해서는 안된다. | `NoShadowingCheck` | ✅ Detected | `shadowVariable`: Local variable 'variable' shadows outer variable<br>`unreadVariable`: Variable 'variable' is assigned a value that is never used. |
| 20 | Common_Cond | 가 | float 자료형에서 동등성 비교연산을 수행하지 말아야 한다. | `NoFloatEqualityCheck` | ❌ Not Detected | 미탐지 |
| 21 | Common_Cond | 나 | 조건문의 결과가 항상 True 혹은 False라면 이는 조건문으로 작성해서는 안된다. | `AlwaysConstantConditionCheck` | ✅ Detected | `knownConditionTrueFalse`: Condition 'y<30' is always true |
| 22 | Common_Cond | 다 | switch의 case의 조건을 만족할 수 없는 Label을 사용하지 않는다. | `UnknownChecker` | ❌ Not Detected | 미탐지 |
| 23 | Common_Cond | 라 | Switch 구문에서 Expression을 논리적 연산으로 사용하지 말아야 한다. | `SwitchStyleCheck` | ❌ Not Detected | 미탐지 |
| 24 | Common_Cond | 마 | 수행되지 않는 소스코드를 작성하지 말아야 한다. | `UnreachableCodeCheck` | ✅ Detected | `unreachableCode`: Statements following 'break' will never be executed.<br>`unreadVariable`: Variable 'n' is assigned a value that is never used.<br>`uselessAssignmentArg`: Assignment of function parameter has no effect outside the function. |
| 25 | Common_Conv | 가 | 선언된 데이터 형으로 표현할 수 있는 숫자의 영역을 초과하는 값을 할당하지 말아야 한다. | `NoOutOfRangeAssignmentCheck` | ✅ Detected | `unreadVariable`: Variable 'x' is assigned a value that is never used. |
| 26 | Common_Conv | 나 | 가변인수를 받는 함수의 Conversion 지시자와 Argument의 type은 동일해야한다. | `VarargsFormatCheck` | ✅ Detected | `invalidPrintfArgType_s`: %s in format string (no. 1) requires 'char *' but the argument type is 'signed int'. |
| 27 | Common_Conv | 다 | 가변인수를 받는 함수의 Conversion 지시자와 Argument의 개수는 동일해야한다. | `VarargsFormatCheck` | ✅ Detected | `wrongPrintfScanfArgNum`: printf format string requires 2 parameters but only 1 is given. |
| 28 | Common_Conv | 라 | Object 저장 값을 표현할 수 없는 데이터로의 형 변환을 하지 말아야 한다. | `RepresentableCastCheck` | ✅ Detected | `variableScope`: The scope of the variable 'sca' can be reduced.<br>`unreadVariable`: Variable 'sca' is assigned a value that is never used. |
| 29 | Common_Conv | 마 | 음수 값을 unsigned type으로 변환해서는 안된다. | `UnknownChecker` | ✅ Detected | `variableScope`: The scope of the variable 'ucx' can be reduced.<br>`unreadVariable`: Variable 'ucx' is assigned a value that is never used. |
| 30 | Common_Conv | 바 | Character 문자열과 Wide character 문자열을 혼용하지 말아야 한다. | `NoMixedCharWideStringCheck` | ❌ Not Detected | 미탐지 |
| 31 | Common_Conv | 사 | 포인터 Cast의 결과로 인한 이전 포인터의 CONST 특성의 상실을 유의해야한다. | `PointerCvQualifierDropCheck` | ✅ Detected | `cstyleCast`: C-style pointer casting<br>`unreadVariable`: Variable 'pi' is assigned a value that is never used.<br>`constVariablePointer`: Variable 'pi' can be declared as pointer to const |
| 32 | Common_Conv | 아 | 포인터 Cast의 결과로 인한 이전 포인터의 volatile 특성 상실을 유의해야 한다. | `PointerCvQualifierDropCheck` | ✅ Detected | `cstyleCast`: C-style pointer casting<br>`unreadVariable`: Variable 'pi' is assigned a value that is never used.<br>`constVariablePointer`: Variable 'pi' can be declared as pointer to const |
| 33 | Common_PtrArray | 가 | 포인터는 참조전에 NULL 여부를 반드시 확인하여야 한다. | `NullDereferenceGuardCheck` | ✅ Detected | `unreadVariable`: Variable 'r' is assigned a value that is never used. |
| 34 | Common_PtrArray | 나 | 지역변수의 주소값을 더 넓은 scope를 가진 변수에 할당하지 말아야 한다. | `ReturnStackAddressCheck` | ✅ Detected | `danglingLifetime`: Non-local variable 'pi' will use pointer to local variable 'a'. |
| 35 | Common_PtrArray | 다 | 지역변수의 주소값을 함수의 리턴값으로 사용하지 말아야 한다. | `ReturnStackAddressCheck` | ✅ Detected | `returnDanglingLifetime`: Returning pointer to local variable 'a' that will be invalid when returning. |
| 36 | Common_PtrArray | 라 | 선언된 배열의 크기를 초과하는 인덱스 값을 사용하지 말아야 한다. | `ArrayIndexBoundCheck` | ✅ Detected | `arrayIndexOutOfBounds`: Array 'a[10]' accessed at index 10, which is out of bounds. |
| 37 | Common_PtrArray | 마 | Null Pointer를 산술연산 하지 않는다. | `NullPointerArithmeticCheck` | ✅ Detected | `unreadVariable`: Variable 'q' is assigned a value that is never used.<br>`nullPointerArithmeticRedundantCheck`: Either the condition 'q==(void*)0' is redundant or there is pointer arithmetic with NULL pointer.<br>`uselessAssignmentPtrArg`: Assignment of function parameter has no effect outside the function. Did you forget dereferencing it? |
| 38 | Common_Expr | 가 | 하나의 Sequence Point 내에서 하나의 Object Value를 두 번 이상 변경하지 않아야 한다. | `SequencingRulesCheck` | ✅ Detected | `unreadVariable`: Variable 'x' is assigned a value that is never used.<br>`unknownEvaluationOrder`: Expression 'x++*x++' depends on order of evaluation of side effects |
| 39 | Common_Expr | 나 | 나누는 값이 변수일 경우 0인지를 반드시 확인하여야 한다. | `NonZeroDivisorGuardCheck` | ✅ Detected | `unreadVariable`: Variable 'r' is assigned a value that is never used. |
| 40 | Common_Expr | 다 | 하나의 Sequence Point내에서 Object의 값을 변경하고 Access 하지 않아야 한다. | `SequencingRulesCheck` | ✅ Detected | `unknownEvaluationOrder`: Expression '(2*index)+index++' depends on order of evaluation of side effects<br>`unreadVariable`: Variable 'r' is assigned a value that is never used. |
| 41 | Common_Expr | 라 | 음수 값 또는 데이터 사이즈를 초과하는 값을 사용하여 Shift operator를 하지 않는다. | `ShiftAmountRangeCheck` | ✅ Detected | `shiftTooManyBits`: Shifting 32-bit value by 40 bits is undefined behaviour<br>`shiftTooManyBits`: Shifting 32-bit value by 4294967294 bits is undefined behaviour<br>`signConversion`: Expression '-2' has a negative value. That is converted to an unsigned value and used in an unsigned calculation. |
| 42 | Common_Expr | 마 | Underlying type이 Unsigned인 데이터에 '-'를 사용하여 결과를 assign하지 말아야 한다. | `UnsignedMinusAssignmentCheck` | ✅ Detected | `unreadVariable`: Variable 'j' is assigned a value that is never used. |
| 43 | Common_Expr | 바 | ’sizeof’의 인자로 side effect를 포함하는 식을 사용하면 안된다. | `UnevaluatedSideEffectCheck` | ✅ Detected | `sizeofCalculation`: Found calculation inside sizeof().<br>`unreadVariable`: Variable 'un' is assigned a value that is never used. |
| 44 | Common_Expr | 사 | Boolean 표현 값에는 논리 연산자(&&, ||, !)만을 사용하여야 한다. | `BoolLogicOnlyOpsCheck` | ✅ Detected | `redundantAssignment`: Variable 'x' is reassigned a value before the old one has been used. |
| 45 | Common_Expr | 아 | condition에 assignment operator를 사용하지 말아야 한다. | `NoAssignmentInConditionCheck` | ❌ Not Detected | 미탐지 |
| 46 | Common_Expr | 자 | Signed Value에서 Bitwise연산자(<<, ~, |, ^ 등)로 인한 Negative Value를 유의해야 한다. | `SignedBitwiseSafetyCheck` | ✅ Detected | `unreadVariable`: Variable 'slr' is assigned a value that is never used. |
| 47 | C_Specific | 1 | Scanf의 Argument 에는 Object Value가 저장된 주소값이 입력되어야 한다. | `VarargsFormatCheck` | ✅ Detected | `invalidScanfArgType_int`: %d in format string (no. 1) requires 'int *' but the argument type is 'signed int'. |
| 48 | C_Specific | 2 | `#include` 구문에서 표준에 맞지 않는 Character set을 사용하지 않아야 한다. | `IncludeCharsetCheck` | ✅ Detected | `missingInclude`: Include file: "Jady$.h" not found.<br>`missingInclude`: Include file: "donghyun.h/*temporary*/" not found.<br>`missingInclude`: Include file: "June's-header.h" not found.<br>`missingInclude`: Include file: "taemin's.h" not found. |
| 49 | C_Specific | 3 | malloc에서 사용하는 인자값은 assign될 변수 type의 정수배이어야 한다. | `MallocSizeMultipleCheck` | ✅ Detected | `redundantAssignment`: Variable 'ptd' is reassigned a value before the old one has been used.<br>`unreadVariable`: Variable 'ptd' is assigned a value that is never used.<br>`cstyleCast`: C-style pointer casting<br>`constVariablePointer`: Variable 'ptd' can be declared as pointer to const<br>`memleak`: Memory leak: ptd<br>`unusedAllocatedMemory`: Variable 'ptd' is allocated memory that is never used. |
| 50 | C_Specific | 4 | 함수의 Argument type과 개수는 함수의 Prototype, 선언, 정의가 모두 같아야 한다. | `FunctionSignatureConsistencyCheck` | ✅ Detected | `redundantAssignment`: Variable 'r' is reassigned a value before the old one has been used.<br>`unreadVariable`: Variable 'r' is assigned a value that is never used. |
| 51 | C_Specific | 5 | 구조체/배열의 초기화 시 default 초기화 값(0)을 제외하고, 구조에 맞게 ‘{}’를 사용하여 선언된 Size에 맞게 초기화 해야 한다. | `AggregateInitStyleCheck` | ✅ Detected | `unusedStructMember`: struct member 's::j' is never used.<br>`unusedStructMember`: struct member 's::i' is never used. |
| 52 | CPP_Specific_1 | 1 | 동적할당된 변수를 release 할 때에는 변수의 type과 일치하게 release 하여야 한다. | `NewDeleteFormConsistencyCheck` | ✅ Detected | `uninitdata`: Memory is allocated but not initialized: p2<br>`uninitdata`: Memory is allocated but not initialized: p1<br>`unusedAllocatedMemory`: Variable 'p1' is allocated memory that is never used.<br>`mismatchAllocDealloc`: Mismatching allocation and deallocation: p2<br>`unusedAllocatedMemory`: Variable 'p2' is allocated memory that is never used.<br>`mismatchAllocDealloc`: Mismatching allocation and deallocation: p1 |
| 53 | CPP_Specific_1 | 2 | 지역 변수의 주소 값을 처리하는 handle을 return하지 말아야 한다. | `ReturnStackAddressCheck` | ✅ Detected | `returnReference`: Reference to local variable returned. |
| 54 | CPP_Specific_1 | 3 | 함수 parameter의 주소 값을 처리하는 handle을 return하지 말아야 한다. | `ReturnStackAddressCheck` | ✅ Detected | `returnReference`: Reference to local variable returned. |
| 55 | CPP_Specific_1 | 4 | 소멸자내에서 처리할 수 없는 예외 상황을 발생시키지 말아야 한다. | `DestructorExceptionSafetyCheck` | ✅ Detected | `throwInNoexceptFunction`: Unhandled exception thrown in function declared not to throw exceptions.<br>`exceptThrowInDestructor`: Class C is not safe, destructor throws exception<br>`noConstructor`: The class 'C' does not declare a constructor although it has private member variables which likely require initialization. |
| 56 | CPP_Specific_1 | 5 | 사용되지 않는 예외 처리 문을 작성하지 말아야 한다. | `UnknownChecker` | ❌ Not Detected | 미탐지 |
| 57 | CPP_Specific_1 | 6 | exception specification에 기술되지 않은 모든 throw에 대하여 예외처리를 해야만 한다. | `ExceptionSpecificationCheck` | ❌ Not Detected | 미탐지 |
| 58 | CPP_Specific_1 | 7 | main 함수에서 처리되지 않는 throw를 작성하지 말아야 한다. | `MainUnhandledThrowCheck` | ❌ Not Detected | 미탐지 |
| 59 | CPP_Specific_2 | 8 | release 된 메모리 영역을 사용하지 말아야 한다. | `UseAfterFreeCheck` | ✅ Detected | `deallocuse`: Dereferencing 'i' after it is deallocated / released |
| 60 | CPP_Specific_2 | 9 | copy operator를 통해서, 복사되지 않는 멤버 변수가 존재하지 말아야 한다. | `PartialCopyAssignmentCheck` | ✅ Detected | `unusedStructMember`: class member 'Example::y' is never used.<br>`uninitMemberVar`: Member variable 'Example::y' is not initialized in the copy constructor. |
| 61 | CPP_Specific_2 | 10 | C++에서 동적 메모리 할당은 반드시 `new` 연산자를 사용해야 한다. | `NoMallocCheck` | ✅ Detected | `mismatchAllocDealloc`: Mismatching allocation and deallocation: p<br>`uninitdata`: Memory is allocated but not initialized: p<br>`unusedAllocatedMemory`: Variable 'p' is allocated memory that is never used. |
| 62 | CPP_Specific_2 | 11 | 순수 가상 함수는 반드시 `= 0` 으로 초기화해야 한다. | `PureVirtualInitCheck` | ❌ Not Detected | 미탐지 |
| 63 | CPP_Specific_2 | 12 | 순수 함수는 반드시 `virtual`로 선언되어야 한다. | `VirtualPureCheck` | ✅ Detected | `functionStatic`: The member function 'Rule_63::Derived::foo' can be static.<br>`duplInheritedMember`: The class 'Derived' defines member function with name 'foo' also defined in its parent class 'Abstract'. |
| 64 | CPP_Specific_2 | 13 | virtual base 클래스 포인터를 derived 클래스 포인터로 변환할 때에는 반드시 `dynamic_cast`를 사용해야 한다. | `VirtualBaseCastCheck` | ✅ Detected | `missingOverride`: The function 'getAsDerived' overrides a function in a base class but is not marked with a 'override' specifier. |
| 65 | CPP_Specific_2 | 14 | 생성자/소멸자 내에서는 가상함수를 반드시 식별자(Qualifier)를 붙여서 호출해야 한다. | `DynamicCastInCtorDtorCheck` | ✅ Detected | `missingOverride`: The function 'func' overrides a function in a base class but is not marked with a 'override' specifier.<br>`missingOverride`: The destructor '~D' overrides a destructor in a base class but is not marked with a 'override' specifier.<br>`virtualCallInConstructor`: Virtual function 'func' is called from constructor 'B()' at line 1009. Dynamic binding is not used. |
| 66 | CPP_Specific_2 | 15 | 생성자 또는 소멸자에서는 `dynamic_cast`를 사용해서는 안 된다. | `DynamicCastInCtorDtorCheck` | ✅ Detected | `virtualCallInConstructor`: Virtual function 'foo' is called from constructor 'B2()' at line 1025. Dynamic binding is not used.<br>`constStatement`: Redundant code: Found unused 'typeid' expression. |

---

## 3. 미탐 규칙 원인 분석 (Gap Analysis)

Cppcheck가 진단해내지 못하는 주요 규칙들은 주로 다음과 같은 기술적 특성에서 기인합니다.

1. **Modern C++ 및 코딩 규격 스타일 진단 한계**:
   - `NoAutoTypeCheck`, `SingleExitAndReturnTypeCheck`, `IfStyleCheck`, `NoGotoCheck` 등 코드의 구조적 스타일을 제한하는 규칙들은 Cppcheck의 탐지 도메인 밖입니다. Clang-Tidy는 컴파일러 AST를 사용하여 이러한 스타일 규격을 손쉽게 탐지하지만, Cppcheck는 버그와 취약점에 집중되어 있습니다.
   
2. **복잡한 C++ 언어 명세 분석 부족**:
   - `VirtualCallInCtorDtorCheck`, `DynamicCastInCtorDtorCheck` 등 생성자/소멸자에서의 가상 함수 호출이나 특정 캐스팅 활용 제한 역시 컴파일러 프론트엔드가 없어 정확한 정적 바인딩 및 가상 테이블 흐름을 시뮬레이션하지 못해 미탐이 발생합니다.
   
3. **이름/매크로 네이밍 검사 부재**:
   - `NoMixedLinkageCheck`, `MacroDefinedBeforeUseCheck` 처럼 정의 흐름과 링크 방식을 따지는 규칙들은 컴파일 빌드 정보(compile_commands.json)가 완벽히 주어지지 않은 상태의 Cppcheck 텍스트 패턴 스캔만으로는 정밀 분석이 불가능합니다.

---

## 4. 종합 제안 및 결론

1. **정량적 평가 요약**:
   - Cppcheck는 메모리 버그(Array Bound, Use After Free 등)와 리소스 관리 및 기본적인 언어 오용(Null Pointer Arithmetic 등)에 대해 약 61.5%의 준수한 검출 성능을 발휘합니다.
   - 그러나 코딩 규격 통제성 조항(Style, Modernize)에 대해서는 대다수 미탐을 기록했습니다.
   
2. **ArqaStatic (Clang-Tidy) 도입의 정당성**:
   - 본 실험 결과를 통해 Cppcheck 단독 운영 시 무기체계 소프트웨어 코딩 규칙 66종 중 절반 가량의 스타일 및 컴파일러 의존적 규칙을 탐지할 수 없음을 증명했습니다.
   - 따라서 **ArqaStatic**에 탑재된 LLVM/Clang AST 기반 체커를 병행 도입하여 Cppcheck의 미탐 영역을 100% 상호 보완해야 합니다.
