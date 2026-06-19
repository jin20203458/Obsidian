# 1. Cppcheck 정탐 (True Positive) 상세 분석 문서

본 문서는 Cppcheck가 소스코드의 결함을 **정확하게 진단(정탐)**해낸 항목(총 29종)들을 정리한 자료입니다.
기대했던 경고(Expected)와 실제 출력된 핵심 경고(Actual)를 매핑하여 검증 신뢰도를 높였습니다.

| 규칙 번호 | 카테고리 | 규칙 핵심 요약 | 기대 경고 (Expected ID) | 실제 검출 경고 (Actual ID) | 판정 |
| :---: | :--- | :--- | :--- | :--- | :---: |
| 05 | Common_Style | 함수 선언시 사용된 리턴형과 함수 내에서 사용되는 리턴형은 일치해야 한다. | `style (Return Type Mismatch)` | `missingReturn` | ✅ 정탐 |
| 08 | Common_Style | goto 문을 사용하지 말아야 한다. | `style (Goto Usage)` | `knownConditionTrueFalse` | ✅ 정탐 |
| 14 | Common_Init | 변수는 사용전 반드시 초기화되어야 한다. | `uninitvar` | `uninitvar` | ✅ 정탐 |
| 15 | Common_Init | 값이 초기화되지 않은 변수의 포인터를 함수의 Read-only(const)로 사용해서는 안된다. | `uninitvar (for Const Param)` | `uninitvar` | ✅ 정탐 |
| 19 | Common_Ident | 외부 scope에서 정의된 변수를 내부 scope에서 재정의 해서는 안된다. | `shadowVariable` | `shadowVariable` | ✅ 정탐 |
| 21 | Common_Cond | 조건문의 결과가 항상 True 혹은 False라면 이는 조건문으로 작성해서는 안된다. | `knownConditionTrueFalse` | `unsignedLessThanZero, knownConditionTrueFalse` | ✅ 정탐 |
| 24 | Common_Cond | 수행되지 않는 소스코드를 작성하지 말아야 한다. | `unreachableCode` | `unreachableCode` | ✅ 정탐 |
| 26 | Common_Conv | 가변인수를 받는 함수의 Conversion 지시자와 Argument의 type은 동일해야한다. | `invalidPrintfArgType` | `invalidPrintfArgType_s` | ✅ 정탐 |
| 27 | Common_Conv | 가변인수를 받는 함수의 Conversion 지시자와 Argument의 개수는 동일해야한다. | `wrongPrintfScanfArgNum` | `wrongPrintfScanfArgNum` | ✅ 정탐 |
| 34 | Common_PtrArray | 지역변수의 주소값을 더 넓은 scope를 가진 변수에 할당하지 말아야 한다. | `danglingLifetime / returnStackAddress` | `danglingLifetime` | ✅ 정탐 |
| 35 | Common_PtrArray | 지역변수의 주소값을 함수의 리턴값으로 사용하지 말아야 한다. | `returnDanglingLifetime` | `returnDanglingLifetime` | ✅ 정탐 |
| 36 | Common_PtrArray | 선언된 배열의 크기를 초과하는 인덱스 값을 사용하지 말아야 한다. | `arrayIndexOutOfBounds` | `arrayIndexOutOfBounds` | ✅ 정탐 |
| 37 | Common_PtrArray | Null Pointer를 산술연산 하지 않는다. | `nullPointerArithmetic` | `nullPointerArithmeticRedundantCheck` | ✅ 정탐 |
| 38 | Common_Expr | 하나의 Sequence Point 내에서 하나의 Object Value를 두 번 이상 변경하지 않아야 한다. | `unknownEvaluationOrder` | `unknownEvaluationOrder` | ✅ 정탐 |
| 40 | Common_Expr | 하나의 Sequence Point내에서 Object의 값을 변경하고 Access 하지 않아야 한다. | `unknownEvaluationOrder` | `unknownEvaluationOrder` | ✅ 정탐 |
| 41 | Common_Expr | 음수 값 또는 데이터 사이즈를 초과하는 값을 사용하여 Shift operator를 하지 않는다. | `shiftTooManyBits / signConversion` | `shiftTooManyBits, signConversion` | ✅ 정탐 |
| 43 | Common_Expr | ’sizeof’의 인자로 side effect를 포함하는 식을 사용하면 안된다. | `sizeofCalculation (Side Effect inside sizeof)` | `sizeofCalculation` | ✅ 정탐 |
| 45 | Common_Expr | condition에 assignment operator를 사용하지 말아야 한다. | `assignmentInAssert / style (Assignment in Condition)` | `knownConditionTrueFalse` | ✅ 정탐 |
| 47 | C_Specific | Scanf의 Argument 에는 Object Value가 저장된 주소값이 입력되어야 한다. | `invalidScanfArgType` | `invalidScanfArgType_int` | ✅ 정탐 |
| 52 | CPP_Specific_1 | 동적할당된 변수를 release 할 때에는 변수의 type과 일치하게 release 하여야 한다. | `mismatchAllocDealloc` | `mismatchAllocDealloc` | ✅ 정탐 |
| 53 | CPP_Specific_1 | 지역 변수의 주소 값을 처리하는 handle을 return하지 말아야 한다. | `returnReference` | `returnReference` | ✅ 정탐 |
| 54 | CPP_Specific_1 | 함수 parameter의 주소 값을 처리하는 handle을 return하지 말아야 한다. | `returnReference` | `returnReference` | ✅ 정탐 |
| 55 | CPP_Specific_1 | 소멸자내에서 처리할 수 없는 예외 상황을 발생시키지 말아야 한다. | `throwInNoexceptFunction / exceptThrowInDestructor` | `throwInNoexceptFunction, exceptThrowInDestructor` | ✅ 정탐 |
| 58 | CPP_Specific_1 | main 함수에서 처리되지 않는 throw를 작성하지 말아야 한다. | `error (Unhandled Throw in Main)` | `duplicateBreak, throwInEntryPoint` | ✅ 정탐 |
| 59 | CPP_Specific_2 | release 된 메모리 영역을 사용하지 말아야 한다. | `deallocuse (Use After Free)` | `deallocuse` | ✅ 정탐 |
| 60 | CPP_Specific_2 | copy operator를 통해서, 복사되지 않는 멤버 변수가 존재하지 말아야 한다. | `uninitMemberVar (Partial Copy Assignment)` | `uninitMemberVar, operatorEqVarError` | ✅ 정탐 |
| 61 | CPP_Specific_2 | C++에서 동적 메모리 할당은 반드시 `new` 연산자를 사용해야 한다. | `error (Malloc in C++ / mismatchAllocDealloc)` | `mismatchAllocDealloc` | ✅ 정탐 |
| 65 | CPP_Specific_2 | 생성자/소멸자 내에서는 가상함수를 반드시 식별자(Qualifier)를 붙여서 호출해야 한다. | `virtualCallInConstructor` | `virtualCallInConstructor` | ✅ 정탐 |
| 66 | CPP_Specific_2 | 생성자 또는 소멸자에서는 `dynamic_cast`를 사용해서는 안 된다. | `virtualCallInConstructor` | `virtualCallInConstructor` | ✅ 정탐 |
