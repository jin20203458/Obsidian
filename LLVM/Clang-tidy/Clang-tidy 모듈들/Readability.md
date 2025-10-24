아래는 Clang-Tidy Readability 모듈의 체커들을 기능별로 분류한 도표입니다.[clang.llvm+2](https://clang.llvm.org/extra/clang-tidy/checks/list.html)​

|카테고리|체커 이름|설명|
|---|---|---|
|**함수 복잡도**|FunctionCognitiveComplexityCheck|함수의 인지적 복잡도를 측정하여 유지보수성을 평가합니다[ittrip](https://ko.ittrip.xyz/c-plus-plus/clang-tidy-cpp-style-bug-prevention)​.|
||FunctionSizeCheck|함수의 크기(라인 수, 분기 수 등)를 검사합니다[clang.llvm](https://clang.llvm.org/extra/clang-tidy/checks/list.html)​.|
|**명명 규칙**|IdentifierNamingCheck|변수, 함수, 클래스 등의 명명 규칙을 검사합니다[sarcasm.github+1](https://sarcasm.github.io/notes/dev/clang-tidy.html)​.|
||IdentifierLengthCheck|식별자 이름의 길이를 검사합니다[clang.llvm](https://clang.llvm.org/extra/clang-tidy/checks/list.html)​.|
||NamedParameterCheck|함수 선언에서 매개변수 이름을 명시하도록 권장합니다[clang.llvm](https://clang.llvm.org/extra/clang-tidy/checks/list.html)​.|
||InconsistentDeclarationParameterNameCheck|함수 선언과 정의에서 매개변수 이름의 불일치를 감지합니다[clang.llvm](https://clang.llvm.org/extra/clang-tidy/checks/list.html)​.|
|**중괄호 및 들여쓰기**|BracesAroundStatementsCheck|조건문이나 루프에서 중괄호 사용을 권장합니다[ittrip](https://ko.ittrip.xyz/c-plus-plus/clang-tidy-cpp-style-bug-prevention)​.|
||MisleadingIndentationCheck|오해를 유발하는 들여쓰기를 감지합니다[clang.llvm](https://clang.llvm.org/extra/clang-tidy/checks/list.html)​.|
|**중복 코드**|RedundantAccessSpecifiersCheck|중복된 접근 지정자를 감지합니다[clang.llvm](https://clang.llvm.org/extra/clang-tidy/checks/list.html)​.|
||RedundantCastingCheck|불필요한 캐스팅을 감지합니다[clang.llvm](https://clang.llvm.org/extra/clang-tidy/checks/list.html)​.|
||RedundantControlFlowCheck|불필요한 제어 흐름(return, continue 등)을 감지합니다[clang.llvm](https://clang.llvm.org/extra/clang-tidy/checks/list.html)​.|
||RedundantDeclarationCheck|중복된 선언을 감지합니다[clang.llvm](https://clang.llvm.org/extra/clang-tidy/checks/list.html)​.|
||RedundantFunctionPtrDereferenceCheck|불필요한 함수 포인터 역참조를 감지합니다[clang.llvm](https://clang.llvm.org/extra/clang-tidy/checks/list.html)​.|
||RedundantMemberInitCheck|불필요한 멤버 초기화를 감지합니다[clang.llvm](https://clang.llvm.org/extra/clang-tidy/checks/list.html)​.|
||RedundantPreprocessorCheck|불필요한 전처리기 지시문을 감지합니다[clang.llvm](https://clang.llvm.org/extra/clang-tidy/checks/list.html)​.|
||RedundantSmartptrGetCheck|스마트 포인터에서 불필요한 `.get()` 호출을 감지합니다[clang.llvm](https://clang.llvm.org/extra/clang-tidy/checks/list.html)​.|
||RedundantStringCStrCheck|문자열에서 불필요한 `.c_str()` 호출을 감지합니다[clang.llvm](https://clang.llvm.org/extra/clang-tidy/checks/list.html)​.|
||RedundantStringInitCheck|불필요한 문자열 초기화를 감지합니다[clang.llvm](https://clang.llvm.org/extra/clang-tidy/checks/list.html)​.|
||RedundantInlineSpecifierCheck|불필요한 inline 지정자를 감지합니다[clang.llvm](https://clang.llvm.org/extra/clang-tidy/checks/list.html)​.|
||DuplicateIncludeCheck|중복된 include를 감지합니다[clang.llvm](https://clang.llvm.org/extra/clang-tidy/checks/list.html)​.|
|**const 사용**|AvoidConstParamsInDecls|함수 선언에서 const 값 매개변수를 피하도록 권장합니다[clang.llvm](https://clang.llvm.org/extra/clang-tidy/checks/list.html)​.|
||ConstReturnTypeCheck|const 반환 타입을 검사합니다[clang.llvm](https://clang.llvm.org/extra/clang-tidy/checks/list.html)​.|
||NonConstParameterCheck|const로 선언할 수 있는 매개변수를 감지합니다[clang.llvm](https://clang.llvm.org/extra/clang-tidy/checks/list.html)​.|
||MakeMemberFunctionConstCheck|const로 선언할 수 있는 멤버 함수를 감지합니다[clang.llvm](https://clang.llvm.org/extra/clang-tidy/checks/list.html)​.|
|**컨테이너 사용**|ContainerContainsCheck|컨테이너의 contains() 메서드 사용을 권장합니다[clang.llvm](https://clang.llvm.org/extra/clang-tidy/checks/list.html)​.|
||ContainerDataPointerCheck|컨테이너의 data() 포인터 사용을 검사합니다[clang.llvm](https://clang.llvm.org/extra/clang-tidy/checks/list.html)​.|
||ContainerSizeEmptyCheck|`size() == 0` 대신 `empty()` 사용을 권장합니다[clang.llvm](https://clang.llvm.org/extra/clang-tidy/checks/list.html)​.|
|**제어 흐름**|ElseAfterReturnCheck|return 후 불필요한 else를 감지합니다[clang.llvm](https://clang.llvm.org/extra/clang-tidy/checks/list.html)​.|
||AvoidNestedConditionalOperatorCheck|중첩된 삼항 연산자를 피하도록 권장합니다[clang.llvm](https://clang.llvm.org/extra/clang-tidy/checks/list.html)​.|
||AvoidUnconditionalPreprocessorIfCheck|무조건적인 전처리기 if를 피하도록 권장합니다[clang.llvm](https://clang.llvm.org/extra/clang-tidy/checks/list.html)​.|
|**불린 표현식**|SimplifyBooleanExprCheck|복잡한 불린 표현식을 단순화합니다[clang.llvm](https://clang.llvm.org/extra/clang-tidy/checks/list.html)​.|
||ImplicitBoolConversionCheck|암묵적 bool 변환을 감지합니다[clang.llvm](https://clang.llvm.org/extra/clang-tidy/checks/list.html)​.|
|**static 사용**|ConvertMemberFunctionsToStatic|static으로 변환할 수 있는 멤버 함수를 감지합니다[clang.llvm](https://clang.llvm.org/extra/clang-tidy/checks/list.html)​.|
||StaticAccessedThroughInstanceCheck|인스턴스를 통한 static 멤버 접근을 감지합니다[clang.llvm](https://clang.llvm.org/extra/clang-tidy/checks/list.html)​.|
||StaticDefinitionInAnonymousNamespaceCheck|익명 네임스페이스 내 불필요한 static을 감지합니다[clang.llvm](https://clang.llvm.org/extra/clang-tidy/checks/list.html)​.|
|**문자열 비교**|StringCompareCheck|문자열 비교를 개선할 수 있는 경우를 감지합니다[clang.llvm](https://clang.llvm.org/extra/clang-tidy/checks/list.html)​.|
|**매직 넘버**|MagicNumbersCheck|하드코딩된 숫자 리터럴(매직 넘버)을 감지합니다[clang.llvm](https://clang.llvm.org/extra/clang-tidy/checks/list.html)​.|
||UppercaseLiteralSuffixCheck|리터럴 접미사를 대문자로 사용하도록 권장합니다[clang.llvm](https://clang.llvm.org/extra/clang-tidy/checks/list.html)​.|
|**포인터/참조**|DeleteNullPointerCheck|null 포인터 삭제를 검사합니다[clang.llvm](https://clang.llvm.org/extra/clang-tidy/checks/list.html)​.|
||ReferenceToConstructedTemporaryCheck|임시 객체에 대한 참조를 감지합니다[clang.llvm](https://clang.llvm.org/extra/clang-tidy/checks/list.html)​.|
||UniqueptrDeleteReleaseCheck|`unique_ptr`의 `delete release()` 패턴을 감지합니다[clang.llvm](https://clang.llvm.org/extra/clang-tidy/checks/list.html)​.|
|**배열/첨자**|MisplacedArrayIndexCheck|잘못 배치된 배열 인덱스를 감지합니다[clang.llvm](https://clang.llvm.org/extra/clang-tidy/checks/list.html)​.|
||SimplifySubscriptExprCheck|첨자 표현식을 단순화합니다[clang.llvm](https://clang.llvm.org/extra/clang-tidy/checks/list.html)​.|
|**선언 스타일**|IsolateDeclarationCheck|한 줄에 여러 변수 선언을 분리하도록 권장합니다[clang.llvm+1](https://clang.llvm.org/extra/clang-tidy/checks/list.html)​.|
|**수학 표현식**|MathMissingParenthesesCheck|수학 표현식에서 누락된 괄호를 감지합니다[clang.llvm](https://clang.llvm.org/extra/clang-tidy/checks/list.html)​.|
||UseStdMinMaxCheck|`std::min`/`std::max` 사용을 권장합니다[clang.llvm](https://clang.llvm.org/extra/clang-tidy/checks/list.html)​.|
|**알고리즘**|UseAnyOfAllOfCheck|`std::any_of`/`std::all_of` 사용을 권장합니다[clang.llvm](https://clang.llvm.org/extra/clang-tidy/checks/list.html)​.|
|**주석**|NamespaceCommentCheck|네임스페이스 종료 지점에 주석을 권장합니다[clang.llvm](https://clang.llvm.org/extra/clang-tidy/checks/list.html)​.|
|**enum**|EnumInitialValueCheck|enum 초기값 사용을 검사합니다[clang.llvm](https://clang.llvm.org/extra/clang-tidy/checks/list.html)​.|
|**auto 사용**|QualifiedAutoCheck|auto 사용 시 한정자(const, pointer 등)를 명시하도록 권장합니다[clang.llvm](https://clang.llvm.org/extra/clang-tidy/checks/list.html)​.|
|**연산자**|OperatorsRepresentationCheck|연산자의 표현 방식을 검사합니다[clang.llvm](https://clang.llvm.org/extra/clang-tidy/checks/list.html)​.|
|**함수 인자**|SuspiciousCallArgumentCheck|의심스러운 함수 호출 인자를 감지합니다[clang.llvm](https://clang.llvm.org/extra/clang-tidy/checks/list.html)​.|
|**return 스타일**|AvoidReturnWithVoidValueCheck|void 값과 함께 return을 피하도록 권장합니다[clang.llvm](https://clang.llvm.org/extra/clang-tidy/checks/list.html)​.|

이 모듈의 체커들은 코드의 가독성과 유지보수성을 향상시키는 데 중점을 두며, 일관된 코딩 스타일, 명확한 표현, 불필요한 코드 제거 등을 권장합니다.[sarcasm.github+2](https://sarcasm.github.io/notes/dev/clang-tidy.html)​
