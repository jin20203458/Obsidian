# 2. Cppcheck 미탐/파생 경고 (False Negative & Collateral) 상세 분석 문서

본 문서는 Cppcheck가 소스코드의 결함을 **진단하지 못했거나(완전 미탐)**, 규칙 위반과 무관한 **엉뚱한 경고만 띄운 항목(파생 경고 발생)**(총 36종)들을 정리한 자료입니다.
주로 코딩 스타일, 네이밍, 구조 제한 규칙에서 Cppcheck의 정적 분석 한계가 드러납니다.

| 규칙 번호 | 카테고리 | 규칙 핵심 요약 | 기대 경고 (Expected) | 실제 발생 경고 (Actual) | 미탐/오탐 사유 (분석 한계) |
| :---: | :--- | :--- | :--- | :--- | :--- |
| 02 | Common_Style | Switch 구문에서 첫번째 Label 전에 코드 구문이 존재하면 안된다. | `syntaxError / style (Switch Label)` | ❌ (경고 없음) | 진단 도메인 밖 (스타일/문법 한계) |
| 03 | Common_Style | 함수/변수의 선언 시 type을 명시해야 한다. (사용자정의해더 검사사) | `style (Auto Type)` | ❌ (경고 없음) | 진단 도메인 밖 (스타일/문법 한계) |
| 04 | Common_Style | 의미 없는 구문은 사용하지 말아야 한다.(side effect) | `redundantCode (Meaningless Expr)` | ❌ (경고 없음) | 진단 도메인 밖 (스타일/문법 한계) |
| 06 | Common_Style | 외부 함수 사용시 이를 명시하고 사용해야 한다. | `style (Extern Decl)` | ❌ (경고 없음) | 진단 도메인 밖 (스타일/문법 한계) |
| 07 | Common_Style | 전처리에 사용되는 매크로는 사용 전에 반드시 정의되어야 한다.(사용자정의해더 검사사) | `preprocessorError (Macro Use Before Def)` | ❌ (경고 없음) | 진단 도메인 밖 (스타일/문법 한계) |
| 09 | Common_Style | 하나의 함수는 하나의 Exit Point를 가져야 한다. | `style (Multiple Exit Points)` | ❌ (경고 없음) | 진단 도메인 밖 (스타일/문법 한계) |
| 10 | Common_Style | switch ~ case 문은 default 문이 포함되어야 한다. | `style (Missing Default in Switch)` | ❌ (경고 없음) | 진단 도메인 밖 (스타일/문법 한계) |
| 11 | Common_Style | 한 줄에 하나의 명령문을 사용한다. | `style (Multiple Statements Per Line)` | ❌ (경고 없음) | 진단 도메인 밖 (스타일/문법 한계) |
| 12 | Common_Style | if - else  if 문은 else 문도 반드시 포함 시킨다. | `style (Missing Else)` | ❌ (경고 없음) | 진단 도메인 밖 (스타일/문법 한계) |
| 13 | Common_Init | char 배열의 초기화시 char 배열의 크기는 초기화 문자열의 크기와 Null을 포함하는 크기로 선언되어야 한다. | `bufferAccessOutOfBounds / arrayIndexOutOfBounds` | ❌ (경고 없음) | 진단 도메인 밖 (스타일/문법 한계) |
| 16 | Common_Ident | external과 internal linkage의 특성을 동시에 가질 수 없다. | `style (Mixed Linkage)` | ❌ (경고 없음) | 진단 도메인 밖 (스타일/문법 한계) |
| 17 | Common_Ident | external linkage scope에서 선언된 함수나 Object의 이름은 유일해야 한다. | `error (Unique Linkage Name)` | ❌ (경고 없음) | 진단 도메인 밖 (스타일/문법 한계) |
| 18 | Common_Ident | external linkage scope에서 정의된 함수나 Object의 데이터형은 선언 시 정의와 동일해야 한다. | `error (Linkage Type Consistency)` | ❌ (경고 없음) | 진단 도메인 밖 (스타일/문법 한계) |
| 20 | Common_Cond | float 자료형에서 동등성 비교연산을 수행하지 말아야 한다. | `style (Float Equality)` | ❌ (경고 없음) | 진단 도메인 밖 (스타일/문법 한계) |
| 22 | Common_Cond | switch의 case의 조건을 만족할 수 없는 Label을 사용하지 않는다. | `unreachableCode (Switch unreachable)` | ❌ (경고 없음) | 진단 도메인 밖 (스타일/문법 한계) |
| 23 | Common_Cond | Switch 구문에서 Expression을 논리적 연산으로 사용하지 말아야 한다. | `style (Boolean Logic in Switch)` | ❌ (경고 없음) | 진단 도메인 밖 (스타일/문법 한계) |
| 25 | Common_Conv | 선언된 데이터 형으로 표현할 수 있는 숫자의 영역을 초과하는 값을 할당하지 말아야 한다. | `integerOverflow / outOfBounds` | `unreadVariable` | 결함 미탐지 (파생 경고만 발생) |
| 28 | Common_Conv | Object 저장 값을 표현할 수 없는 데이터로의 형 변환을 하지 말아야 한다. | `integerOverflow / truncation` | `unreadVariable`, `variableScope` | 결함 미탐지 (파생 경고만 발생) |
| 29 | Common_Conv | 음수 값을 unsigned type으로 변환해서는 안된다. | `signConversion` | `unreadVariable`, `variableScope` | 결함 미탐지 (파생 경고만 발생) |
| 30 | Common_Conv | Character 문자열과 Wide character 문자열을 혼용하지 말아야 한다. | `style (Mixed Char/Wide Char)` | ❌ (경고 없음) | 진단 도메인 밖 (스타일/문법 한계) |
| 31 | Common_Conv | 포인터 Cast의 결과로 인한 이전 포인터의 CONST 특성의 상실을 유의해야한다. | `style (Const Qualifier Drop)` | `unreadVariable`, `cstyleCast`, `constVariablePointer` | 결함 미탐지 (파생 경고만 발생) |
| 32 | Common_Conv | 포인터 Cast의 결과로 인한 이전 포인터의 volatile 특성 상실을 유의해야 한다. | `style (Volatile Qualifier Drop)` | `unreadVariable`, `cstyleCast`, `constVariablePointer` | 결함 미탐지 (파생 경고만 발생) |
| 33 | Common_PtrArray | 포인터는 참조전에 NULL 여부를 반드시 확인하여야 한다. | `nullPointer` | `unreadVariable` | 결함 미탐지 (파생 경고만 발생) |
| 39 | Common_Expr | 나누는 값이 변수일 경우 0인지를 반드시 확인하여야 한다. | `zerodiv` | `unreadVariable` | 결함 미탐지 (파생 경고만 발생) |
| 42 | Common_Expr | Underlying type이 Unsigned인 데이터에 '-'를 사용하여 결과를 assign하지 말아야 한다. | `signConversion / unsignedMinus` | `unreadVariable` | 결함 미탐지 (파생 경고만 발생) |
| 44 | Common_Expr | Boolean 표현 값에는 논리 연산자(&&, ||, !)만을 사용하여야 한다. | `style (Bitwise logic on Boolean)` | `redundantAssignment` | 결함 미탐지 (파생 경고만 발생) |
| 45 | Common_Expr | condition에 assignment operator를 사용하지 말아야 한다. | `assignmentInAssert / style (Assignment in Condition)` | ❌ (경고 없음) | 진단 도메인 밖 (스타일/문법 한계) |
| 46 | Common_Expr | Signed Value에서 Bitwise연산자(<<, ~, |, ^ 등)로 인한 Negative Value를 유의해야 한다. | `shiftNegative / bitwiseNegative` | `unreadVariable` | 결함 미탐지 (파생 경고만 발생) |
| 48 | C_Specific | `#include` 구문에서 표준에 맞지 않는 Character set을 사용하지 않아야 한다. | `syntaxError (Invalid Include Charset)` | `missingInclude` | 결함 미탐지 (파생 경고만 발생) |
| 49 | C_Specific | malloc에서 사용하는 인자값은 assign될 변수 type의 정수배이어야 한다. | `invalidAllocationSize / memoryLeak` | `cstyleCast`, `redundantAssignment`, `constVariablePointer`, `unusedAllocatedMemory`, `unreadVariable`, `memleak` | 결함 미탐지 (파생 경고만 발생) |
| 50 | C_Specific | 함수의 Argument type과 개수는 함수의 Prototype, 선언, 정의가 모두 같아야 한다. | `error (Function Signature Consistency)` | `unreadVariable`, `redundantAssignment` | 결함 미탐지 (파생 경고만 발생) |
| 51 | C_Specific | 구조체/배열의 초기화 시 default 초기화 값(0)을 제외하고, 구조에 맞게 ‘{}’를 사용하여 선언된 Size에 맞게 초기화 해야 한다. | `style (Aggregate Init Style)` | `unusedStructMember` | 결함 미탐지 (파생 경고만 발생) |
| 57 | CPP_Specific_1 | exception specification에 기술되지 않은 모든 throw에 대하여 예외처리를 해야만 한다. | `error (Exception Specification Mismatch)` | ❌ (경고 없음) | 진단 도메인 밖 (스타일/문법 한계) |
| 62 | CPP_Specific_2 | 순수 가상 함수는 반드시 `= 0` 으로 초기화해야 한다. | `style (Pure Virtual Init)` | ❌ (경고 없음) | 진단 도메인 밖 (스타일/문법 한계) |
| 63 | CPP_Specific_2 | 순수 함수는 반드시 `virtual`로 선언되어야 한다. | `style (Virtual Pure)` | `duplInheritedMember`, `functionStatic` | 결함 미탐지 (파생 경고만 발생) |
| 64 | CPP_Specific_2 | virtual base 클래스 포인터를 derived 클래스 포인터로 변환할 때에는 반드시 `dynamic_cast`를 사용해야 한다. | `style (Virtual Base Cast)` | `unreadVariable`, `missingOverride`, `redundantAssignment`, `uselessAssignmentPtrArg` | 결함 미탐지 (파생 경고만 발생) |
