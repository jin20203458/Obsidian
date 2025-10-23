다음은 Clang-Tidy `bugprone` 모듈의 체커들을 기능별로 분류한 도표입니다.[clang.llvm+1](https://clang.llvm.org/extra/clang-tidy/checks/list.html)​

| 카테고리            | 체커 이름                                         | 설명                                                                    |
| --------------- | --------------------------------------------- | --------------------------------------------------------------------- |
| **메모리 및 포인터**   | `PointerArithmeticOnPolymorphicObjectCheck`   | 다형 객체에 대한 포인터 연산을 감지합니다.                                              |
|                 | `BoolPointerImplicitConversionCheck`          | `bool` 포인터의 암묵적 변환을 검사합니다.                                            |
|                 | `CastingThroughVoidCheck`                     | `void` 포인터를 통한 불안전한 캐스팅을 감지합니다.                                       |
|                 | `MultiLevelImplicitPointerConversionCheck`    | 다단계 암묵적 포인터 변환을 검사합니다. (`char**` -> `void**` 등)                       |
|                 | `DanglingHandleCheck`                         | 임시 객체를 참조하는 등 댕글링 핸들(참조)을 감지합니다.                                      |
|                 | `MultipleNewInOneExpressionCheck`             | 한 표현식 내 여러 `new` 연산자 사용 시 메모리 누수 가능성을 감지합니다.                          |
|                 | `SuspiciousMemoryComparisonCheck`             | `memcmp`로 패딩이 있는 구조체를 비교하는 등 의심스러운 메모리 비교를 검사합니다.                     |
|                 | `SuspiciousMemsetUsageCheck`                  | `memset`의 인자 순서 오류나 크기 오류 등 잘못된 사용을 검사합니다.                            |
|                 | `SuspiciousReallocUsageCheck`                 | `realloc` 실패 시 메모리 누수를 유발할 수 있는 사용 패턴을 검사합니다.                         |
|                 | `UndefinedMemoryManipulationCheck`            | `memcpy`/`memset` 등 non-trivial 타입에 대한 정의되지 않은 메모리 조작을 검사합니다.         |
|                 | `SharedPtrArrayMismatchCheck`                 | `shared_ptr<T>`에 배열을 할당하는 타입 불일치를 감지합니다.                              |
|                 | `SmartPtrArrayMismatchCheck`                  | 배열에 대한 스마트 포인터 사용 시 적절한 deleter가 없는 경우를 감지합니다.                        |
|                 | `UniquePtrArrayMismatchCheck`                 | `unique_ptr<T>`와 `unique_ptr<T[]>`의 혼용을 감지합니다.                        |
| **조건문 및 제어 흐름** | `AssignmentInIfConditionCheck`                | `if` 조건문 내 대입 연산 (`=`) 실수를 감지합니다.                                     |
|                 | `RedundantBranchConditionCheck`               | 이전 조건에 의해 결과가 정해지는 중복 분기 조건을 검사합니다.                                   |
|                 | `BranchCloneCheck`                            | `if-else`의 각 분기에서 코드가 중복되는 경우를 감지합니다.                                 |
|                 | `ChainedComparisonCheck`                      | `a < b < c` 와 같이 의도와 다르게 동작할 수 있는 연쇄 비교를 검사합니다.                       |
|                 | `SwitchMissingDefaultCaseCheck`               | `switch` 문에서 `default` 케이스 누락을 감지합니다.                                 |
|                 | `InfiniteLoopCheck`                           | 조건이 항상 참이 되어 발생하는 무한 루프 가능성을 감지합니다.                                   |
|                 | `TooSmallLoopVariableCheck`                   | 루프 범위에 비해 변수 타입이 작아 오버플로우가 발생할 수 있는 경우를 검사합니다.                        |
|                 | `IncDecInConditionsCheck`                     | 조건문 내 증감 연산자 사용으로 인한 혼동 가능성을 감지합니다.                                   |
|                 | `TerminatingContinueCheck`                    | 루프의 마지막에 위치하여 불필요한 `continue` 문을 검사합니다.                               |
| **타입 변환 및 캐스팅** | `ImplicitWideningOfMultiplicationResultCheck` | 곱셈 결과가 오버플로우된 후 더 큰 타입으로 암묵적 확장되는 경우를 검사합니다.                          |
|                 | `MisplacedWideningCastCheck`                  | 연산 전에 수행되어야 할 확장 캐스트가 연산 후에 위치한 경우를 감지합니다.                            |
|                 | `IntegerDivisionCheck`                        | 부동소수점 결과를 의도했을 수 있는 정수 나눗셈을 감지합니다.                                    |
|                 | `IncorrectRoundingsCheck`                     | 정수 연산을 이용한 부정확한 반올림 코드를 검사합니다.                                        |
|                 | `NonZeroEnumToBoolConversionCheck`            | `enum` 값을 `bool`로 변환 시 의도치 않은 동작 가능성을 검사합니다.                          |
|                 | `SignedCharMisuseCheck`                       | `signed char`를 문자 처리나 배열 인덱스로 사용할 때 발생하는 문제를 감지합니다.                   |
| **문자열**         | `StringConstructorCheck`                      | `std::string` 생성자의 위험한 사용 패턴을 감지합니다.                                  |
|                 | `StringIntegerAssignmentCheck`                | 문자열에 정수 값을 직접 대입하는 경우를 검사합니다.                                         |
|                 | `StringLiteralWithEmbeddedNulCheck`           | 문자열 리터럴 내부에 `\0` 문자가 포함된 경우를 감지합니다.                                   |
|                 | `StringviewNullptrCheck`                      | `std::string_view` 생성자에 `nullptr`를 전달하는 경우를 검사합니다.                    |
|                 | `SuspiciousStringviewDataUsageCheck`          | null 종결을 보장하지 않는 `string_view::data()`의 위험한 사용을 검사합니다.                |
|                 | `SuspiciousStringCompareCheck`                | `strcmp` 같은 함수의 반환 값을 `bool`로 잘못 사용하는 경우를 검사합니다.                      |
|                 | `NotNullTerminatedResultCheck`                | `strncpy` 등 null 종결을 보장하지 않는 함수 사용 후의 문제를 감지합니다.                      |
| **예외 및 에러 처리**  | `ExceptionEscapeCheck`                        | `noexcept` 함수나 소멸자에서 예외가 빠져나가는 경우를 검사합니다.                             |
|                 | `EmptyCatchCheck`                             | 예외를 무시하는 빈 `catch` 블록을 감지합니다.                                         |
|                 | `ThrowKeywordMissingCheck`                    | 예외를 다시 던질 때 `throw;` 대신 예외 객체를 복사하는 실수를 감지합니다.                        |
|                 | `UnhandledExceptionAtNewCheck`                | `new` 연산 시 발생하는 `bad_alloc` 예외를 처리하지 않는 경우를 검사합니다.                    |
|                 | `PosixReturnCheck`                            | POSIX 함수의 반환 값(에러 코드)을 확인하지 않는 경우를 감지합니다.                             |
| **함수 호출 및 인자**  | `ArgumentCommentCheck`                        | 함수 호출 시 주석으로 단 인자 이름과 실제 매개변수 이름의 불일치를 검사합니다.                         |
|                 | `SwappedArgumentsCheck`                       | 타입이 같아 구별이 어려운 인자들의 순서가 바뀐 경우를 감지합니다.                                 |
|                 | `EasilySwappableParametersCheck`              | 타입이 같아 쉽게 순서가 바뀔 수 있는 매개변수들을 감지합니다.                                   |
|                 | `SuspiciousMissingCommaCheck`                 | 초기화 리스트 등에서 쉼표가 누락된 경우를 감지합니다.                                        |
| **객체 지향 프로그래밍** | `CopyConstructorInitCheck`                    | 복사 생성자에서 멤버 초기화가 잘못된 경우를 검사합니다.                                       |
|                 | `UndelegatedConstructorCheck`                 | 생성자 위임 구문이 잘못 사용된 경우를 감지합니다.                                          |
|                 | `CrtpConstructorAccessibilityCheck`           | CRTP 패턴에서 기반 클래스 생성자의 접근성 문제를 검사합니다.                                  |
|                 | `VirtualNearMissCheck`                        | `override`를 의도했지만 시그니처가 미세하게 달라 오버라이드가 되지 않은 함수를 감지합니다.               |
|                 | `ParentVirtualCallCheck`                      | 생성자나 소멸자에서 가상 함수를 호출하는 경우를 감지합니다.                                     |
|                 | `ComparePointerToMemberVirtualFunctionCheck`  | 가상 멤버 함수에 대한 포인터 비교의 문제를 검사합니다.                                       |
|                 | `UseAfterMoveCheck`                           | `std::move`로 이동된 객체를 다시 사용하는 경우를 감지합니다.                               |
|                 | `MoveForwardingReferenceCheck`                | 전달 참조(forwarding reference)에 `std::move`를 잘못 사용하는 경우를 검사합니다.          |
|                 | `ForwardingReferenceOverloadCheck`            | 전달 참조 오버로드가 다른 생성자나 대입 연산자를 가리는 경우를 검사합니다.                            |
|                 | `UnhandledSelfAssignmentCheck`                | 대입 연산자에서 자기 자신에게 대입하는 경우를 처리하지 않은 코드를 감지합니다.                          |
| **매크로**         | `MacroParenthesesCheck`                       | 매크로 인자나 전체 정의에서 괄호가 누락되어 발생하는 연산자 우선순위 문제를 감지합니다.                     |
|                 | `MacroRepeatedSideEffectsCheck`               | 매크로 내에서 인자가 여러 번 평가되어 부작용이 반복 발생하는 경우를 감지합니다.                         |
|                 | `MultipleStatementMacroCheck`                 | `do-while(0)`으로 감싸지 않은 여러 문장으로 된 매크로를 검사합니다.                          |
| **연산자 및 표현식**   | `SizeofContainerCheck`                        | 컨테이너의 크기를 얻기 위해 `.size()` 대신 `sizeof`를 잘못 사용한 경우를 감지합니다.              |
|                 | `SizeofExpressionCheck`                       | 포인터에 `sizeof`를 사용하여 의도와 다른 크기를 얻는 경우를 감지합니다.                          |
|                 | `SuspiciousSemicolonCheck`                    | `if`, `for`, `while` 문 뒤에 의도치 않은 세미콜론이 있는 경우를 감지합니다.                  |
|                 | `StandaloneEmptyCheck`                        | 아무 동작도 하지 않는 불필요한 빈 문장(세미콜론)을 검사합니다.                                  |
|                 | `IncorrectEnableIfCheck`                      | `std::enable_if`의 잘못된 사용 패턴을 검사합니다.                                   |
| **리소스 관리**      | `UnusedRaiiCheck`                             | `lock_guard`와 같은 RAII 객체가 생성 직후 소멸되어 아무 효과가 없는 코드를 감지합니다.             |
|                 | `UnusedReturnValueCheck`                      | 에러 코드나 중요한 상태를 반환하는 함수의 반환 값을 사용하지 않는 경우를 감지합니다.                      |
|                 | `UnusedLocalNonTrivialVariableCheck`          | 생성 시 부작용만 있고 사용되지 않는 지역 변수를 검사합니다.                                    |
|                 | `InaccurateEraseCheck`                        | 컨테이너에서 `std::remove` 호출 후 `erase`를 호출하지 않아 원소가 실제로 삭제되지 않은 경우를 감지합니다. |
| **동시성 및 시그널**   | `BadSignalToKillThreadCheck`                  | 스레드를 종료하기 위해 부적절한 시그널을 사용하는 경우를 감지합니다.                                |
|                 | `SignalHandlerCheck`                          | 시그널 핸들러 내에서 안전하지 않은 함수를 호출하는 경우를 검사합니다.                               |
|                 | `SpuriouslyWakeUpFunctionsCheck`              | 조건 변수의 허위 깨어남(spurious wakeup)을 처리하지 않는 `wait` 함수 사용을 감지합니다.          |
| **기타**          | `AssertSideEffectCheck`                       | `assert` 매크로 내부에 부작용이 있는 코드를 넣어 릴리즈 빌드에서 코드가 사라지는 문제를 감지합니다.          |
|                 | `DynamicStaticInitializersCheck`              | 정적 객체의 초기화 순서에 의존하는 위험한 코드를 검사합니다.                                    |
|                 | `ForwardDeclarationNamespaceCheck`            | 전방 선언과 실제 정의의 네임스페이스가 다른 경우를 감지합니다.                                   |
|                 | `ReservedIdentifierCheck`                     | 표준에 의해 예약된 식별자(예: `_`로 시작하는 이름) 사용을 감지합니다.                            |
|                 | `UnsafeFunctionsCheck`                        | `gets`와 같이 버퍼 오버플로우에 취약한 안전하지 않은 함수 사용을 검사합니다.                        |

