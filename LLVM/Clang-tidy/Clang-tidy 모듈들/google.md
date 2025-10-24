아래는 Clang-Tidy Google 모듈의 체커들을 기능별로 분류한 도표입니다.[clang.llvm+1](https://clang.llvm.org/extra/clang-tidy/checks/list.html)​

|카테고리|체커 이름|설명|
|---|---|---|
|**캐스팅/예외**|AvoidCStyleCastsCheck|C 스타일 캐스팅 사용을 감지합니다. Google 스타일 가이드에서는 C++ 캐스팅을 권장합니다.|
||AvoidNSObjectNewCheck|Objective-C에서 `NSObject::new` 사용을 감지합니다.|
||AvoidThrowingObjCExceptionCheck|Objective-C 예외 던지기를 감지합니다.|
|**테스트/명명 규칙**|AvoidUnderscoreInGoogletestNameCheck|GoogleTest 테스트 이름에 언더스코어 사용을 감지합니다.|
||UpgradeGoogletestCaseCheck|구식 GoogleTest 케이스를 최신 스타일로 업그레이드합니다.|
||FunctionNamingCheck|함수 명명 규칙을 검사합니다. Google 스타일 가이드에 맞는 이름을 권장합니다.|
|**기본 인자/생성자**|DefaultArgumentsCheck|함수의 기본 인자 사용을 감지합니다.|
||ExplicitConstructorCheck|암묵적 변환을 방지하기 위해 생성자에 `explicit` 지정자를 권장합니다.|
||ExplicitMakePairCheck|`std::make_pair` 호출 시 타입 명시를 권장합니다.|
|**글로벌 네임스페이스/변수**|GlobalNamesInHeadersCheck|헤더 파일에서 글로벌 네임스페이스 오염을 감지합니다[clang.llvm](https://clang.llvm.org/extra/clang-tidy/checks/google/global-names-in-headers.html)​.|
||GlobalVariableDeclarationCheck|글로벌 변수 선언을 감지합니다.|
||UnnamedNamespaceInHeaderCheck|헤더 파일에서 익명 네임스페이스 사용을 감지합니다.|
||UsingNamespaceDirectiveCheck|`using namespace` 지시문 사용을 감지합니다.|
|**타입/연산자**|IntegerTypesCheck|Google 스타일에 맞는 정수 타입 사용을 권장합니다.|
||OverloadedUnaryAndCheck|단항 `&` 연산자 오버로딩을 감지합니다.|
|**TODO 주석**|TodoCommentCheck|TODO 주석을 감지합니다. Google 스타일에서는 TODO 주석에 작성자와 날짜를 명시하도록 권장합니다.|

이 모듈은 Google C++ 스타일 가이드에 맞춰 코드 품질, 일관성, 안전성을 유지하도록 돕는 체커들을 제공합니다. 네임스페이스 오염, 명명 규칙, 캐스팅, 테스트 코드 등 Google의 권장 패턴을 자동으로 점검합니다.[clang.llvm+1](https://clang.llvm.org/extra/clang-tidy/checks/google/global-names-in-headers.html)​

1. [https://developer.android.com/studio/releases/past-releases/as-3-3-0-release-notes?hl=ko](https://developer.android.com/studio/releases/past-releases/as-3-3-0-release-notes?hl=ko)
2. [https://learn.microsoft.com/ko-kr/cpp/code-quality/clang-tidy?view=msvc-170](https://learn.microsoft.com/ko-kr/cpp/code-quality/clang-tidy?view=msvc-170)
3. [https://clang.llvm.org/extra/clang-tidy/checks/list.html](https://clang.llvm.org/extra/clang-tidy/checks/list.html)
4. [https://makurisgame190817.tistory.com/11](https://makurisgame190817.tistory.com/11)
5. [https://blog.csdn.net/Edidaughter/article/details/143360227](https://blog.csdn.net/Edidaughter/article/details/143360227)
6. [https://gjbex.github.io/Defensive_programming_and_debugging/CodeValidation/StaticCodeAnalyzers/clang_tidy/](https://gjbex.github.io/Defensive_programming_and_debugging/CodeValidation/StaticCodeAnalyzers/clang_tidy/)
7. [https://nx006.tistory.com/37](https://nx006.tistory.com/37)
8. [https://clang.llvm.org/extra/clang-tidy/checks/google/global-names-in-headers.html](https://clang.llvm.org/extra/clang-tidy/checks/google/global-names-in-headers.html)
9. [https://www.reddit.com/r/embedded/comments/yd4ahb/clangtidy_which_check_flags_you_typically_use/](https://www.reddit.com/r/embedded/comments/yd4ahb/clangtidy_which_check_flags_you_typically_use/)
10. [https://github.com/woojung3/custom-clang-tidy-checks](https://github.com/woojung3/custom-clang-tidy-checks)