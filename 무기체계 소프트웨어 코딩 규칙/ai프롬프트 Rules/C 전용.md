### [C전용-1] scanf 인자로 주소값 전달
- checkerName: VarargsFormatCheck
- category: C-Specific, IO
- severity: ERROR
- summary: "scanf 함수에는 값을 저장할 변수의 주소(&)를 전달해야 합니다."
- violation: `scanf("%d", n);`
- correction: `scanf("%d", &n);`

### [C전용-2] #include 파일명 표준 문자 사용
- checkerName: IncludeCharsetCheck
- category: C-Specific, Preprocessor
- severity: WARNING
- summary: "#include 구문에 사용되는 파일명에는 영문, 숫자, 밑줄(_)만 사용해야 합니다."
- violation: `#include "my'file.h"`
- correction: `#include "my_file.h"`

### [C전용-3] malloc 사이즈 정수배 할당
- checkerName: MallocSizeMultipleCheck
- category: C-Specific, Memory
- severity: MAJOR
- summary: "malloc으로 메모리 할당 시, 할당받을 타입 크기(sizeof(type))의 정수배로 크기를 지정해야 합니다."
- violation: `ptd = (long *) malloc(3);`
- correction: `ptd = (long *) malloc(sizeof(long) * 1);`

### [C전용-4] 함수 시그니처 일관성
- checkerName: FunctionSignatureConsistencyCheck, FunctionCallArgumentConsistencyCheck
- category: C-Specific, Function
- severity: ERROR
- summary: "함수의 선언, 정의, 호출 시 사용되는 인자의 타입과 개수는 모두 일치해야 합니다."
- violation: `foo(a);` ([translate:선언과 다른 타입/개수로 호출])
- correction: `foo(b);` ([translate:선언과 일치하는 타입/개수로 호출])

### [C전용-5] 집합체 명시적 초기화
- checkerName: AggregateInitStyleCheck
- category: C-Specific, Initialization
- severity: INFO
- summary: "구조체나 배열 초기화 시, 중괄호({})를 사용하여 모든 멤버를 명시적으로 초기화하는 것을 권장합니다."
- violation: `int arr[3][2] = {1, 2, 3, 4, 5, 6};`
- correction: `int arr[3][2] = {{1, 2}, {3, 4}, {5, 6}};`
