


## 진행사항

CWE-22 : **경로 조작 및 자원 삽입**
[path-sensitive-arqa.PathTraversal] CSA

**CWE-59 : 링크 따라가기** 
[ast-improper-link-resolution] CT

CWE-78 : **운영체제 명령어 삽입**
[path-sensitive-arqa.UnsafeCommandExecution] CSA

CWE-89 : **SQL 삽입**
[path-sensitive-arqa.UnsafeDatabaseQuery] CSA

CWE-99 : **자원 식별자의 부적절한 제어** 
[path-sensitive-arqa.UnsafeResourceAccess] CSA

**CWE-119 : 버퍼 오버플로우** <font color="#ff0000">(낙담 : 클래스 등급)</font>
[path-sensitive-arqa.ArrayBound] CSA

**CWE-134 : 포맷 스트링 삽입** 
[ast-varargs-format] CT

**CWE-170 : 부적절한 널 종료** 
[ast-not-null-terminated-result] CT 
[ast-char-array-string-size] CT

**CWE-190 : 정수 오버플로우 (런타임 오버플로우 보완이 필요함)** 
[ast-no-out-of-range-assignment] CT

CWE-209 : **오류 메시지를 통한 정보 노출**
[path-sensitive-arqa.ErrorMessageExposure]

---

**CWE-259 : 하드코딩된 비밀번호 사용** 
[ast-hardcoded-password] CT

**CWE-285 : 부적절한 인가** <font color="#ff0000">(낙담 : 클래스 등급)</font>
[ast-improper-authorization] CT

**CWE-306 : 중요 기능에 대한 인증 누락** 
[ast-missing-authentication] CT

**CWE-307 : 과도한 인증 시도 제한 부적절** 
[ast-excessive-authentication] CT

CWE-312 : **민감 정보의 평문 저장** <font color="#ffff00">(주의 : 클래스 등급)</font>
[path-sensitive-arqa.CleartextStorage]

**CWE-319 : 민감 정보의 평문 전송** 
[ast-cleartext-transmission] CT

**CWE-321 : 하드코딩된 암호화 키 사용** 
[ast-hardcoded-key] CT

**CWE-327 : 취약한 암호화 알고리즘 사용** <font color="#ffff00">(주의 : 클래스 등급)</font>
[ast-broken-crypto] CT

**CWE-330 : 취약한 난수 생성** <font color="#ff0000">(낙담 : 클래스 등급)</font>
[ast-insufficient-random] CT

**CWE-369 : 0으로 나누기** 
[cfg-nonzero-divisor-guard] CT 
[path-sensitive-core.DivideZero] CSA

---

CWE-367 : **검사 시점과 사용 시점의 경쟁 조건**
[path-sensitive-arqa.TOCTOU] CSA

**CWE-390 : 오류 상황 감지 후 조치 부재** 
[ast-error-without-action] CT

CWE-400 : 통제되지 않은 자원 소비 <font color="#ff0000">(낙담 : 클래스 등급)</font>
- CWE-789/770 [path-sensitive-arqa.TaintedMemoryAllocation] CSA 
- CWE-834 [path-sensitive-arqa.TaintedThreadDelay] CSA 

- C   CWE-401 [path-sensitive-unix.Malloc] CSA 
- C++ CWE-401 [path-sensitive-cplusplus.NewDelete] CSA
- [path-sensitive-unix.Stream] CSA

- CWE-835 [path-sensitive-arqa.TaintedLoopBound] CSA


CWE-404 : **부적절한 자원 해제** <font color="#ffff00">(주의 : 클래스 등급)</font>
- C   CWE-401 [path-sensitive-unix.Malloc] CSA 
- C++ CWE-401 [path-sensitive-cplusplus.NewDelete] CSA
- [path-sensitive-unix.Stream] CSA
- [path-sensitive-arqa.ThreadLock] CSA


**CWE-415 : 삭제된 변수 삭제** 
C   [path-sensitive-unix.Malloc] CSA 
C++ [path-sensitive-cplusplus.NewDelete] CSA

**CWE-416 : 변수 삭제후 사용** 
C   [path-sensitive-unix.Malloc] CSA 
C++ [path-sensitive-cplusplus.NewDelete] CSA


**CWE-457 : 초기화되지 않은 변수 사용** 
[path-sensitive-core.uninitialized.Assign] CSA 
[cfg-init-before-use] CT

CWE-467 : **포인터 타입에 대한 sizeof() 사용**
[ast-sizeof-on-pointer] CT

CWE-469 : **크기 결정을 위한 포인터 뺄셈의 오용**
[path-sensitive-arqa.PointerSubtraction] CSA

**CWE-476 : 포인터가 NULL이 되는 경로 추적** 
[path-sensitive-core.NullDereferenc] CSA

---

**CWE-489 : 활성화된 디버그 코드** 
[ast-leftover-debug-code] CT

**CWE-494 : 무결성 검사 없는 코드 다운로드** 
[ast-insecure-code-download] CT

**CWE-495 : (C++ 전용) Public 메소드에서 Private 반환
[ast-private-data-exposure] CT

**CWE-496 : (C++ 전용) **Public 데이터를 Private 배열에 할당**
[ast-public-data-assigned] CT

CWE-497 : **시스템 데이터 정보 노출** 
[path-sensitive-arqa.SystemInfoExposure]

**CWE-521** : **취약한 패스워드 요구조건**
[ast-weak-password-requirements] CT

CWE-562 : **스택 변수 주소 반환**
[ast-return-stack-address] CT
[path-sensitive-core.StackAddressEscape] CSA

CWE-587 : **포인터에 고정 주소 할당**
[ast-fixed-address-assignment] CT

CWE-615 : **주석을 통한 정보 노출**
[lex-suspicious-comment] CT

CWE-628 : **잘못된 인자로 함수 호출**
[ast-function-call-argument-consistency] CT
[ast-varargs-format] CT

---

CWE-676 : **위험한 함수 사용**
[ast-dangerous-function] CT

CWE-732 : **중요 자원에 대한 잘못된 권한 설정** <font color="#ffff00">(주의 : 클래스 등급)</font>
[ast-incorrect-permission-assignment] CT

CWE-755 : **예외 조건의 부적절한 처리 <font color="#ff0000">(낙담 : 클래스 등급)</font>
[path-sensitive-arqa.ErrorMessageExposure]
CWE-404(전체)
[ast-unused-exception-handler]
[ast-main-unhandled-throw]
[ast-destructor-exception-safety]
[path-sensitive-arqa.UncheckedErrorReturn]

**CWE-759 : 솔트 없는 단방향 해시 함수 사용**  
[ast-no-salted-hash] CT

016