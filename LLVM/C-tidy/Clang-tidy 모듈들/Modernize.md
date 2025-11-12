아래는 Clang-Tidy Modernize 모듈의 체커들을 기능별로 분류한 도표입니다.[clang.llvm+2](https://clang.llvm.org/extra/clang-tidy/checks/list.html)​

|카테고리|체커 이름|설명|
|---|---|---|
|**C++11/14 언어 기능**|UseAutoCheck|타입 추론이 가능한 곳에서 `auto` 키워드 사용을 권장합니다[kdab](https://www.kdab.com/clang-tidy-part-1-modernize-source-code-using-c11c14/)​.|
||UseNullptrCheck|0이나 NULL 대신 `nullptr` 사용을 권장합니다[kdab](https://www.kdab.com/clang-tidy-part-1-modernize-source-code-using-c11c14/)​.|
||UseOverrideCheck|가상 함수 오버라이드 시 `override` 키워드 사용을 권장합니다[kdab](https://www.kdab.com/clang-tidy-part-1-modernize-source-code-using-c11c14/)​.|
||UseUsingCheck|`typedef` 대신 `using` 별칭 사용을 권장합니다[kdab](https://www.kdab.com/clang-tidy-part-1-modernize-source-code-using-c11c14/)​.|
||UseBoolLiteralsCheck|정수 리터럴 대신 `true`/`false` 사용을 권장합니다[kdab](https://www.kdab.com/clang-tidy-part-1-modernize-source-code-using-c11c14/)​.|
||UseNoexceptCheck|`throw()` 대신 `noexcept` 사용을 권장합니다[kdab](https://www.kdab.com/clang-tidy-part-1-modernize-source-code-using-c11c14/)​.|
||RawStringLiteralCheck|이스케이프 문자가 많은 문자열에 대해 raw string literal 사용을 권장합니다[kdab](https://www.kdab.com/clang-tidy-part-1-modernize-source-code-using-c11c14/)​.|
||UseTrailingReturnTypeCheck|후행 반환 타입(trailing return type) 사용을 권장합니다.|
||ReturnBracedInitListCheck|반환 시 중괄호 초기화 리스트 사용을 권장합니다.|
||UnaryStaticAssertCheck|단항 `static_assert` 사용을 권장합니다.|
|**C++17/20 기능**|UseDesignatedInitializersCheck|지정 초기화자(designated initializers) 사용을 권장합니다.|
||UseStdFormatCheck|`printf` 스타일 대신 `std::format` 사용을 권장합니다.|
||UseStdPrintCheck|`printf` 대신 `std::print` 사용을 권장합니다.|
||UseStdNumbersCheck|수학 상수에 대해 `std::numbers` 사용을 권장합니다.|
||UseStartsEndsWithCheck|문자열 prefix/suffix 체크에 `starts_with`/`ends_with` 사용을 권장합니다.|
|**C++20 Concepts/Ranges**|UseConstraintsCheck|템플릿 제약에 concepts 사용을 권장합니다.|
||UseRangesCheck|알고리즘에서 ranges 라이브러리 사용을 권장합니다.|
|**스마트 포인터**|MakeSharedCheck|`new`를 사용한 `shared_ptr` 생성 대신 `std::make_shared` 사용을 권장합니다[kdab](https://www.kdab.com/clang-tidy-part-1-modernize-source-code-using-c11c14/)​.|
||MakeUniqueCheck|`new`를 사용한 `unique_ptr` 생성 대신 `std::make_unique` 사용을 권장합니다[kdab](https://www.kdab.com/clang-tidy-part-1-modernize-source-code-using-c11c14/)​.|
||MakeSmartPtrCheck|스마트 포인터 생성에 `make_*` 함수 사용을 권장합니다.|
||ReplaceAutoPtrCheck|deprecated된 `auto_ptr` 대신 `unique_ptr` 사용을 권장합니다[kdab](https://www.kdab.com/clang-tidy-part-1-modernize-source-code-using-c11c14/)​.|
|**루프 현대화**|LoopConvertCheck|전통적인 for 루프를 range-based for 루프로 변환합니다[kdab+1](https://www.kdab.com/clang-tidy-part-1-modernize-source-code-using-c11c14/)​.|
||LoopConvertUtils|루프 변환 유틸리티 함수들입니다.|
|**초기화**|UseDefaultMemberInitCheck|클래스 멤버의 기본 초기화 사용을 권장합니다.|
||UseEqualsDefaultCheck|명시적 기본 구현 대신 `= default` 사용을 권장합니다[kdab](https://www.kdab.com/clang-tidy-part-1-modernize-source-code-using-c11c14/)​.|
||UseEqualsDeleteCheck|삭제된 함수에 `= delete` 사용을 권장합니다.|
|**컨테이너/알고리즘**|UseEmplaceCheck|`push_back` 대신 `emplace_back` 사용을 권장합니다[kdab](https://www.kdab.com/clang-tidy-part-1-modernize-source-code-using-c11c14/)​.|
||ShrinkToFitCheck|메모리 축소를 위해 `shrink_to_fit` 사용을 권장합니다[kdab](https://www.kdab.com/clang-tidy-part-1-modernize-source-code-using-c11c14/)​.|
||ReplaceRandomShuffleCheck|deprecated된 `random_shuffle` 대신 `shuffle` 사용을 권장합니다.|
||MinMaxUseInitializerListCheck|`min`/`max`에 initializer list 사용을 권장합니다.|
|**헤더/네임스페이스**|DeprecatedHeadersCheck|deprecated된 C 헤더(`<stdio.h>`) 대신 C++ 헤더(`<cstdio>`) 사용을 권장합니다[kdab+1](https://www.kdab.com/clang-tidy-part-1-modernize-source-code-using-c11c14/)​.|
||DeprecatedIosBaseAliasesCheck|deprecated된 `ios_base` 별칭 대신 새로운 이름 사용을 권장합니다.|
||ConcatNestedNamespacesCheck|중첩된 네임스페이스를 간결한 구문으로 병합합니다.|
|**매크로/enum**|MacroToEnumCheck|매크로를 `enum`으로 변환합니다.|
||ReplaceDisallowCopyAndAssignMacroCheck|복사 금지 매크로를 삭제된 함수로 교체합니다.|
|**함수/파라미터**|PassByValueCheck|복사 후 이동이 필요한 매개변수에 pass-by-value 사용을 권장합니다[kdab](https://www.kdab.com/clang-tidy-part-1-modernize-source-code-using-c11c14/)​.|
||RedundantVoidArgCheck|`void func(void)` 대신 `void func()` 사용을 권장합니다[kdab](https://www.kdab.com/clang-tidy-part-1-modernize-source-code-using-c11c14/)​.|
||AvoidBindCheck|`std::bind` 대신 람다 사용을 권장합니다.|
||AvoidCArraysCheck|C 스타일 배열 대신 `std::array`/`std::vector` 사용을 권장합니다.|
|**속성/어노테이션**|UseNodiscardCheck|반환값을 무시하면 안 되는 함수에 `[[nodiscard]]` 사용을 권장합니다.|
|**타입 traits**|TypeTraitsCheck|구식 type traits 대신 C++14 `_t` 별칭 사용을 권장합니다.|
|**투명 함수 객체**|UseTransparentFunctorsCheck|`std::less<>`와 같은 투명 함수 객체 사용을 권장합니다.|
|**예외 처리**|UseUncaughtExceptionsCheck|deprecated된 `uncaught_exception` 대신 `uncaught_exceptions` 사용을 권장합니다.|
|**리터럴/표현식**|IntegralLiteralExpressionMatcher|정수 리터럴 표현식 매칭 유틸리티입니다.|

