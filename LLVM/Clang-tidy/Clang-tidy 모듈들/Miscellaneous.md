아래는 Clang-Tidy Misc(Miscellaneous) 모듈의 체커들을 기능별로 분류한 도표입니다.[clang.llvm+2](https://clang.llvm.org/extra/clang-tidy/checks/list.html)​

|카테고리|체커 이름|설명|
|---|---|---|
|**const 정확성**|ConstCorrectnessCheck|const로 선언할 수 있는 지역 변수를 감지합니다[rocm.amd+1](https://rocm.docs.amd.com/projects/llvm-project/en/latest/LLVM/clang-tools/html/clang-tidy/checks/misc/const-correctness.html)​.|
||MisplacedConstCheck|잘못 배치된 const 키워드를 감지합니다[clang.llvm](https://clang.llvm.org/extra/clang-tidy/checks/list.html)​.|
|**헤더 파일**|DefinitionsInHeadersCheck|헤더 파일에 정의가 있는 경우를 감지합니다[clang.llvm+1](https://clang.llvm.org/extra/clang-tidy/checks/list.html)​.|
||HeaderIncludeCycleCheck|헤더 파일의 순환 include를 감지합니다[clang.llvm](https://clang.llvm.org/extra/clang-tidy/checks/list.html)​.|
||IncludeCleanerCheck|사용되지 않거나 누락된 include를 검사합니다[clang.llvm+1](https://clang.llvm.org/extra/clang-tidy/checks/misc/include-cleaner.html)​.|
|**식별자 혼동**|ConfusableIdentifierCheck|시각적으로 혼동될 수 있는 식별자를 감지합니다[clang.llvm](https://clang.llvm.org/extra/clang-tidy/checks/list.html)​.|
||MisleadingBidirectional|양방향 유니코드 문자로 인한 오해를 감지합니다[clang.llvm](https://clang.llvm.org/extra/clang-tidy/checks/list.html)​.|
||MisleadingIdentifier|오해를 유발하는 식별자를 감지합니다[clang.llvm](https://clang.llvm.org/extra/clang-tidy/checks/list.html)​.|
|**코루틴**|CoroutineHostileRAIICheck|코루틴과 호환되지 않는 RAII 객체 사용을 감지합니다[clang.llvm](https://clang.llvm.org/extra/clang-tidy/checks/list.html)​.|
|**재귀**|NoRecursionCheck|재귀 함수 호출을 감지합니다[clang.llvm](https://clang.llvm.org/extra/clang-tidy/checks/list.html)​.|
|**new/delete**|NewDeleteOverloadsCheck|new/delete 연산자 오버로드의 문제를 검사합니다[clang.llvm](https://clang.llvm.org/extra/clang-tidy/checks/list.html)​.|
|**복사 불가 객체**|NonCopyableObjects|복사할 수 없는 객체의 복사 시도를 감지합니다[clang.llvm](https://clang.llvm.org/extra/clang-tidy/checks/list.html)​.|
|**클래스 멤버**|NonPrivateMemberVariablesInClassesCheck|클래스에서 private이 아닌 멤버 변수를 감지합니다[clang.llvm](https://clang.llvm.org/extra/clang-tidy/checks/list.html)​.|
|**중복 표현식**|RedundantExpressionCheck|중복되거나 불필요한 표현식을 감지합니다[clang.llvm](https://clang.llvm.org/extra/clang-tidy/checks/list.html)​.|
|**static_assert**|StaticAssertCheck|런타임 assert 대신 static_assert 사용을 권장합니다[clang.llvm](https://clang.llvm.org/extra/clang-tidy/checks/list.html)​.|
|**예외 처리**|ThrowByValueCatchByReferenceCheck|예외를 값으로 던지고 참조로 받는 관행을 검사합니다[clang.llvm](https://clang.llvm.org/extra/clang-tidy/checks/list.html)​.|
|**연산자 오버로딩**|UnconventionalAssignOperatorCheck|비표준적인 대입 연산자 구현을 감지합니다[clang.llvm](https://clang.llvm.org/extra/clang-tidy/checks/list.html)​.|
|**unique_ptr**|UniqueptrResetReleaseCheck|unique_ptr의 reset(release()) 패턴을 감지합니다[clang.llvm](https://clang.llvm.org/extra/clang-tidy/checks/list.html)​.|
|**사용되지 않는 선언**|UnusedAliasDeclsCheck|사용되지 않는 별칭 선언을 감지합니다[clang.llvm](https://clang.llvm.org/extra/clang-tidy/checks/list.html)​.|
||UnusedParametersCheck|사용되지 않는 함수 매개변수를 감지합니다[clang.llvm](https://clang.llvm.org/extra/clang-tidy/checks/list.html)​.|
||UnusedUsingDeclsCheck|사용되지 않는 using 선언을 감지합니다[clang.llvm](https://clang.llvm.org/extra/clang-tidy/checks/list.html)​.|
|**익명 네임스페이스**|UseAnonymousNamespaceCheck|파일 범위 static 대신 익명 네임스페이스 사용을 권장합니다[clang.llvm](https://clang.llvm.org/extra/clang-tidy/checks/list.html)​.|
|**내부 링크**|UseInternalLinkageCheck|내부 링크 사용을 권장합니다[clang.llvm](https://clang.llvm.org/extra/clang-tidy/checks/list.html)​.|

이 모듈은 다른 카테고리에 맞지 않는 다양한 검사들을 포함하고 있으며, const 정확성, 헤더 파일 관리, 식별자 명명 문제, 코루틴 안전성 등 여러 측면의 코드 품질을 개선하는 데 도움을 줍니다.[clang.llvm+2](https://clang.llvm.org/extra/clang-tidy/checks/misc/include-cleaner.html)​

