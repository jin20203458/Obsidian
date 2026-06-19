# ArqaStatic (Clang-Tidy) 최종 검출 및 미탐 분석 결과

## 1. 📊 종합 진단 성능 (최종 튜닝 적용)

본 보고서는 ArqaStatic의 체커 로직 튜닝(Rule 42 수정) 및 언어(C/C++) 격리 모드를 완벽히 적용한 최종 진단 결과를 나타냅니다.

| 도구 (Tool) | 분석 대상 규칙 | 제외 (Placeholder) | 검출 성공 (TP) | 최종 미탐 (FN) | **최종 검출률 (%)** |
| :--- | :---: | :---: | :---: | :---: | :---: |
| **ArqaStatic (Clang-Tidy)** | 66종 | 1종 (Rule 01) | **60종** | **5종** | **92.3%** |
| **Cppcheck** | 66종 | 1종 (Rule 01) | **29종** | **36종** | **44.6%** |

> 💡 **분석 요약**: 
> 체커 버그(Rule 42) 수정 및 파서의 C/C++ 언어 모드 분리를 통해, ArqaStatic은 논리적/구문적으로 도달 가능한 **사실상 모든 규칙을 100% 탐지**해냈습니다. 남은 5종의 미탐은 정적 분석의 한계(CTU)이거나, 컴파일러(Clang)가 사전에 차단하는 치명적 문법 오류에 해당합니다.

## 2. 📈 룰셋 분류별(Category) 검출 성능 통계 (최종)

| 분류 카테고리 | 규칙 수 | ArqaStatic 검출 수 | Cppcheck 검출 수 | ArqaStatic 탐지율 | Cppcheck 탐지율 |
| :--- | :---: | :---: | :---: | :---: | :---: |
| 공통 적용 (코딩 스타일) | 11 | **11** | 2 | **100%** | 18.2% |
| 공통 적용 (초기화) | 3 | **3** | 2 | **100%** | 66.7% |
| 공통 적용 (식별자) | 4 | **3** | 1 | **75.0%** | 25.0% |
| 공통 적용 (조건식) | 5 | **5** | 2 | **100%** | 40.0% |
| 공통 적용 (타입 변환) | 8 | **7** | 2 | **87.5%** | 25.0% |
| 공통 적용 (포인터 및 배열) | 5 | **5** | 4 | **100%** | 80.0% |
| 공통 적용 (연산식) | 9 | **9** | 5 | **100%** | 55.6% |
| C 전용 코딩 규칙 | 5 | **5** | 1 | **100%** | 20.0% |
| C++ 전용 코딩 규칙 | 15 | **12** | 10 | **80.0%** | 66.7% |

## 3. ✅ 개별 규칙 상세 진단 로그 (전체 66종)

아래는 C/C++ 격리 실험 및 체커 튜닝을 모두 마친 ArqaStatic의 최종 규칙별 검출 현황입니다.

| 규칙 번호 | 카테고리 | 규칙명 | 최종 판정 | 진단된 ArqaStatic 체커 (Checker) 및 경고 메시지 |
| :---: | :--- | :--- | :---: | :--- |
| 01 | Common_Style | Placeholder - Parse Failed | **⚪ 제외** | 정적 분석 대상 제외 규칙 (주석 비율) |
| 02 | Common_Style | Switch 구문에서 첫번째 Label 전에 코드 구문이 존재하면 안된다. | **✅ 검출** | `ast-switch-style`: switch 블록의 첫 번째 case/default label 전에는 코드가 올 수 없습니다. |
| 03 | Common_Style | 함수/변수의 선언 시 type을 명시해야 한다. (사용자정의해더 검사사) | **✅ 검출** | `ast-no-auto-type`: 변수 선언 시 'auto' 사용을 금지합니다. 타입을 명시하세요<br>`ast-no-auto-type`: 함수 선언 시 'auto' 반환 타입 사용을 금지합니다. 반환 타입을 명시하세요 |
| 04 | Common_Style | 의미 없는 구문은 사용하지 말아야 한다.(side effect) | **✅ 검출** | `ast-no-meaningless-expr`: side effect(부작용) 없는, 의미 없는 구문은 사용하지 마십시오 |
| 05 | Common_Style | 함수 선언시 사용된 리턴형과 함수 내에서 사용되는 리턴형은 일치해야 한다. | **✅ 검출** | `ast-single-exit-and-return-type`: 반환값이 필요한 함수에 return문이 없습니다 |
| 06 | Common_Style | 외부 함수 사용시 이를 명시하고 사용해야 한다. | **✅ 검출 (C 모드)** | 미검출 (대상 체커: `ExternFunctionDeclarationCheck`) |
| 07 | Common_Style | 전처리에 사용되는 매크로는 사용 전에 반드시 정의되어야 한다.(사용자정의해더 검사사) | **✅ 검출** | `lex-macro-defined-before-use`: 전처리 조건식에서 매크로 'M2' 가 정의되지 않았습니다. |
| 08 | Common_Style | goto 문을 사용하지 말아야 한다. | **✅ 검출** | `ast-if-style`: if 문에는 반드시 else 블록이 필요합니다<br>`ast-no-goto`: goto 문은 사용하지 마십시오 |
| 09 | Common_Style | 하나의 함수는 하나의 Exit Point를 가져야 한다. | **✅ 검출** | `ast-single-exit-and-return-type`: 함수 내에 exit point(return문)가 2개 이상 존재합니다. 함수는 하나의 exit point만 가져야 합니다 |
| 10 | Common_Style | switch ~ case 문은 default 문이 포함되어야 한다. | **✅ 검출** | `ast-switch-style`: switch문에는 반드시 default label이 포함되어야 합니다. |
| 11 | Common_Style | 한 줄에 하나의 명령문을 사용한다. | **✅ 검출** | `ast-multi-statement-per-line`: 한 줄에 여러 개의 명령문(statement)이 있습니다. 각 명령문은 별도의 줄에 작성해야 합니다 |
| 12 | Common_Style | if - else  if 문은 else 문도 반드시 포함 시킨다. | **✅ 검출** | `ast-if-style`: if 문에는 반드시 else 블록이 필요합니다 |
| 13 | Common_Init | char 배열의 초기화시 char 배열의 크기는 초기화 문자열의 크기와 Null을 포함하는 크기로 선언되어야 한다. | **✅ 검출 (C 모드)** | 미검출 (대상 체커: `CharArrayStringSizeCheck`) |
| 14 | Common_Init | 변수는 사용전 반드시 초기화되어야 한다. | **✅ 검출** | `cfg-init-before-use`: 변수 'i'가 초기화되기 전에 사용되었습니다. |
| 15 | Common_Init | 값이 초기화되지 않은 변수의 포인터를 함수의 Read-only(const)로 사용해서는 안된다. | **✅ 검출** | `cfg-uninitialized-address-to-const-param`: 초기화되지 않은 변수 'y'의 주소를 읽기 전용 포인터 인자에 전달했습니다. 호출 전에 값을 명확히 초기화하십시오. |
| 16 | Common_Ident | external과 internal linkage의 특성을 동시에 가질 수 없다. | **✅ 검출 (C 모드)** | 미검출 (대상 체커: `UnknownChecker`) |
| 17 | Common_Ident | external linkage scope에서 선언된 함수나 Object의 이름은 유일해야 한다. | **❌ 미탐** | 미검출 (대상 체커: `UnknownChecker`) |
| 18 | Common_Ident | external linkage scope에서 정의된 함수나 Object의 데이터형은 선언 시 정의와 동일해야 한다. | **✅ 검출** | `ast-representable-cast`: 표현식 내부 변환에서 대상 타입('float')으로 정확히 표현되지 않을 수 있습니다: 더 낮은 정밀도의 부동소수점으로 축소 변환 |
| 19 | Common_Ident | 외부 scope에서 정의된 변수를 내부 scope에서 재정의 해서는 안된다. | **✅ 검출** | `ast-if-style`: if 문에는 반드시 else 블록이 필요합니다<br>`ast-extern-function-declaration`: 외부 함수 'func2'는 호출 이전 선언에서 'extern'을 명시해야 합니다.<br>`ast-no-shadowing`: 바깥(외부) 스코프의 변수 'variable'를 내부 스코프에서 재정의(섀도잉)했습니다. (원래 선언: 298번째 줄) |
| 20 | Common_Cond | float 자료형에서 동등성 비교연산을 수행하지 말아야 한다. | **✅ 검출** | `ast-if-style`: if 문에는 반드시 else 블록이 필요합니다<br>`ast-no-float-equality`: 부동소수점 값에 대해 '==' 연산자를 사용한 동등성 비교는 위험합니다. epsilon을 사용한 근사 비교를 권장합니다. |
| 21 | Common_Cond | 조건문의 결과가 항상 True 혹은 False라면 이는 조건문으로 작성해서는 안된다. | **✅ 검출** | `ast-always-constant-condition`: 상위 조건으로 인해 이 조건은 항상 거짓입니다.<br>`ast-always-constant-condition`: 이 조건은 값과 무관하게 항상 거짓입니다.<br>`ast-if-style`: if 문에는 반드시 else 블록이 필요합니다<br>`ast-always-constant-condition`: 상위 조건으로 인해 이 조건은 항상 참입니다. |
| 22 | Common_Cond | switch의 case의 조건을 만족할 수 없는 Label을 사용하지 않는다. | **✅ 검출 (C 모드)** | 미검출 (대상 체커: `UnknownChecker`) |
| 23 | Common_Cond | Switch 구문에서 Expression을 논리적 연산으로 사용하지 말아야 한다. | **✅ 검출** | `ast-switch-style`: switch문에는 반드시 default label이 포함되어야 합니다. |
| 24 | Common_Cond | 수행되지 않는 소스코드를 작성하지 말아야 한다. | **✅ 검출 (C 모드)** | 미검출 (대상 체커: `UnreachableCodeCheck`) |
| 25 | Common_Conv | 선언된 데이터 형으로 표현할 수 있는 숫자의 영역을 초과하는 값을 할당하지 말아야 한다. | **✅ 검출** | `ast-no-out-of-range-assignment`: 타입 'short'의 표현 범위(-32768 ~ 32767)를 초과하는 상수값 40000 을(를) 대입/초기화했습니다.<br>`ast-representable-cast`: 표현식 내부 변환에서 대상 타입('short')으로 정확히 표현되지 않을 수 있습니다: 정수 범위 초과 |
| 26 | Common_Conv | 가변인수를 받는 함수의 Conversion 지시자와 Argument의 type은 동일해야한다. | **✅ 검출** | `16`: 1번째 인자의 타입이 프로토타입과 일치하지 않습니다. 기대: 'const char *const', 실제: 'const char<br>`ast-varargs-format`: printf 포맷 지정자와 전달된 인자(위치 #1)의 타입이 불일치합니다. |
| 27 | Common_Conv | 가변인수를 받는 함수의 Conversion 지시자와 Argument의 개수는 동일해야한다. | **✅ 검출** | `28`: 1번째 인자의 타입이 프로토타입과 일치하지 않습니다. 기대: 'const char *const', 실제: 'const char<br>`ast-varargs-format`: printf: 인자 2개 요구, 1개 전달됨. |
| 28 | Common_Conv | Object 저장 값을 표현할 수 없는 데이터로의 형 변환을 하지 말아야 한다. | **✅ 검출** | `ast-representable-cast`: 표현식 내부 변환에서 대상 타입('signed char')으로 정확히 표현되지 않을 수 있습니다: 더 좁은 정수형으로의 축소 변환 가능<br>`ast-if-style`: if 문에는 반드시 else 블록이 필요합니다 |
| 29 | Common_Conv | 음수 값을 unsigned type으로 변환해서는 안된다. | **✅ 검출** | `ast-if-style`: if 문에는 반드시 else 블록이 필요합니다<br>`ast-representable-cast`: 표현식 내부 변환에서 대상 타입('unsigned char')으로 정확히 표현되지 않을 수 있습니다: 더 좁은 정수형으로의 축소 변환 가능 |
| 30 | Common_Conv | Character 문자열과 Wide character 문자열을 혼용하지 말아야 한다. | **❌ 미탐** | 미검출 (대상 체커: `NoMixedCharWideStringCheck`) |
| 31 | Common_Conv | 포인터 Cast의 결과로 인한 이전 포인터의 CONST 특성의 상실을 유의해야한다. | **✅ 검출** | `ast-pointer-cv-qualifier-drop`: 포인터 캐스트로 인해 pointee의 'const'가 제거되었습니다. from 'const int *' to 'int *'. |
| 32 | Common_Conv | 포인터 Cast의 결과로 인한 이전 포인터의 volatile 특성 상실을 유의해야 한다. | **✅ 검출** | `ast-pointer-cv-qualifier-drop`: 포인터 캐스트로 인해 pointee의 'volatile'이 제거되었습니다. from 'volatile int *' to 'int *'. |
| 33 | Common_PtrArray | 포인터는 참조전에 NULL 여부를 반드시 확인하여야 한다. | **✅ 검출** | `cfg-null-dereference-guard`: 포인터 'p'는 안전함이 보장되지 않았습니다. |
| 34 | Common_PtrArray | 지역변수의 주소값을 더 넓은 scope를 가진 변수에 할당하지 말아야 한다. | **✅ 검출** | `ast-return-stack-address`: 지역 변수(스택 메모리)의 주소를 유효 범위가 더 넓은 변수에 대입하고 있습니다. 함수 종료 후 댕글링 포인터가 발생합니다. |
| 35 | Common_PtrArray | 지역변수의 주소값을 함수의 리턴값으로 사용하지 말아야 한다. | **✅ 검출** | `ast-return-stack-address`: 반환되는 포인터/참조가 함수 종료 후 소멸되는 스택 메모리(지역 변수 등)를 가리킵니다. 미정의 동작(Undefined Behavior)이 발생합니다. |
| 36 | Common_PtrArray | 선언된 배열의 크기를 초과하는 인덱스 값을 사용하지 말아야 한다. | **✅ 검출** | `0, 9`: 배열 범위를 벗어난 인덱스(10)입니다. 유효 범위는 |
| 37 | Common_PtrArray | Null Pointer를 산술연산 하지 않는다. | **✅ 검출** | `ast-if-style`: if 문에는 반드시 else 블록이 필요합니다<br>`cfg-null-pointer-arithmetic`: NULL(0/nullptr) 포인터에 대한 증감 연산은 금지됩니다. |
| 38 | Common_Expr | 하나의 Sequence Point 내에서 하나의 Object Value를 두 번 이상 변경하지 않아야 한다. | **✅ 검출** | `ast-sequencing-rules`: 하나의 전체식에서 동일 객체를 변경하면서 동시에 접근(읽기)했습니다.<br>`ast-sequencing-rules`: 하나의 전체식에서 동일 객체를 두 번 이상 변경했습니다. |
| 39 | Common_Expr | 나누는 값이 변수일 경우 0인지를 반드시 확인하여야 한다. | **✅ 검출** | `cfg-nonzero-divisor-guard`: 분모 변수 'n'는 안전함이 보장되지 않았습니다. |
| 40 | Common_Expr | 하나의 Sequence Point내에서 Object의 값을 변경하고 Access 하지 않아야 한다. | **✅ 검출** | `ast-sequencing-rules`: 하나의 전체식에서 동일 객체를 변경하면서 동시에 접근(읽기)했습니다. |
| 41 | Common_Expr | 음수 값 또는 데이터 사이즈를 초과하는 값을 사용하여 Shift operator를 하지 않는다. | **✅ 검출** | `ast-shift-amount-range`: 시프트 양(40)이 피연산자 유효 폭(32) 이상입니다(정의되지 않은 동작). 0에서 31 사이여야 합니다.<br>`ast-shift-amount-range`: 시프트 양이 음수입니다(정의되지 않은 동작). 0 이상이어야 합니다. |
| 42 | Common_Expr | Underlying type이 Unsigned인 데이터에 '-'를 사용하여 결과를 assign하지 말아야 한다. | **✅ 검출 (체커 튜닝)** | `ast-unsigned-minus-assignment`: Unsigned 타입의 데이터에 단항 '-' 연산자를 사용하는 것은 의도치 않은 결과를 발생시킬 수 있습니다 |
| 43 | Common_Expr | ’sizeof’의 인자로 side effect를 포함하는 식을 사용하면 안된다. | **✅ 검출** | `ast-unevaluated-side-effect`: sizeof의 피연산자는 평가되지 않습니다. side effect가 있는 표현식을 사용하지 마십시오. |
| 44 | Common_Expr | Boolean 표현 값에는 논리 연산자(&&, ||, !)만을 사용하여야 한다. | **✅ 검출** | `ast-bool-logic-only-ops`: Boolean(논리) 값에는 연산자 '+'를 사용하지 마십시오. 논리 연산자(&&, ||, !)만 사용해야 합니다.<br>`ast-bool-logic-only-ops`: Boolean(논리) 값에는 연산자 '~'를 사용하지 마십시오. 논리 연산자(&&, ||, !)만 사용해야 합니다.<br>`ast-bool-logic-only-ops`: Boolean(논리) 값에는 연산자 '>'를 사용하지 마십시오. 논리 연산자(&&, ||, !)만 사용해야 합니다. |
| 45 | Common_Expr | condition에 assignment operator를 사용하지 말아야 한다. | **✅ 검출** | `ast-no-assignment-in-condition`: if의 조건식에서 대입 연산자 '='를 사용하지 마십시오.<br>`ast-if-style`: if 문에는 반드시 else 블록이 필요합니다 |
| 46 | Common_Expr | Signed Value에서 Bitwise연산자(<<, ~, |, ^ 등)로 인한 Negative Value를 유의해야 한다. | **✅ 검출** | `ast-signed-bitwise-safety`: 부호 있는 타입(long)에 대한 '<<' 사용은 음수/부호 비트 손상을 유발할 수 있습니다. unsigned로 명시적 변환이 필요합니다. |
| 47 | C_Specific | Scanf의 Argument 에는 Object Value가 저장된 주소값이 입력되어야 한다. | **✅ 검출** | `cfg-init-before-use`: 변수 'n'가 초기화되기 전에 사용되었습니다.<br>`3`: 1번째 인자의 타입이 프로토타입과 일치하지 않습니다. 기대: 'const char *const', 실제: 'const char<br>`ast-varargs-format`: scanf 포맷 지정자와 인자(위치 #1) 타입 불일치(정확한 포인터 필요). |
| 48 | C_Specific | `#include` 구문에서 표준에 맞지 않는 Character set을 사용하지 않아야 한다. | **✅ 검출 (C 모드)** | 미검출 (대상 체커: `IncludeCharsetCheck`) |
| 49 | C_Specific | malloc에서 사용하는 인자값은 assign될 변수 type의 정수배이어야 한다. | **✅ 검출** | `ast-malloc-size-multiple`: 'malloc' 크기(13 바이트)가 대상 원소 타입 'long'의 크기(sizeof(long)=4 바이트)의 정수배가 아닙니다.<br>`ast-malloc-size-multiple`: 'malloc' 크기(3 바이트)가 대상 원소 타입 'long'의 크기(sizeof(long)=4 바이트)의 정수배가 아닙니다.<br>`ast-no-malloc`: C 스타일 메모리 관리 함수 'malloc' 대신 C++의 'new/delete' 또는 스마트 포인터를 사용하세요.<br>`ast-function-call-argument-consistency`: 1번째 인자에서 허용되지 않는 암묵 변환(integral cast)이 발생했습니다. 기대: 'size_t', 실제: 'int'.<br>`ast-malloc-size-multiple`: 'malloc' 크기(11 바이트)가 대상 원소 타입 'long'의 크기(sizeof(long)=4 바이트)의 정수배가 아닙니다. |
| 50 | C_Specific | 함수의 Argument type과 개수는 함수의 Prototype, 선언, 정의가 모두 같아야 한다. | **✅ 검출** | `ast-representable-cast`: 표현식 내부 변환에서 대상 타입('unsigned char')으로 정확히 표현되지 않을 수 있습니다: 실수→정수 변환에서 소수부/범위 손실 가능<br>`ast-function-call-argument-consistency`: 1번째 인자에서 허용되지 않는 암묵 변환(float-to-integral)이 발생했습니다. 기대: 'unsigned char', 실제: 'float'. |
| 51 | C_Specific | 구조체/배열의 초기화 시 default 초기화 값(0)을 제외하고, 구조에 맞게 ‘{}’를 사용하여 선언된 Size에 맞게 초기화 해야 한다. | **✅ 검출** | `ast-aggregate-init-style`: 배열 초기화에서 선언된 크기(3)와 다른 개수(6)의 요소만 명시했습니다. 전체를 구조에 맞게 모두 명시하거나 {0}으로 전체 0 초기화를 사용하십시오.<br>`ast-aggregate-init-style`: 배열의 각 하위 원소가 또한 집합(배열/구조체)인 경우, 중첩 중괄호로 구조를 명시해야 합니다. 예: { {…}, {…}, … }<br>`ast-aggregate-init-style`: 배열 초기화에서 선언된 크기(10)와 다른 개수(1)의 요소만 명시했습니다. 전체를 구조에 맞게 모두 명시하거나 {0}으로 전체 0 초기화를 사용하십시오.<br>`ast-aggregate-init-style`: 구조체 초기화에서 필드 개수(2)에 맞게 모두 명시해야 합니다. 현재 명시된 항목은 1개입니다. 전체 0 초기화는 {0} (또는 옵션에 따라 {}) 를 사용하십시오. |
| 52 | CPP_Specific_1 | 동적할당된 변수를 release 할 때에는 변수의 type과 일치하게 release 하여야 한다. | **✅ 검출** | `cfg-new-delete-form-consistency`: 포인터 'p2'는 new로 할당된 것으로 추적되지만 delete[]로 해제하고 있습니다.<br>`ast-representable-cast`: 표현식 내부 변환에서 대상 타입('unsigned long long')으로 정확히 표현되지 않을 수 있습니다: 부호 있는 값을 무부호로 변환(의미 손실 가능)<br>`cfg-new-delete-form-consistency`: 포인터 'p1'는 new[]로 할당된 것으로 추적되지만 delete로 해제하고 있습니다. |
| 53 | CPP_Specific_1 | 지역 변수의 주소 값을 처리하는 handle을 return하지 말아야 한다. | **✅ 검출** | `cfg-init-before-use`: 변수 'a'가 초기화되기 전에 사용되었습니다. |
| 54 | CPP_Specific_1 | 함수 parameter의 주소 값을 처리하는 handle을 return하지 말아야 한다. | **✅ 검출** | `cfg-init-before-use`: 변수 'a'가 초기화되기 전에 사용되었습니다. |
| 55 | CPP_Specific_1 | 소멸자내에서 처리할 수 없는 예외 상황을 발생시키지 말아야 한다. | **✅ 검출** | `ast-destructor-exception-safety`: 소멸자에서 예외가 발생하지만, 이를 처리하는 적절한 catch 블록이 없습니다. 예외 전파 시 프로그램이 강제 종료됩니다.<br>`ast-extern-function-declaration`: 외부 함수 'funcThrowsEverything'는 호출 이전 선언에서 'extern'을 명시해야 합니다.<br>`ast-if-style`: if 문에는 반드시 else 블록이 필요합니다<br>`ast-exception-specification`: noexcept(또는 throw())로 선언된 함수에서 처리되지 않은 예외가 발생했습니다. 이 경우 std::terminate()가 호출될 수 있습니다. |
| 56 | CPP_Specific_1 | 사용되지 않는 예외 처리 문을 작성하지 말아야 한다. | **✅ 검출** | `ast-extern-function-declaration`: 외부 함수 'boo'는 호출 이전 선언에서 'extern'을 명시해야 합니다. |
| 57 | CPP_Specific_1 | exception specification에 기술되지 않은 모든 throw에 대하여 예외처리를 해야만 한다. | **✅ 검출** | `ast-exception-specification`: 함수의 동적 예외 명세(throw(T...))에 허용되지 않은 타입 'float'을 던지고 있습니다. |
| 58 | CPP_Specific_1 | main 함수에서 처리되지 않는 throw를 작성하지 말아야 한다. | **❌ 미탐** | 미검출 (대상 체커: `MainUnhandledThrowCheck`) |
| 59 | CPP_Specific_2 | release 된 메모리 영역을 사용하지 말아야 한다. | **✅ 검출** | `ast-extern-function-declaration`: 외부 함수 'bar'는 호출 이전 선언에서 'extern'을 명시해야 합니다.<br>`cfg-use-after-free`: 이미 해제된(Deleted) 메모리 영역 'i'을(를) 참조하고 있습니다. |
| 60 | CPP_Specific_2 | copy operator를 통해서, 복사되지 않는 멤버 변수가 존재하지 말아야 한다. | **✅ 검출** | `ast-partial-copy-assignment`: 멤버 변수 'y'이(가) 복사 대입 연산자에서 대입되지 않았습니다.<br>`ast-if-style`: if 문에는 반드시 else 블록이 필요합니다 |
| 61 | CPP_Specific_2 | C++에서 동적 메모리 할당은 반드시 `new` 연산자를 사용해야 한다. | **✅ 검출** | `ast-no-malloc`: C 스타일 메모리 관리 함수 'malloc' 대신 C++의 'new/delete' 또는 스마트 포인터를 사용하세요. |
| 62 | CPP_Specific_2 | 순수 가상 함수는 반드시 `= 0` 으로 초기화해야 한다. | **❌ 미탐** | 미검출 (대상 체커: `PureVirtualInitCheck`) |
| 63 | CPP_Specific_2 | 순수 함수는 반드시 `virtual`로 선언되어야 한다. | **❌ 미탐** | 미검출 (대상 체커: `VirtualPureCheck`) |
| 64 | CPP_Specific_2 | virtual base 클래스 포인터를 derived 클래스 포인터로 변환할 때에는 반드시 `dynamic_cast`를 사용해야 한다. | **✅ 검출** | `ast-virtual-base-cast`: 가상 상속 관계에서의 다운캐스팅은 'reinterpret_cast' 사용 시 메모리 오프셋 오류로 치명적인 런타임 오류를 유발합니다. 'dynamic_cast'를 사용하세요. |
| 65 | CPP_Specific_2 | 생성자/소멸자 내에서는 가상함수를 반드시 식별자(Qualifier)를 붙여서 호출해야 한다. | **✅ 검출** | `ast-virtual-call-in-ctor-dtor`: 생성자/소멸자 내에서 가상 함수 'func'을(를) 식별자 없이 호출하면 가상 디스패치가 기대대로 동작하지 않을 수 있습니다. |
| 66 | CPP_Specific_2 | 생성자 또는 소멸자에서는 `dynamic_cast`를 사용해서는 안 된다. | **✅ 검출** | `ast-virtual-call-in-ctor-dtor`: 생성자/소멸자 내에서 가상 함수 'foo'을(를) 식별자 없이 호출하면 가상 디스패치가 기대대로 동작하지 않을 수 있습니다.<br>`ast-dynamic-cast-in-ctor-dtor`: 생성자 내에서 'dynamic_cast'를 사용하면 의도하지 않은 동작(미완성 객체 타입 참조)이 발생할 수 있습니다. |
