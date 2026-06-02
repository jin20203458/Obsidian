
## 1. Core 및 패키지 계층 체커 (`path-sensitive-core` 접두사)

Clang Static Analyzer의 핵심 엔진 영역에서 구동되며 변수 초기화, 널 참조, 제로 분할 등 제어 흐름 및 메모리의 근본적인 결함을 심볼릭 실행을 통해 추적하는 체커군입니다.

### DivZeroChecker

- **등록명:** `path-sensitive-core.DivideZero`
    
- **목적:** 수학 연산 시 분모가 0이 되어 발생하는 '0으로 나누기(Division by zero)' 오류를 검사합니다. 오염된 데이터(Tainted value)에 의해 0으로 나누어질 가능성이 있는 경우도 추가로 탐지합니다.
    
- **옵션:** 고유 옵션은 제공되지 않습니다.
    

### DereferenceChecker

- **등록명:** `path-sensitive-core.NullDereference`
    
- **목적:** 엔진 로드/스토어 시점에 발생하는 널 포인터 역참조(Null dereference), 미정의된(Undefined) 포인터 값 및 레이블 주소의 역참조 오류를 찾아냅니다.
    
- **옵션:**
    
    - `SuppressAddressSpaces` (bool): x86/x86-64 아키텍처 특수 세그먼트(GS, FS, SS에 해당하는 주소 공간 #256, 257, 258)에 대한 역참조 경고를 억제할지 제어합니다.
        

### UndefinedAssignmentChecker

- **등록명:** `path-sensitive-core.uninitialized.Assign`
    
- **목적:** 초기화되지 않았거나 쓰레기 값을 포함한 변수(Undefined value)의 결과를 다른 변수에 대입(할당)하는 것을 탐지합니다. 단, 흔히 사용되는 swap 함수 내부에서의 맞교환 대입은 예외적으로 처리합니다.
    
- **옵션:** 고유 옵션은 제공되지 않습니다.
    

### StackAddrEscapeChecker

- **등록명:** `path-sensitive-core.StackAddressEscape`
    
- **목적:** 함수의 지역 변수나 임시 객체처럼 스택(Stack)에 할당된 메모리 주소가 return을 통해 호출자에게 반환되거나, 전역 변수나 비동기 실행 블록(`dispatch_async` 등)에 저장되어 댕글링(Dangling) 참조를 유발하는 결함을 검사합니다.
    
- **옵션:** 고유 옵션은 제공되지 않습니다.
    

## 2. Unix & C++ 표준 체커 (`path-sensitive-unix` 및 `cplusplus` 접두사)

Unix/POSIX API 표준 함수 및 C++의 동적 메모리 할당/해제 연산자, 파일 스트림 자원 처리 규칙을 검증하여 오용이나 누수를 탐지하는 체커군입니다.

### MallocChecker

- **등록명:** `path-sensitive-unix.Malloc`
    
- **목적:** `malloc`, `free`, `realloc` 동적 메모리 할당 및 해제를 전반적으로 모델링합니다. 메모리 누수(Leak), 이중 해제(Double Free), 해제 후 사용(Use-After-Free) 등을 광범위하게 검사합니다.
    
- **옵션:**
    
    - `Optimistic` (bool): `ownership_takes`, `ownership_holds`, `ownership_returns`와 같이 사용자 정의 메모리 관리 함수에 붙은 소유권 어노테이션(Annotation)을 적극적으로 모델링할지 지정합니다.
        
    - `AddNoOwnershipChangeNotes` (bool): 메모리를 해제하지 않고 그대로 반환할 때, 분석기에서 소유권 변경이 없음을 알리는 추가 진단 노트를 생성할지 제어합니다.
        

### NewDeleteChecker

- **등록명:** `path-sensitive-cplusplus.NewDelete`
    
- **목적:** C++의 `new`, `delete` 연산자에 따른 동적 메모리 할당 및 해제 수명 주기를 모델링하여 메모리 누수, 이중 해제, 해제 후 사용 등의 결함을 검사합니다.
    
- **옵션:** `MallocChecker`의 모델링 옵션을 공유 및 연동합니다.
    

### MismatchedDeallocatorChecker

- **등록명:** `path-sensitive-unix.MismatchedDeallocator`
    
- **목적:** 할당자/해제자의 불일치 결함(예: `malloc`으로 메모리를 할당한 후 C++의 `delete`로 자원을 해제하려는 행위)을 탐지합니다.
    
- **옵션:** 고유 옵션은 제공되지 않습니다.
    

### StreamChecker

- **등록명:** `path-sensitive-unix.Stream`
    
- **목적:** 파일 스트림(`FILE*`) 관련 함수(`fopen`, `fclose`, `fread`, `fseek` 등)의 동작을 추적합니다. NULL 스트림 사용, 이미 닫힌 스트림 재사용, 위치가 불확정(indeterminate)인 상태에서의 I/O 수행, 리소스 누수(닫지 않은 스트림) 등을 찾아냅니다.
    
- **옵션:**
    
    - `Pedantic` (bool): 깐깐한 모드를 활성화하여, `fwrite`나 `fseek` 실패 시 에러 플래그가 설정되고 파일 위치가 불확정하게 변하는 실패 경로(Failure branch)를 추가로 모델링 및 경고합니다.
        

## 3. ARQA 커스텀 및 보안 취약점 체커 (`path-sensitive-arqa` 접두사)

외부 유입 데이터의 오염(Taint) 흐름을 추적하고 무기체계 소프트웨어 코딩 규칙 및 주요 보안 취약점(CWE) 표준을 탐지하기 위해 커스텀 정의된 전용 체커군입니다.

### AlwaysConstantConditionChecker

- **등록명:** `path-sensitive-arqa.AlwaysConstantCondition`
    
- **목적:** 조건문(`if`, `while` 등)의 평가 결과가 상위 흐름이나 상수에 의해 항상 참(True) 또는 거짓(False)으로 고정되는 논리적 결함을 추적합니다. 무기체계 SW 코딩 규칙에 따라 `if(1)`과 같은 명시적 리터럴도 결함으로 간주합니다.
    
- **옵션:**
    
    - `CheckMacros` (bool): 매크로(Macro) 내부에서 전개된 조건문(예: `assert` 내부)까지 검사할지 여부를 제어합니다 (기본값: `false`).
        

### ArrayBoundChecker

- **등록명:** `path-sensitive-arqa.ArrayBound`
    
- **목적:** 메모리 영역(배열 등)에 대해 범위 초과 접근(상한선 또는 하한선을 벗어난 읽기/쓰기)이 일어나는지 심볼릭 평가를 통해 경로 민감형(Path-sensitive)으로 검사합니다. 오염된 데이터(Taint)를 인덱스로 사용한 접근도 함께 탐지합니다.
    
- **옵션:** 고유 옵션은 제공되지 않습니다.
    

### CleartextStorageChecker

- **등록명:** `path-sensitive-arqa.CleartextStorage`
    
- **목적:** 비밀번호나 API 키, 토큰 등 민감한 정보가 암호화나 해싱 없이 파일, 로그 등에 평문으로 저장되는 취약점(CWE-312)을 추적합니다.
    
- **옵션:** 고유 옵션은 제공되지 않습니다.
    

### CleartextTransmissionChecker

- **등록명:** `path-sensitive-arqa.CleartextTransmission`
    
- **목적:** 민감한 정보가 변수에 저장된 후, 암호화 과정 없이 `send`, `write` 등 네트워크 전송 API(Sink)를 통해 평문으로 외부 방출되는 취약점(CWE-319)을 추적합니다.
    
- **옵션:** 고유 옵션은 제공되지 않습니다.
    

### ErrorMessageExposureChecker

- **등록명:** `path-sensitive-arqa.ErrorMessageExposure`
    
- **목적:** `strerror()`, `e.what()` 등을 통해 생성된 시스템 상세 에러 메시지가 변수를 타고 전파되다가, 외부 출력 함수(`send`, `printf` 등)로 그대로 방출되는 정보 노출 취약점(CWE-209)을 추적합니다.
    
- **옵션:** 고유 옵션은 제공되지 않습니다.
    

### PathTraversalChecker

- **등록명:** `path-sensitive-arqa.PathTraversal`
    
- **목적:** 검증되지 않은 외부 입력값(오염된 데이터)이 `fopen`, `remove`, `mkdir` 등 시스템의 파일 조작 API 경로 인자로 그대로 사용되어, 의도치 않은 시스템 파일에 접근할 수 있는 경로 순회(Path Traversal) 취약점을 검사합니다.
    
- **옵션:** 고유 옵션은 제공되지 않습니다.
    

### PointerSubtractionChecker

- **등록명:** `path-sensitive-arqa.PointerSubtraction`
    
- **목적:** 두 포인터 간의 뺄셈 연산(`ptr1 - ptr2`)을 검사합니다. 힙과 스택 등 서로 다른 메모리 공간에 있거나 아예 다른 변수를 가리키는 등, 명백하게 '다른 메모리 영역(Base Region)'에 속한 포인터들을 빼서 생기는 잘못된 거리 계산 오류를 방지합니다.
    
- **옵션:**
    
    - `StrictPointerSubtraction` (bool): 보수적 모드(Strict Mode)를 활성화하여, 안전성 증명 여부와 상관없이 모든 포인터 간의 뺄셈 연산을 원천적으로 차단하고 경고합니다 (기본값: `false`).
        

### SystemInfoExposureChecker

- **등록명:** `path-sensitive-arqa.SystemInfoExposure`
    
- **목적:** 환경 변수, OS 버전, 호스트명 등 중요 시스템 설정 정보(`getenv`, `uname` 등)를 획득한 후, 암호화나 해싱 없이 네트워크(`send`) 등을 통해 평문으로 외부 유출시키는 취약점(CWE-497)을 차단합니다.
    
- **옵션:** 고유 옵션은 제공되지 않습니다.
    

### TaintedLoopBoundChecker

- **등록명:** `path-sensitive-arqa.TaintedLoopBound`
    
- **목적:** 외부에서 조작할 수 있는 오염된 데이터가 `for`, `while` 등 반복문의 종료 조건을 제어하는 데 사용되는 취약점을 탐지합니다. CPU 자원 고갈 공격을 방어하기 위해 수학적 상한선 증명이 존재하는지 판단합니다.
    
- **옵션:**
    
    - `MaxSafeLoopBound` (int): 안전하다고 판단할 수 있는 루프의 최대 허용 반복 횟수를 지정합니다 (기본값: `100,000`).
        

### TaintedMemoryAllocationChecker

- **등록명:** `path-sensitive-arqa.TaintedMemoryAllocation`
    
- **목적:** 오염된 입력값이 `malloc`, `HeapAlloc`과 같은 C 메모리 할당 함수나 C++의 배열 할당 연산자(`new[]`)의 크기 인자로 직접 사용되어, 막대한 메모리를 점유해 시스템을 다운(OOM)시키는 취약점(CWE-789/CWE-400)을 추적합니다.
    
- **옵션:**
    
    - `MaxSafeAllocSize` (int): 단일 동적 메모리 할당 함수 호출 시 허용되는 안전 상한선 바이트 크기입니다 (기본값: `10,485,760` = 10MB).
        
    - `MaxSafeArraySize` (int): C++ `new[]` 배열 연산자로 할당 시 허용되는 안전 상한선 배열 요소 개수입니다 (기본값: `1,000,000`).
        

### TaintedThreadDelayChecker

- **등록명:** `path-sensitive-arqa.TaintedThreadDelay`
    
- **목적:** 외부 입력값이 `sleep`, `usleep`, `Sleep` 등 스레드 대기 시간을 결정하는 함수의 인자로 사용되어 스레드 풀을 고갈시킬 수 있는 취약점(CWE-400)을 탐지합니다.
    
- **옵션:**
    
    - `MaxSafeDelay` (int): 안전하다고 간주되는 최대 대기 시간 한도입니다 (API 단위 기준, 기본값: `10000ms` = 10초).
        

### ThreadLockChecker

- **등록명:** `path-sensitive-arqa.ThreadLock`
    
- **목적:** 멀티스레드 환경에서 동기화 객체(`pthread_mutex_t`, `CriticalSection` 등)의 락 상태를 추적하여, 이중 잠금(Double Lock), 잠기지 않은 객체에 대한 잘못된 해제, 함수 종료 전 락을 풀지 않아 발생하는 데드락(동기화 객체 누수) 등을 잡아냅니다.
    
- **옵션:** 고유 옵션은 제공되지 않습니다.
    

### TOCTOUChecker

- **등록명:** `path-sensitive-arqa.TOCTOU`
    
- **목적:** 파일 검사(`Check`: `access`, `stat`)와 실제 사용(`Use`: `open`, `chmod`) 사이에 발생하는 시간차(Gap)를 악용해 공격자가 심볼릭 링크로 자원을 바꿔치기하는 TOCTOU 경쟁 조건 취약점(CWE-367)을 찾습니다.
    
- **옵션:** 고유 옵션은 제공되지 않습니다.
    

### UncheckedErrorReturnChecker

- **등록명:** `path-sensitive-arqa.UncheckedErrorReturn`
    
- **목적:** 권한 제어, 파일 오픈, 소켓 통신 등 중요 시스템 API의 반환값(오류 코드)을 조건문에서 전혀 검사하지 않고 무시하거나(CWE-392), 에러 여부를 점검하기도 전에 다른 함수의 인자로 넘겨버리는 취약점(CWE-755)을 추적합니다.
    
- **옵션:**
    
    - `WarnOnUncheckedUse` (bool): 에러 검사 전의 반환값을 다른 함수 인자로 사용하는 Use-Before-Check 형태(Strict 모드)를 경고할지 지정합니다 (기본값: `true`).
        

### UnsafeCommandExecutionChecker

- **등록명:** `path-sensitive-arqa.UnsafeCommandExecution`
    
- **목적:** 검증되지 않은 오염된 외부 입력값이 시스템 셸을 호출하는 명령어 실행 API(`system`, `popen`, `execv` 등)에 파라미터로 전달되어 임의의 명령을 실행할 수 있는 취약점을 탐지합니다. `atoi` 등의 정화(Sanitizer) 로직을 거친 숫자는 안전한 것으로 간주합니다.
    
- **옵션:** 고유 옵션은 제공되지 않습니다.
    

### UnsafeDatabaseQueryChecker

- **등록명:** `path-sensitive-arqa.UnsafeDatabaseQuery`
    
- **목적:** 외부 입력값이 이스케이프 처리(예: `mysql_real_escape_string`)나 변환 없이, 데이터베이스 쿼리 실행 API(SQLite, MySQL, PostgreSQL, ODBC, OCI 등)의 명령 문자열로 직접 사용되는 보안 취약점(SQL Injection 등)을 찾아냅니다.
    
- **옵션:** 고유 옵션은 제공되지 않습니다.
    

### UnsafeResourceAccessChecker

- **등록명:** `path-sensitive-arqa.UnsafeResourceAccess`
    
- **목적:** 오염된 외부 데이터가 네트워크 접속 정보(`getaddrinfo`, `connect`), 환경 변수 세팅(`putenv`, `SetEnvironmentVariable`), 윈도우 레지스트리 조작(`RegCreateKey`, `RegSetValueEx`) 등 시스템의 중요 자원을 지정하는 API의 식별자로 무분별하게 사용되는 취약점을 차단합니다.
    
- **옵션:** 고유 옵션은 제공되지 않습니다.
    

23456