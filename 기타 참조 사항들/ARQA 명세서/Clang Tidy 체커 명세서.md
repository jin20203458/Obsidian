## 1. 렉서 및 전처리기 기반 체커 (`lex-` 접두사)

코드의 텍스트 구성 성분(Token), 주석, 헤더 포함 관계 및 매크로 전처리 단계를 분석하는 체커군입니다.

### IncludeCharsetCheck

- **등록명:** `lex-include-charset`
    
- **목적:** `#include` 지시문에 명시된 헤더 파일 경로가 안전하고 이식 가능한 문자 집합을 사용하는지, 경로 길이 및 슬래시 표기가 적절한지 검증합니다.
    
- **옵션:**
    
    - `AllowHyphen` (bool): 하이픈 허용 여부
        
    - `AllowUnicode` (bool): 비ASCII 문자 허용 여부
        
    - `AllowSpaces` (bool): 공백 허용 여부
        
    - `AllowParentPath` (bool): 부모 경로(`..`) 허용 여부
        
    - `AllowDoubleSep` (bool): 연속 구분자(`//`) 허용 여부
        
    - `AllowAbsolutePath` (bool): 절대 경로 허용 여부
        
    - `MaxPathLength` (unsigned): 허용되는 경로의 최대 길이
        
    - `EnforceForwardSlashOnly` (bool): 백슬래시(`\`) 사용을 금지하고 슬래시(`/`)만 사용하도록 강제할지 여부
        

### MacroDefinedBeforeUseCheck

- **등록명:** `lex-macro-defined-before-use`
    
- **목적:** 전처리 조건식(`#if`, `#elif`) 내부에서 정의되지 않은 매크로가 조건 판별식에 사용되는 오류를 찾아 경고합니다.
    
- **옵션:**
    
    - `WarnOnUndefined` (bool): 정의되지 않은 매크로 발견 시 경고 여부
        
    - `IgnoreBuiltinMacros` (bool): 컴파일러 내장 매크로(`__GNUC__`, `__FILE__` 등)를 검사에서 무시할지 여부
        

### SuspiciousCommentCheck

- **등록명:** `lex-suspicious-comment`
    
- **목적:** 주석(`//`, `/* */`) 내부에 비밀번호, API 키, 토큰 등 하드코딩된 민감한 자격 증명(Credential) 정보가 텍스트 형태로 잔존하는지 정규표현식 매칭으로 탐지합니다. (CWE 관련)
    
- **옵션:** 고유 옵션은 제공되지 않습니다.
    

## 2. AST 기반 체커 (`ast-` 접두사)

컴파일러가 생성한 구문 트리(Abstract Syntax Tree)의 구조와 타입 정보를 바탕으로 논리 오류 및 보안 취약점을 정적 분석하는 가장 방대한 체커군입니다.

### AggregateInitStyleCheck

- **등록명:** `ast-aggregate-init-style`
    
- **목적:** 구조체 및 배열 초기화 스타일 강제. 전체 0 초기화 시 `{0}` 또는 `{}`만 허용하며, 그 외에는 필드/요소 개수에 맞춰 명시적 초기화를 강제합니다. 다차원 배열은 중첩 중괄호를 요구합니다.
    
- **옵션:**
    
    - `AllowEmptyBracesZeroInit` (bool): `{}`를 통한 전체 0 초기화 허용 여부
        
    - `RequireNestedBracesForSubarrays` (bool): 하위 요소가 집합형(배열/구조체)일 때 중첩 중괄호 요구 여부
        
    - `RequireFullElementsForArrays` (bool): 선언된 크기에 맞게 모든 요소를 명시할 것을 요구할지 여부
        
    - `RequireFullBracesForStruct` (bool): 구조체 전체 필드 개수에 맞게 값을 명시할 것을 요구할지 여부
        

### AlwaysConstantConditionCheck

- **등록명:** `ast-always-constant-condition`
    
- **목적:** `if`, `while`, `for`, 조건 연산자(`?:`) 등의 조건식이 값과 무관하게 항상 참(True) 또는 거짓(False)으로 평가되는 경우를 탐지합니다.
    
- **옵션:**
    
    - `EnableConstantFoldPhase` / `EnableTypeRangePhase` (bool): 상수 평가 및 타입 범위를 활용한 분석 활성화 여부
        
    - `EnablePathConstraintPhase` (bool): 상위 분기 조건을 하위 분기로 전파하는 얕은 경로 제어 활성화 여부
        
    - `OnlyIntegerComparisons` (bool): 정수/열거형 타입 비교만 분석 대상으로 삼을지 여부
        
    - `UseParentDominatorOnly` (bool): 직상위 분기 제약만 고려할지 여부
        
    - `MaxConstraintDepth` (int): 조건 분석의 최대 중첩 깊이 제한
        
    - `ReportInElseBranches` / `AnalyzeTernary` (bool): else 내부 보고 및 삼항 연산자 분석 여부
        

### ArrayIndexBoundCheck

- **등록명:** `ast-array-index-bound`
    
- **목적:** `a[K]`나 `*(a + K)`와 같이 배열의 정적 크기를 초과하거나 음수 인덱스로 접근하는 버퍼 오버런/언더런을 탐지합니다.
    
- **옵션:**
    
    - `TrackIntraBlockConstants` (bool): 동일 블록 내에서 변수에 할당된 상수를 추적하여 분석할지 여부
        
    - `MaxScanStmts` (unsigned): 블록 내 상수 추적 시 스캔할 최대 구문(Statement) 수 제한
        

### BoolLogicOnlyOpsCheck

- **등록명:** `ast-bool-logic-only-ops`
    
- **목적:** Boolean 또는 Boolean 유사 평가 값에 논리 연산자(`&&`, `||`, `!`)가 아닌 산술/비트 연산자를 사용하는 행위를 금지합니다.
    
- **옵션:**
    
    - `ForbidEqualityComparisonsOnBool` (bool): 불리언 값에 대한 동등 비교(`==`, `!=`)도 금지할지 여부
        

### BrokenCryptoCheck

- **등록명:** `ast-broken-crypto`
    
- **목적:** 파훼된 해시 함수(MD5, SHA1 등)나 취약한 대칭키 암호화(DES, RC4 등) 및 ECB 운용 모드 사용을 탐지합니다. (CWE-327)
    
- **옵션:**
    
    - `WarnOnWeakHash` (bool): 취약한 해시 함수 사용 시 경고 여부
        
    - `WarnOnWeakCipher` (bool): 취약한 암호화/운용 모드 사용 시 경고 여부
        

### CharArrayStringSizeCheck

- **등록명:** `ast-char-array-string-size`
    
- **목적:** 문자열 리터럴로 `char` 배열을 초기화할 때, 배열 크기가 널 종료 문자(`\0`)를 포함하기에 부족한지 검사합니다.
    
- **옵션:**
    
    - `AllowNullTerminatedMatch` (bool): 배열 크기와 문자열 길이가 같아 널 문자 공간이 없는 경우를 허용할지 여부
        
    - `RequireExactSize` (bool): 배열 크기가 정확히 문자열 길이 + 1이 되도록 강제할지 여부
        
    - `OfferFixIt` (bool): 크기를 자동으로 수정하는 FixIt 힌트 제공 여부
        

### DangerousFunctionCheck

- **등록명:** `ast-dangerous-function`
    
- **목적:** 경계 확인이 없어 버퍼 오버플로우를 유발하는 함수(`strcpy`, `sprintf` 등)나 보안상 폐기된 함수(`gets` 등)의 사용을 탐지합니다.
    
- **옵션:**
    
    - `WarnOnDeprecated` (bool): `gets`, `bcopy` 등 폐기된 함수 사용 시 경고 여부
        
    - `WarnOnUnboundedCopy` (bool): 길이 검사가 없는 `strcpy`, `strcat` 등의 함수 사용 시 경고 여부
        

### DestructorExceptionSafetyCheck

- **등록명:** `ast-destructor-exception-safety`
    
- **목적:** 소멸자 내부에서 처리되지 않은 예외가 외부로 전파되는 것을 탐지하며, 프로그램 강제 종료를 막기 위해 `catch(...)` 처리를 유도합니다.
    
- **옵션:**
    
    - `WarnOnRethrowWithoutCatchAll` (bool): 확실한 `catch(...)`가 없는데도 `throw;`를 사용하는 경우 경고 여부
        
    - `WarnOnUnknown` (bool): 소멸자 내에서 타입을 추론할 수 없는 예외가 발생할 때 경고 여부
        

### DynamicCastInCtorDtorCheck

- **등록명:** `ast-dynamic-cast-in-ctor-dtor`
    
- **목적:** 생성자나 소멸자 내부에서 `dynamic_cast`를 사용하는 경우를 탐지합니다. (초기화/소멸 중에는 RTTI가 불완전하여 오동작 위험이 있음)
    
- **옵션:** 고유 옵션은 제공되지 않습니다.
    

### ErrorWithoutActionCheck

- **등록명:** `ast-error-without-action`
    
- **목적:** 비어있는 `catch` 블록이나 내용이 없는 `if`/`else` 조건문 실행 블록을 탐지합니다. (CWE-390, 단, 내부에 주석이 있으면 예외 가능)
    
- **옵션:**
    
    - `CheckEmptyCatch` (bool): 빈 `catch` 블록 검사 여부
        
    - `CheckEmptyIf` (bool): 빈 `if`/`else` 블록 검사 여부
        
    - `AllowCommentedEmptyBlocks` (bool): 비어있는 블록 내부에 주석이 있다면 검사에서 제외할지 여부
        

### ExceptionSpecificationCheck

- **등록명:** `ast-exception-specification`
    
- **목적:** `noexcept`, `throw()` 등 예외를 던지지 않겠다고 선언한 함수에서 예외가 발생하거나, 동적 예외 명세에 포함되지 않은 예외를 던지는 경우를 탐지합니다.
    
- **옵션:** 고유 옵션은 제공되지 않습니다.
    

### ExcessiveAuthenticationCheck

- **등록명:** `ast-excessive-authentication`
    
- **목적:** 인증 함수 내부에 무차별 대입 공격(Brute-force)을 방어하기 위한 로직(속도 제한, 지연 호출 등)이 존재하는지 와일드카드 패턴으로 검사합니다.
    
- **옵션:**
    
    - `AuthFunctions` (string): 검사 대상이 되는 인증 함수들의 와일드카드 패턴 리스트
        
    - `MitigationFunctions` (string): 방어 함수(지연, 잠금 등)의 와일드카드 패턴 리스트
        

### ExternFunctionDeclarationCheck

- **등록명:** `ast-extern-function-declaration`
    
- **목적:** 외부 함수를 프로토타입 선언 없이 사용하거나, 외부 함수 선언 시 `extern` 키워드가 누락된 경우를 탐지합니다.
    
- **옵션:**
    
    - `RequireExplicitExtern` (bool): 호출 이전 선언 시 명시적으로 `extern`을 붙이도록 강제할지 여부
        

### FixedAddressAssignmentCheck

- **등록명:** `ast-fixed-address-assignment`
    
- **목적:** 포인터 변수에 하드코딩된 정수 리터럴로 고정 메모리 주소를 직접 할당하는 행위를 탐지합니다. (`0`/`nullptr` 등은 제외, CWE-587)
    
- **옵션:** 고유 옵션은 제공되지 않습니다.
    

### FunctionCallArgumentConsistencyCheck

- **등록명:** `ast-function-call-argument-consistency`
    
- **목적:** 함수 호출 시 전달하는 인자의 개수 및 타입이 프로토타입 선언과 일치하는지, 또는 허용 가능한 암묵적 캐스팅 범위 내에 있는지 검사합니다.
    
- **옵션:**
    
    - `EnforceExactParamType` (bool): 정확한 타입 일치를 강제할지 여부
        
    - 기타 `Allow...` 옵션들: 배열/함수 포인터 디케이, 한정자 상향 캐스팅, 부모 클래스 캐스팅, 정수/실수 승격 등 세밀한 암묵적 캐스팅 허용 여부 제어
        
    - `IgnoreExplicitCasts` (bool): 명시적 캐스트 무시 여부
        
    - `RequirePrototypeForCalls` (bool): 함수 호출 전 프로토타입 존재 필수 여부
        

### FunctionSignatureConsistencyCheck

- **등록명:** `ast-function-signature-consistency`
    
- **목적:** [C언어 전용] 동일한 이름을 가진 모든 함수 선언 및 정의가 완전히 일치하는 시그니처(반환형, 매개변수 타입/개수, 가변 인자 유무)를 가졌는지 검사합니다.
    
- **옵션:** 고유 옵션은 제공되지 않습니다.
    

### HardcodedKeyCheck

- **등록명:** `ast-hardcoded-key`
    
- **목적:** `key`, `secret`, `iv` 등의 이름을 가진 변수에 하드코딩된 암호화 키 값(문자열 혹은 바이트 배열)이 직접 할당되는 보안 취약점을 탐지합니다. (CWE-798)
    
- **옵션:**
    
    - `KeyVariablePatterns` (string): 암호화 키를 의미하는 변수명의 와일드카드 패턴 리스트
        
    - `MinKeyLength` (unsigned): 너무 짧은 값으로 인한 오탐을 방지하기 위한 최소 문자열 크기 제한
        

### HardcodedPasswordCheck

- **등록명:** `ast-hardcoded-password`
    
- **목적:** `password`, `token` 관련 변수에 자격 증명이 하드코딩되거나, 사용자 입력값을 하드코딩 문자열과 직접 비교(`strcmp` 등)하는 로직을 탐지합니다.
    
- **옵션:**
    
    - `PasswordVariablePatterns` (string): 패스워드 변수명의 와일드카드 패턴 리스트
        
    - `StringCompareFunctions` (string): 비밀번호 비교에 사용되는 함수의 와일드카드 패턴 리스트
        

### IfStyleCheck

- **등록명:** `ast-if-style`
    
- **목적:** 방어적 프로그래밍 관점에서 조건 분기의 누락을 막기 위해, 모든 `if` 문에 예외나 다른 분기를 처리하기 위한 `else` 블록이 포함되어 있는지 검사합니다.
    
- **옵션:** 고유 옵션은 제공되지 않습니다.
    

### ImproperAuthorizationCheck

- **등록명:** `ast-improper-authorization`
    
- **목적:** 인가(Authorization)를 수행하는 주요 API가 실패했을 때, 그 반환값을 검사하지 않고 무시하여 권한 상승 등이 발생할 수 있는 결함을 탐지합니다.
    
- **옵션:**
    
    - `AuthFunctions` (string): 검사 대상 인가 함수의 이름 와일드카드 패턴 리스트
        

### ImproperLinkResolutionCheck

- **등록명:** `ast-improper-link-resolution`
    
- **목적:** 파일 접근 시 발생하는 심볼릭 링크 공격을 방지합니다. `open()`, `fopen()` 등에서 파일 디스크립터를 얻을 때 `O_NOFOLLOW` 플래그가 누락되는 등 안전하지 않은 접근을 경고합니다. (CWE-59)
    
- **옵션:** 고유 옵션은 제공되지 않습니다.
    

### IncorrectPermissionAssignmentCheck

- **등록명:** `ast-incorrect-permission-assignment`
    
- **목적:** `chmod`, `umask` 등 중요 자원 설정을 변경하는 함수에 누구나 쓰기 가능한 권한 상수값(World-Writable, 예: `0002` 등)이 전달되는지 AST 상수 평가로 탐지합니다. (CWE-732)
    
- **옵션:**
    
    - `PermissionFunctions` (string): 권한 설정 대상 함수들의 와일드카드 패턴 리스트
        
    - `DangerousMask` (string): 취약한 권한을 의미하는 위험 마스크 값 표기
        

### InsecureCodeDownloadCheck

- **등록명:** `ast-insecure-code-download`
    
- **목적:** 외부에서 코드를 다운로드하거나 동적으로 로드하는 함수를 호출할 때, 디지털 서명이나 해시(SHA-256 등)를 통한 무결성 검증 단계를 누락했는지 탐지합니다. (CWE-494)
    
- **옵션:**
    
    - `DownloadFunctions`, `LoadingFunctions`, `VerificationFunctions` (string): 각각 다운로드, 동적 로드, 무결성 검증 함수를 식별하는 와일드카드 패턴 리스트
        

### InsufficientRandomCheck

- **등록명:** `ast-insufficient-random`
    
- **목적:** 보안/암호학적 작업에 예측 가능한 레거시 난수 함수(`rand` 등)를 사용하거나 안전하지 않은 C++ 난수 엔진(`std::mt19937` 등)을 사용하는 경우를 탐지합니다. (CWE-330)
    
- **옵션:**
    
    - `WarnOnLegacyFunctions` / `WarnOnInsecureCPPPRNG` (bool): 레거시 함수 및 안전하지 않은 C++ 난수 엔진 각각에 대한 경고 여부
        

### LeftoverDebugCodeCheck

- **등록명:** `ast-leftover-debug-code`
    
- **목적:** 릴리즈(프로덕션) 배포 환경에 남아있으면 안 되는 디버그 전용 출력 함수 호출이나 테스트용 스위치(변수) 활성화 상태를 탐지합니다. (CWE-489)
    
- **옵션:**
    
    - `DebugKeywords` / `DebugFunctions` (string): 디버그용 변수와 함수를 식별하기 위한 와일드카드 패턴 리스트
        

### MainUnhandledThrowCheck

- **등록명:** `ast-main-unhandled-throw`
    
- **목적:** `main()` 함수 내부에서 처리되지 않고 외부로 탈출(전파)되는 예외 객체를 탐지하거나, 명시적인 `catch(...)` 없이 사용된 재투척(`throw;`) 문을 경고합니다.
    
- **옵션:**
    
    - `SkipNonImmediateLambdas` / `SkipLocalClasses` (bool): 즉시 호출되지 않는 람다나 지역 클래스를 분석 대상에서 제외할지 여부
        
    - `WarnOnRethrowWithoutCatchAll` / `WarnOnUnknown` (bool): catch 없이 재투척하거나 타입을 추론할 수 없는 예외 발생 시 경고 여부
        

### MallocSizeMultipleCheck

- **등록명:** `ast-malloc-size-multiple`
    
- **목적:** `malloc`으로 메모리를 할당할 때 전달하는 바이트 크기가, 대상 포인터의 원소 타입 크기(`sizeof(T)`)의 정수배를 만족하는지 검증합니다.
    
- **옵션:**
    
    - `RequireExplicitSizeof` (bool): 계산된 크기가 상수 배수라 할지라도 수식 내에 명시적인 `sizeof(T)` 표기가 없으면 경고할지 여부
        
    - `AllowFlexibleTail` (bool): 유연한 배열 구조(Flexible array) 꼬리가 더해지는 형태의 수식을 허용할지 여부
        

### MissingAuthenticationCheck

- **등록명:** `ast-missing-authentication`
    
- **목적:** 시스템 명령 실행이나 계정 삭제와 같은 중요 기능을 수행하기 전, 사용자의 세션이나 토큰을 검증하는 인증 절차 로직이 누락되었는지 탐지합니다.
    
- **옵션:**
    
    - `CriticalFunctions` / `AuthFunctions` (string): 중요 기능 함수와 인증 기능 함수를 구별하기 위한 와일드카드 패턴 리스트
        

### MultiStatementPerLineCheck

- **등록명:** `ast-multi-statement-per-line`
    
- **목적:** 가독성 향상을 위해 한 줄에 여러 개의 명령문, 멤버 변수, 전역 선언이 혼재되어 작성된 경우를 탐지하여 각각 별도의 줄로 분리하도록 유도합니다.
    
- **옵션:**
    
    - `CheckGlobalDecls`, `CheckFunctionBodies`, `CheckCompoundStmts`, `CheckRecordMembers` (bool): 각각 전역 선언, 함수 본문 내부, 복합 명령문, 클래스/구조체 멤버 선언에 대한 검사 포함 여부
        

### NoAssignmentInConditionCheck

- **등록명:** `ast-no-assignment-in-condition`
    
- **목적:** `if`, `while`, `for`, 조건 연산자(`?:`) 등의 조건식 내부에서 오타나 가독성 저하를 유발하는 대입 연산자(`=`, `+=`, `-=` 등) 사용을 원천 금지합니다.
    
- **옵션:**
    
    - `CheckTernaryCondition` (bool): 삼항 조건 연산자의 조건식 부분까지 검사 대상에 포함할지 여부
        

### NoAutoTypeCheck

- **등록명:** `ast-no-auto-type`
    
- **목적:** 명시적인 타입 선언을 유도하여 코드 가독성을 높이기 위해, 변수 선언 및 함수의 반환 타입에 `auto` 키워드를 사용하는 것을 금지합니다.
    
- **옵션:**
    
    - `CheckVariableDecls` / `CheckFunctionReturns` (bool): 변수 선언 시점과 함수 반환형에서 각각 `auto` 사용을 검사할지 여부
        

### NoFloatEqualityCheck

- **등록명:** `ast-no-float-equality`
    
- **목적:** 부동소수점 값의 미세 오차로 인해 오동작할 수 있는 정확한 동등성 비교(`==`, `!=`)를 금지하고 입실론(epsilon) 근사 비교를 권장합니다.
    
- **옵션:**
    
    - `IncludeDouble` / `IncludeLongDouble` (bool): `double`과 `long double` 자료형도 검사할지 여부
        
    - `RequireBothOperandsFloating` / `IgnoreBothConstantOperands` (bool): 양쪽 피연산자가 모두 실수일 때만 검사하거나, 둘 다 상수식인 경우 무시할지 여부
        
    - `AllowNaNSelfInequalityPattern` / `AllowCompareWithZero` (bool): NaN 판별 패턴(`x != x`)이나 0과의 직접 비교를 허용할지 여부
        
    - `OfferFixItUsingEpsilon` (bool): 입실론을 활용한 자동 수정안(Fix-It) 제시 여부
        
    - `EpsilonExpr` / `FixFunction` (string): 제안에 사용할 오차 허용 상수 문자열(예: `"EPS"`)과 절댓값 함수(예: `"std::fabs"`) 정의
        

### NoGotoCheck

- **등록명:** `ast-no-goto`
    
- **목적:** 구조적 프로그래밍 패러다임을 준수하고 제어 흐름 복잡도를 제어하기 위해 모든 `goto` 문의 사용을 탐지하여 경고합니다.
    
- **옵션:** 고유 옵션은 제공되지 않습니다.
    

### NoMallocCheck

- **등록명:** `ast-no-malloc`
    
- **목적:** C++ 코드베이스 내에서 `malloc`, `calloc`, `realloc`, `free`와 같은 C 스타일 메모리 관리 함수 사용을 감지하고 `new`/`delete` 및 스마트 포인터 사용을 유도합니다.
    
- **옵션:** 고유 옵션은 제공되지 않습니다.
    

### NoMeaninglessExprCheck

- **등록명:** `ast-no-meaningless-expr`
    
- **목적:** 표현식이 실행되더라도 상태 변화(Side-effect, 부작용)가 전혀 일어나지 않는 무의미한 독립 구문을 탐지합니다.
    
- **옵션:**
    
    - `IncludePossibleEffects` (bool): `volatile` 변수 읽기 등 잠재적인 부작용 가능성도 평가에 포함할지 여부
        
    - `IgnoreExplicitVoidCast` (bool): 의도적으로 `(void)` 캐스팅을 붙여 결과를 무시한 표현식은 검사에서 제외할지 여부
        

### NoMixedCharWideStringCheck

- **등록명:** `ast-no-mixed-char-wide-string`
    
- **목적:** `"hello"` `L"world"`와 같이 서로 다른 인코딩 형식(일반, Wide, UTF-8 등)의 문자열 리터럴들이 잘못 섞여서 하나로 연결(Concatenate)되는 상황을 진단합니다.
    
- **옵션:** 고유 옵션은 제공되지 않습니다.
    

### NoMixedLinkageCheck

- **등록명:** `ast-no-mixed-linkage`
    
- **목적:** 동일한 식별자에 대해 외부 연결(`extern`)과 내부 연결(`static`)이 혼용되어 선언된 링키지 모호성 결함을 진단합니다.
    
- **옵션:** 고유 옵션은 제공되지 않습니다.
    

### NoOutOfRangeAssignmentCheck

- **등록명:** `ast-no-out-of-range-assignment`
    
- **목적:** 정수, 부동소수점, 열거형, 비트필드 등의 타입에 값을 대입/초기화할 때 해당 타입이 표현 가능한 최솟값/최댓값 범위를 초과(오버플로우)하는지 검사합니다.
    
- **옵션:**
    
    - `OnlyConstantExpr` (bool): 상수 표현식에 대해서만 확정적으로 범위 초과를 경고할지 여부 (`false` 시 잠재적 위험도 경고)
        
    - `CheckBitFields` / `CheckBoolean` / `CheckFloatingTargets` (bool): 비트필드, `bool` 타입, 부동소수점 타겟의 검사 활성화 여부
        
    - `DiagnoseFloatUnderflow` / `DiagnoseFloatInexact` (bool): 실수 변환 시 발생하는 언더플로우나 반올림 등에 의한 부정확성(Inexact)도 진단할지 여부
        

### NoSaltedHashCheck

- **등록명:** `ast-no-salted-hash`
    
- **목적:** 패스워드 해싱 시 `crypt()`에 하드코딩된 솔트(Static Salt)를 사용하거나 일반 단방향 해시(MD5, SHA1 등)에 패스워드를 그대로 넘기는 보안 결함을 탐지합니다. (CWE-759)
    
- **옵션:** 고유 옵션은 제공되지 않습니다.
    

### NoShadowingCheck

- **등록명:** `ast-no-shadowing`
    
- **목적:** 지역 변수 이름이 바깥쪽 스코프의 변수, 파라미터, 전역 변수 등과 동일하게 지어져 상위 식별자를 가리는 섀도잉 문제를 진단합니다.
    
- **옵션:**
    
    - `ForbidOuterLocalShadowing` / `ForbidGlobalShadowing` (bool): 외곽 지역 변수 및 전역 변수 섀도잉 금지 여부
        
    - `CheckParamShadowing` / `CheckCatchParamShadowing` (bool): 함수 매개변수나 `catch` 블록 예외 변수 섀도잉 검사 여부
        
    - `CheckStructuredBindings` (bool): C++17 구조화 바인딩(`auto [x, y]`) 내 섀도잉 검사 여부
        
    - `OnlyMainFileGlobals` (bool): 메인 파일에 속한 전역 변수의 섀도잉만 경고할지 여부
        
    - `IgnoreForLoopInit` / `IgnoreIfSwitchInit` (bool): `for`, `if`/`switch` 초기화 블록 내 섀도잉을 예외적으로 허용할지 여부
        
    - `IncludeMethods` / `IncludeFreeFunctions` (bool): 클래스 메서드 및 일반 자유 함수 포함 여부
        
    - `OfferRenameFixIt` (bool) / `RenameSuffix` (string): 수정 권장 Fix-It 제공 여부 및 이름 뒤에 붙일 접미사 지정 (예: `"_new"`)
        

### NotNullTerminatedResultCheck

- **등록명:** `ast-not-null-terminated-result`
    
- **목적:** `memcpy`, `strncpy` 등의 문자열/메모리 조작 함수 호출 시, 결과물이 널 문자(`\0`)로 정상 종료되지 않아 발생하는 버퍼 오버독 취약점을 탐지합니다. (CWE-170)
    
- **옵션:**
    
    - `WantToUseSafeFunctions` (bool): `memcpy_s` 등 안전한 C11 확장 함수 사용을 권장안으로 제시할지 여부
        
    - `OfferFixIt` (bool): 코드를 자동으로 수정하는 Fix-It 힌트 제공 여부
        

### PartialCopyAssignmentCheck

- **등록명:** `ast-partial-copy-assignment`
    
- **목적:** 복사 대입 연산자(`operator=`) 내부에서 일부 멤버 변수가 대입 대상에서 누락되는 논리적 누수를 검사합니다.
    
- **옵션:**
    
    - `CheckDeepCopy` (bool): 포인터 멤버에 대해 단순 주소만 복사되는 얕은 복사(Shallow Copy) 여부를 추가 검사할지 지정
        

### PointerCvQualifierDropCheck

- **등록명:** `ast-pointer-cv-qualifier-drop`
    
- **목적:** 명시적 또는 암시적 캐스팅 과정에서 포인터가 가리키는 대상(Pointee)의 안전 한정자인 `const`나 `volatile`이 임의 제거되는 상황을 진단합니다.
    
- **옵션:**
    
    - `WarnOnConstDrop` (bool): `const` 한정자가 제거될 때 경고 여부
        
    - `WarnOnVolatileDrop` (bool): `volatile` 한정자가 제거될 때 경고 여부
        

### PrivateDataExposureCheck

- **등록명:** `ast-private-data-exposure`
    
- **목적:** `public` 메서드가 `private`/`protected` 멤버 변수의 원본 주소나 비-상수(non-const) 참조를 반환하여 캡슐화를 위반하는 행위를 탐지합니다. (CWE-495)
    
- **옵션:** 고유 옵션은 제공되지 않습니다.
    

### PublicDataAssignedCheck

- **등록명:** `ast-public-data-assigned`
    
- **목적:** 외부에서 전달된 외부 포인터(또는 배열) 파라미터를 깊은 복사(Deep Copy) 없이 내부 `private` 멤버 변수에 직접 대입하여 참조 유출을 유발하는 상황을 탐지합니다. (CWE-496)
    
- **옵션:** 고유 옵션은 제공되지 않습니다.
    

### PureVirtualInitCheck

- **등록명:** `ast-pure-virtual-init`
    
- **목적:** 순수 가상 함수를 선언할 때, 매크로나 다른 우회 리터럴이 아닌 명시적인 `= 0` 표준 리터럴을 사용하여 선언했는지 엄격히 검사합니다. (예: `= ZERO` 사용 금지)
    
- **옵션:** 고유 옵션은 제공되지 않습니다.
    

### RepresentableCastCheck

- **등록명:** `ast-representable-cast`
    
- **목적:** 정수/실수 변환(캐스팅) 과정에서 값의 범위 초과(오버플로우), 부호 불일치로 인한 의미 손실, 소수부 정밀도 파괴 등을 초래하는 표현 불가능한 형변환을 탐지합니다.
    
- **옵션:**
    
    - `WarnPotential` (bool): 확정이 아닌 잠재적 위험 가능성만 있어도 경고할지 여부
        
    - `IgnoreExplicitCasts` (bool): 개발자가 명시한 캐스트(`static_cast` 등)는 의도적인 것으로 보아 무시할지 여부
        
    - `TreatEnumAsUnderlying` (bool): `enum` 타입을 내부 기저 정수형으로 풀어서 검사할지 여부
        
    - `WarnIntToFloat` (bool): 정수에서 실수로 변환할 때 발생하는 정밀도 손실 경고 여부
        

### ReturnStackAddressCheck

- **등록명:** `ast-return-stack-address`
    
- **목적:** 함수 종료 시 소멸되는 지역 변수(스택 메모리)의 주소나 참조를 반환하여 댕글링 포인터 및 미정의 동작(UB)을 유발하는 코드를 탐지합니다.
    
- **옵션:** 고유 옵션은 제공되지 않습니다.
    

### SequencingRulesCheck

- **등록명:** `ast-sequencing-rules`
    
- **목적:** 하나의 전체 수식(Full-expression) 내부에서 동일 변수를 중복해서 변경하거나, 변경과 동시에 접근(읽기)하여 연산 순서가 보장되지 않는(Unsequenced) 위험 동작을 진단합니다.
    
- **옵션:**
    
    - `TrackMembersAndSubscripts` (bool): 구조체 멤버나 배열의 인덱스 접근 요소까지 정밀 추적할지 여부
        

### ShiftAmountRangeCheck

- **등록명:** `ast-shift-amount-range`
    
- **목적:** 비트 시프트 연산(`<<`, `>>`)에서 시프트 양이 음수이거나, 피연산자 자료형의 유효 비트 폭을 초과하는 범위를 가지는지 검사합니다.
    
- **옵션:**
    
    - `UseDeclaredFixedWidth` (bool): 최종 결과 타입이 아닌 원래 선언된 타입의 폭(예: `uint8_t` 비트필드)을 기준으로 엄격히 검사할지 여부
        
    - `WarnOnBoolShift` (bool): `bool` 타입에 대해 비트 시프트 연산을 사용하는 행위 경고 여부
        

### SignedBitwiseSafetyCheck

- **등록명:** `ast-signed-bitwise-safety`
    
- **목적:** 부호 있는 정수(Signed)에 비트 연산(`<<`, `~`, `&`, `|`, `^`)을 적용할 때 부호 비트가 손상되거나 예기치 않은 음수 결과가 발생하는 위험을 경고합니다.
    
- **옵션:**
    
    - `WarnOnShiftLeftSigned` / `WarnOnBitwiseNotSigned` (bool): 부호 있는 정수의 `<<`, `~` 연산 경고 여부
        
    - `WarnOnBitwiseAndOrXor` (bool): 부호 있는 정수의 `&`, `|`, `^` 연산도 깐깐하게 경고할지 여부
        
    - `AllowTopLevelUnsignedCast` (bool): 최상위에 명시적으로 `unsigned` 캐스트를 씌운 경우는 경고에서 예외 처리할지 여부
        

### SingleExitAndReturnTypeCheck

- **등록명:** `ast-single-exit-and-return-type`
    
- **목적:** 단일 진출입(Single Exit) 규칙 준수를 위해 함수 내 반환 지점(`return`)을 1개로 제한하며, 선언된 반환 타입과 실제 반환값 타입의 일치 여부를 검사합니다.
    
- **옵션:**
    
    - `CheckSingleExit` (bool): 함수 내에 여러 개의 `return` 문이 존재하는지 검사
        
    - `CheckReturnType` (bool): 선언 타입과 반환 타입의 일치 여부 검사
        
    - `CheckMissingReturn` (bool): 값을 반환해야 하는 함수에서 `return` 문이 누락되었는지 검사
        
    - `StrictTypeMatching` (bool): 암묵적 변환을 허용하지 않고 엄격한 타입 매칭을 적용할지 여부
        

### SizeofOnPointerCheck

- **등록명:** `ast-sizeof-on-pointer`
    
- **목적:** 포인터 변수에 대해 `sizeof()` 연산자를 사용할 때, 버퍼의 실제 크기가 아닌 아키텍처 포인터 크기(4/8바이트)가 반환되는 논리적 치명상을 탐지합니다. (CWE-467)
    
- **옵션:** 고유 옵션은 제공되지 않습니다.
    

### SwitchStyleCheck

- **등록명:** `ast-switch-style`
    
- **목적:** `switch` 문의 구조적 안정성과 코딩 스타일 규칙을 검증합니다.
    
- **옵션:**
    
    - `CheckLogicalOperators` (bool): 조건식에 `&&`, `||`, `!` 등 논리 연산자 사용 금지 여부
        
    - `RequireDefaultLabel` (bool): `default` 구문 강제 여부
        
    - `CheckCodeBeforeFirstLabel` (bool): 첫 번째 `case` 문 이전에 실행 코드가 존재하는지 검사
        
    - `CheckFallThrough` (bool): `break`가 누락된 `case` 문이 명시적 속성/주석 없이 사용되었는지 검사
        
    - `CheckUnreachableCases` (bool): 조건식의 타입 범위를 벗어나 절대 실행될 수 없는 `case` 값이 있는지 검사
        
    - `WarnOnEnumMismatch` (bool): `enum` 타입에 정의되지 않은 값이 `case`에 사용되었는지 검사
        
    - `CheckComplexExpressions` (bool): 복잡한 수식의 상수 평가 결과도 유효 범위 내에 있는지 검사
        

### UnevaluatedSideEffectCheck

- **등록명:** `ast-unevaluated-side-effect`
    
- **목적:** `sizeof`, `alignof`, `noexcept`, `decltype` 등 실제로 평가되지 않는 런타임 문맥 내부에서 상태 변경 유발 연산(`++`, `--`, 대입, 함수 호출 등)이 오용되는 것을 방지합니다.
    
- **옵션:**
    
    - `CheckSizeofAlignof`, `CheckNoexcept`, `CheckDecltype` (bool): 각각의 해당 키워드에 대해 검사를 독립적으로 활성화할지 여부
        

### UnsignedMinusAssignmentCheck

- **등록명:** `ast-unsigned-minus-assignment`
    
- **목적:** 단항 음수 연산자(`-`)의 계산 결과를 기본 자료형이 부호 없는(Unsigned) 타입 변수에 대입, 초기화, 또는 반환하는 위험 동작을 방지합니다.
    
- **옵션:**
    
    - `CheckAssignment`, `CheckCompoundAssignments`, `CheckVarInit`, `CheckReturn`, `CheckCtorMemberInit` (bool): 검사를 적용할 위치를 개별 제어
        
    - `AllowExplicitUnsignedCast` (bool): 명시적인 `unsigned` 캐스팅이 동반된 경우에는 경고하지 않고 허용할지 여부
        

### UnusedExceptionHandlerCheck

- **등록명:** `ast-unused-exception-handler`
    
- **목적:** `try-catch` 구조에서 상위에 위치한 `catch` 블록(예: Base 클래스 또는 `catch(...)`)이 하위 블록의 예외를 먼저 가로채어, 뒤쪽의 구체적인 핸들러가 사장되는 구조적 오류를 진단합니다.
    
- **옵션:** 고유 옵션은 제공되지 않습니다.
    

### VarargsFormatCheck

- **등록명:** `ast-varargs-format`
    
- **목적:** 포맷 스트링 계열 함수에서 보안 취약점을 유발하는 `%n` 지시자 사용을 금지하고, 포맷 문자열과 인자의 개수/타입 불일치를 정적 분석합니다.
    
- **옵션:**
    
    - `EnablePrintf` / `EnableScanf` (bool): 각각의 함수군 검사 활성화 여부
        
    - `AllowDynamicFormat` (bool): 동적으로 생성된 문자열 변수를 포맷 스트링 인자로 사용하는 것을 허용할지 여부
        
    - `WarnOnPercentN` (bool): `%n` 지정자 탐지 시 강력 경고 여부
        
    - `ScanfRequirePointerExact` (bool): `scanf` 계열에서 정확한 포인터 타입 일치를 요구할지 여부
        
    - `PrintfZeroWidthStarAsInt` (bool): `printf` 가변 폭(`*`) 지정자 사용 시 인자로 정확히 `int`를 요구할지 여부
        
    - `HonorMSVCExt` (bool): MSVC 컴파일러 고유의 포맷 확장 규칙 반영 여부
        

### VirtualBaseCastCheck

- **등록명:** `ast-virtual-base-cast`
    
- **목적:** 가상 상속(Virtual Inheritance) 구조 클래스에서 `static_cast`나 `reinterpret_cast`를 통한 다운캐스팅 시 발생하는 오프셋 메모리 오류를 방지하기 위해, 안전한 `dynamic_cast`로 변경할 것을 권장합니다.
    
- **옵션:** 고유 옵션은 제공되지 않습니다.
    

### VirtualCallInCtorDtorCheck

- **등록명:** `ast-virtual-call-in-ctor-dtor`
    
- **목적:** 객체가 불완전한 상태인 생성자/소멸자 본문 내부에서, 스코프 식별자 지정 없이 가상 함수를 직접 호출하여 가상 디스패치 테이블(vtable)이 예기치 않게 동작하는 상황을 경고합니다.
    
- **옵션:** 고유 옵션은 제공되지 않습니다.
    

### VirtualPureCheck

- **등록명:** `ast-virtual-pure`
    
- **목적:** `= 0`으로 선언된 순수 가상 함수를 재정의/구현할 때, 코드 가독성 및 명확성을 높이기 위해 선언부 앞에 `virtual` 키워드가 명시되어 있는지 검사합니다.
    
- **옵션:** 고유 옵션은 제공되지 않습니다.
    

### WeakPasswordRequirementsCheck

- **등록명:** `ast-weak-password-requirements`
    
- **목적:** 비밀번호 설정 및 변경 로직 주변에서 패스워드의 최소 복잡도(길이, 조합 등)를 검증하는 유효성 체크 함수가 누락된 취약점을 탐지합니다. (CWE-521)
    
- **옵션:**
    
    - `PasswordSetFunctions` (string): 패스워드 설정으로 식별할 함수의 와일드카드 패턴 리스트
        
    - `ComplexityCheckFunctions` (string): 복잡도 검증으로 식별할 함수의 와일드카드 패턴 리스트
        

## 3. CFG 기반 체커 (`cfg-` 접두사)

제어 흐름 그래프(Control Flow Graph)를 생성하고 시뮬레이션하여 모든 실행 경로 상의 모순, 자원 누수, 미정의 동작 가능성을 추적하는 경로 민감 분석 체커군입니다.

### InitBeforeUseCheck

- **등록명:** `cfg-init-before-use`
    
- **목적:** 지역 자동 변수가 값을 읽히기 전 함수 내 모든 실행 경로 상에서 반드시 초기화되도록 CFG 기반 데이터플로우 분석을 수행합니다.
    
- **옵션:**
    
    - `TreatMemsetZeroAsInit` (bool): `memset`을 통해 0으로 채우는 행위를 유효한 초기화로 인정할지 여부
        
    - `TreatSprintfFamilyAsInit` (bool): `sprintf` 계열 함수의 사용을 초기화 행위로 간주할지 여부
        

### NewDeleteFormConsistencyCheck

- **등록명:** `cfg-new-delete-form-consistency`
    
- **목적:** 메모리를 `new`로 할당하고 `delete[]`로 해제하거나, 반대로 `new[]`로 할당 후 `delete`로 해제하는 등 동적 할당과 해제의 형태 불일치를 진단합니다.
    
- **옵션:** 고유 옵션은 제공되지 않습니다.
    

### NonZeroDivisorGuardCheck

- **등록명:** `cfg-nonzero-divisor-guard`
    
- **목적:** 변수를 이용한 나눗셈(`/`) 및 나머지(`%`) 연산 시, 해당 분모 변수가 0이 아님을 검사하는 보호 조건(Guard) 로직이 제어 흐름 경로 상에 안전하게 존재하는지 분석합니다.
    
- **옵션:**
    
    - `CheckModulus` / `CheckCompoundAssignment` (bool): `%` 연산과 복합 대입(`/=`, `%=`) 연산자 포함 여부
        
    - `IncludeFloatingPoint` (bool): 부동소수점 타입 변수를 분모로 사용할 때도 검사할지 여부
        
    - `EnforceExplicitGuard` (bool): 초기화 값에 의존하지 않고 명시적인 조건문 검사를 엄격히 강제할지 여부
        
    - `RecognizeConstNonZeroAsGuard` / `RecognizeSizeofAsNonZero` (bool): 0이 아닌 상수값이나 `sizeof` 연산 결과 자체를 안전한 보호 로직으로 인정할지 여부
        
    - `CfgBlockVisitBudget` (unsigned): CFG 순회 시 허용할 최대 분석 예산 설정
        

### NullDereferenceGuardCheck

- **등록명:** `cfg-null-dereference-guard`
    
- **목적:** 포인터를 역참조하기 전에 모든 제어 흐름 경로 상에서 NULL 여부 검사(Guard)가 확실히 수행되었는지 CFG 기반으로 정밀 데이터플로우 분석을 수행합니다.
    
- **옵션:**
    
    - `CheckArraySubscript` (bool): 배열 인덱스 접근(`[]`) 시에도 널 역참조 검사를 수행할지 여부
        
    - `TreatAddressOfInitAsNonNull` / `TreatNewAsNonNull` (bool): 주소 연산자(`&`)나 `new` 할당을 통한 초기화를 Non-Null로 간주할지 여부
        
    - `EnforceExplicitGuard` (bool): 초기화 값에 의존하지 않고 명시적 조건문 검사를 강제할지 여부 (엄격 모드)
        
    - `CfgBlockVisitBudget` (unsigned): 데이터플로우 분석 시 CFG 탐색 예산 한도 설정
        
    - `AssumeNonNullParams` / `AssumeNonNullGlobals` (bool): 파라미터나 전역 변수를 기본적으로 Non-Null로 가정할지 여부
        

### NullPointerArithmeticCheck

- **등록명:** `cfg-null-pointer-arithmetic`
    
- **목적:** `++`, `--`, `+`, `-`, `+=` 등 NULL 상태(`0`/`nullptr`)인 포인터 객체에 대해 잘못된 주소 산술 연산을 수행하는 것을 금지합니다.
    
- **옵션:**
    
    - `CheckArraySubscriptBase` (bool): 배열 인덱싱의 기준(Base) 포인터에 대해서도 검사할지 여부
        
    - `GuardScanBudget` (unsigned): CFG 역방향 탐색 최대 횟수 예산 제한
        

### UninitializedAddressToConstParamCheck

- **등록명:** `cfg-uninitialized-address-to-const-param`
    
- **목적:** 로컬 변수가 명확히 초기화되지 않은 상태에서 해당 변수의 주소(`&var`)를 읽기 전용인 `const` 포인터 파라미터로 넘기는 잘못된 호출을 CFG 기반으로 감지합니다.
    
- **옵션:**
    
    - `ConsiderMemsetZeroAsInit` (bool): `memset`을 통한 0 초기화를 유효한 초기화로 인정할지 여부
        
    - `ConsiderCompoundAssignAsInit` (bool): `+=` 같은 복합 대입 연산자도 초기화로 인정할지 여부
        
    - `CfgBlockVisitBudget` (unsigned): 데이터플로우 분석 시 CFG 노드 방문 최대 한도 제한
        

### UnreachableCodeCheck

- **등록명:** `cfg-unreachable-code`
    
- **목적:** `return`, `throw`, `break`, `continue`, `goto` 등의 명시적 흐름 제어 구문 뒤에 위치하여 절대 실행될 수 없는 데드 코드(Dead Code)를 탐지합니다.
    
- **옵션:** 고유 옵션은 제공되지 않습니다.
    

### UseAfterFreeCheck

- **등록명:** `cfg-use-after-free`
    
- **목적:** `delete`를 통해 동적 해제된 포인터를 이후 후속 로직에서 다시 참조하거나 멤버에 접근하는 UAF(Use-After-Free) 보안 취약점을 데이터플로우 분석으로 추적합니다.
    
- **옵션:** 고유 옵션은 제공되지 않습니다.
    

## 4. 기타 체커 (Metrics & Call Graph)

특정 소스 단위를 넘어서 복잡도를 수치화하거나, 코드베이스 전체 혹은 파일 단위의 구조적 종속 관계(함수 호출 지도)를 분석하여 시각화 데이터(JSON 등)를 추출하는 도구형 체커군입니다. (별도 접두사 없음)

### FunctionCallGraphCheck

- **등록명:** `function-call-graph`
    
- **목적:** 파일 내 함수들의 호출 관계 결합도(Fan-in, Fan-out)를 측정하여 임계값 초과 시 경고하고, 프로그램 종료 시 이를 JSON 콜 그래프 파일로 추출합니다.
    
- **옵션:**
    
    - `FanInThreshold` / `FanOutThreshold` (unsigned): 함수 호출을 받거나 하는 최대 횟수의 임계값
        
    - `OutputPath` (string): 추출할 JSON 파일의 저장 경로
        

### FunctionCodeMetric

- **등록명:** `function-code-metric`
    
- **목적:** 함수의 크기와 복잡도를 나타내는 다양한 메트릭 지표를 측정하여 허용 가능한 한도를 제한합니다.
    
- **옵션:**
    
    - `LineThreshold` (unsigned/int): 전체 라인 수 제한
        
    - `StatementThreshold` (unsigned/int): 구문 개수 제한 (기본값: `800`)
        
    - `NclocThreshold` (unsigned/int): 주석을 제외한 실행 코드 라인 수 제한
        
    - `CyclomaticComplexityThreshold` (unsigned/int): CFG 기반 순환 복잡도 한도
        
    - `ParameterThreshold` (unsigned/int): 매개변수 개수 제한
        
    - `NestingThreshold` (unsigned/int): 최대 중첩 깊이 제한
        
    - `VariableThreshold` (unsigned/int): 지역 변수 수 제한
        

### FunctionSignatureCallGraphCheck

- **등록명:** `function-signature-call-graph`
    
- **목적:** 함수의 호출자(Caller)와 피호출자(Callee) 간의 구조적 시그니처 정보(매개변수, 반환형 등)를 추출하여 JSON 파일로 저장합니다.
    
- **옵션:**
    
    - `OutputPath` (string): 결과물이 저장될 JSON 파일의 경로