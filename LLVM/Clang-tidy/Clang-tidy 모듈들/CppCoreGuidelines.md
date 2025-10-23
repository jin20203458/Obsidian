아래는 Clang-Tidy CppCoreGuidelines 모듈의 주요 체커들을 기능별로 분류한 도표입니다.[clang.llvm+1](https://clang.llvm.org/extra/clang-tidy/checks/list.html)​

| 카테고리                    | 체커 이름                                  | 설명                                                                                                                                        |
| ----------------------- | -------------------------------------- | ----------------------------------------------------------------------------------------------------------------------------------------- |
| **코루틴/람다**              | AvoidCapturingLambdaCoroutinesCheck    | 람다에서 코루틴 캡처를 피하도록 권장합니다.                                                                                                                  |
|                         | AvoidReferenceCoroutineParametersCheck | 코루틴 파라미터로 참조 타입 사용을 피하도록 권장합니다.                                                                                                           |
| **const/ref 멤버/글로벌 변수** | AvoidConstOrRefDataMembersCheck        | const 또는 참조 멤버 사용을 피하도록 권장합니다.                                                                                                            |
|                         | AvoidNonConstGlobalVariablesCheck      | non-const 글로벌 변수 사용을 피하도록 권장합니다.                                                                                                          |
| **제어 흐름**               | AvoidDoWhileCheck                      | do-while 루프 사용을 피하도록 권장합니다.                                                                                                               |
|                         | AvoidGotoCheck                         | goto 문 사용을 피하도록 권장합니다.                                                                                                                    |
| **초기화/멤버 초기화**          | InitVariablesCheck                     | 변수 초기화를 권장합니다.                                                                                                                            |
|                         | PreferMemberInitializerCheck           | 생성자에서 멤버 초기화를 권장합니다.                                                                                                                      |
|                         | ProTypeMemberInitCheck                 | 멤버 변수의 초기화를 권장합니다.                                                                                                                        |
| **매크로**                 | MacroUsageCheck                        | 매크로 사용을 피하도록 권장합니다.                                                                                                                       |
| **캡처/forward/move**     | MisleadingCaptureDefaultByValueCheck   | 람다 캡처 기본값(by value)의 오해를 방지합니다.                                                                                                           |
|                         | MissingStdForwardCheck                 | 전달 참조에서 std::forward 누락을 감지합니다.                                                                                                           |
|                         | RvalueReferenceParamNotMovedCheck      | rvalue 참조 파라미터가 move되지 않은 경우를 감지합니다.                                                                                                      |
| **타입 변환/캐스팅**           | NarrowingConversionsCheck              | narrowing 변환(데이터 손실 가능성)을 감지합니다.                                                                                                          |
|                         | ProTypeConstCastCheck                  | const_cast 사용을 피하도록 권장합니다.                                                                                                                |
|                         | ProTypeCstyleCastCheck                 | C 스타일 캐스팅 사용을 피하도록 권장합니다.                                                                                                                 |
|                         | ProTypeReinterpretCastCheck            | reinterpret_cast 사용을 피하도록 권장합니다.                                                                                                          |
|                         | ProTypeStaticCastDowncastCheck         | static_cast로 다운캐스팅을 피하도록 권장합니다.                                                                                                           |
|                         | ProTypeUnionAccessCheck                | union 멤버 접근의 안전성을 검사합니다.                                                                                                                  |
| **vararg/malloc/메모리**   | ProTypeVarargCheck                     | 가변 인자(vararg) 함수 사용을 피하도록 권장합니다.                                                                                                          |
|                         | NoMallocCheck                          | malloc/free 등 C 스타일 메모리 할당을 피하도록 권장합니다.                                                                                                   |
|                         | OwningMemoryCheck                      | 소유권이 명확하지 않은 메모리 사용을 감지합니다.                                                                                                               |
| **suspend/lock**        | NoSuspendWithLockCheck                 | lock이 걸린 상태에서 suspend를 피하도록 권장합니다.                                                                                                        |
| **배열/포인터**              | ProBoundsArrayToPointerDecayCheck      | 배열이 포인터로 decay되는 경우를 감지합니다.                                                                                                               |
|                         | ProBoundsConstantArrayIndexCheck       | 상수 배열 인덱스 사용을 검사합니다.                                                                                                                      |
|                         | ProBoundsPointerArithmeticCheck        | 포인터 산술 연산을 피하도록 권장합니다.                                                                                                                    |
| **슬라이싱/특수 멤버**          | SlicingCheck                           | 파생 클래스 객체를 기반 클래스 객체로 복사할 때 발생하는 슬라이싱을 감지합니다[clang.llvm](https://clang.llvm.org/extra/clang-tidy/checks/cppcoreguidelines/slicing.html)​. |
|                         | SpecialMemberFunctionsCheck            | 복사/이동 생성자, 소멸자 등 특수 멤버 함수 구현을 권장합니다.                                                                                                      |
| **인터페이스/글로벌 초기화**       | InterfacesGlobalInitCheck              | 인터페이스의 글로벌 초기화 문제를 감지합니다.                                                                                                                 |
| **가상 소멸자**              | VirtualClassDestructorCheck            | 가상 소멸자 구현을 권장합니다.                                                                                                                         |

1. [https://www.reddit.com/r/embedded/comments/yd4ahb/clangtidy_which_check_flags_you_typically_use/](https://www.reddit.com/r/embedded/comments/yd4ahb/clangtidy_which_check_flags_you_typically_use/)
2. [https://learn.microsoft.com/ko-kr/cpp/code-quality/clang-tidy?view=msvc-170](https://learn.microsoft.com/ko-kr/cpp/code-quality/clang-tidy?view=msvc-170)
3. [https://www.reddit.com/r/cpp_questions/comments/1b2223f/clangtidy_with_stdformat_giving_error_use_of/](https://www.reddit.com/r/cpp_questions/comments/1b2223f/clangtidy_with_stdformat_giving_error_use_of/)
4. [https://clang.llvm.org/extra/clang-tidy/checks/list.html](https://clang.llvm.org/extra/clang-tidy/checks/list.html)
5. [https://the-developer.de/c++/cpp_core_guidelines_checker_setup/](https://the-developer.de/c++/cpp_core_guidelines_checker_setup/)
6. [https://zenn.dev/dameyodamedame/articles/de20cdf00efc9a](https://zenn.dev/dameyodamedame/articles/de20cdf00efc9a)
7. [https://clang.llvm.org/extra/clang-tidy/](https://clang.llvm.org/extra/clang-tidy/)
8. [https://makurisgame190817.tistory.com/11](https://makurisgame190817.tistory.com/11)
9. [https://learn.microsoft.com/ko-kr/shows/pure-virtual-cpp-2022/clang-tidy-in-visual-studio-code](https://learn.microsoft.com/ko-kr/shows/pure-virtual-cpp-2022/clang-tidy-in-visual-studio-code)
10. [https://clang.llvm.org/extra/clang-tidy/checks/cppcoreguidelines/slicing.html](https://clang.llvm.org/extra/clang-tidy/checks/cppcoreguidelines/slicing.html)