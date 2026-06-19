# Cppcheck 실험 검증 코드 및 로우데이터 - 공통 적용 (타입 변환)

본 문서는 DAPA 무기체계 소프트웨어 코딩 규칙 중 **공통 적용 (타입 변환)**에 속하는 규칙들의 실제 테스트 코드와 Cppcheck 정적 분석 결과(진단 경고 로우데이터)를 매핑한 자료입니다.

---

## 📌 Rule 25: 선언된 데이터 형으로 표현할 수 있는 숫자의 영역을 초과하는 값을 할당하지 말아야 한다.

* **세부 조항**: 가
* **매핑 체커 (Clang-Tidy)**: `NoOutOfRangeAssignmentCheck`
* **최종 진단 판정**: **⚠️ 미탐 (파생 경고만 발생) - 결함은 미탐지하고 부수적 경고만 발생**

### 💻 검증 소스코드
> 파일 위치: `dapa_test_suite.cpp` (L388 ~ L394)

```cpp
 388 | namespace Rule_25 {
 389 |     void foo(short n)
 390 |     {
 391 |         short x;
 392 |         x = 10000 * 4;  // ❌ 40000은 short의 표현 범위(32767)를 초과
 393 |     }
 394 | }
```

### 🔍 Cppcheck 진단 경고 로그 (Raw Output)
| 라인 (Line) | 중요도 (Severity) | 경고 ID (Warning ID) | 분류 | 진단 메시지 (Message) |
| :---: | :---: | :---: | :---: | :--- |
| 392 | style | `unreadVariable` | ⚠️ 파생 경고 (Collateral) | Variable 'x' is assigned a value that is never used. |

---

## 📌 Rule 26: 가변인수를 받는 함수의 Conversion 지시자와 Argument의 type은 동일해야한다.

* **세부 조항**: 나
* **매핑 체커 (Clang-Tidy)**: `VarargsFormatCheck`
* **최종 진단 판정**: **✅ 정탐 (True Positive) - 의도된 결함 정확히 검출**

### 💻 검증 소스코드
> 파일 위치: `dapa_test_suite.cpp` (L400 ~ L407)

```cpp
 400 | namespace Rule_26 {
 401 |     #include <stdio.h>
 402 | 
 403 |     extern void foo(int n, unsigned char *s)
 404 |     {
 405 |         printf("The total is %s", n);  // ❌ %s는 문자열 포인터 기대, int 전달됨
 406 |     }
 407 | }
```

### 🔍 Cppcheck 진단 경고 로그 (Raw Output)
| 라인 (Line) | 중요도 (Severity) | 경고 ID (Warning ID) | 분류 | 진단 메시지 (Message) |
| :---: | :---: | :---: | :---: | :--- |
| 405 | warning | `invalidPrintfArgType_s` | ✅ 정탐 경고 (Genuine) | %s in format string (no. 1) requires 'char *' but the argument type is 'signed int'. |

---

## 📌 Rule 27: 가변인수를 받는 함수의 Conversion 지시자와 Argument의 개수는 동일해야한다.

* **세부 조항**: 다
* **매핑 체커 (Clang-Tidy)**: `VarargsFormatCheck`
* **최종 진단 판정**: **✅ 정탐 (True Positive) - 의도된 결함 정확히 검출**

### 💻 검증 소스코드
> 파일 위치: `dapa_test_suite.cpp` (L413 ~ L420)

```cpp
 413 | namespace Rule_27 {
 414 |     #include <stdio.h>
 415 | 
 416 |     extern void foo(int test)
 417 |     {
 418 |         printf("The score for test %d is %d", test);  // ❌ %d가 2개인데 인자는 1개
 419 |     }
 420 | }
```

### 🔍 Cppcheck 진단 경고 로그 (Raw Output)
| 라인 (Line) | 중요도 (Severity) | 경고 ID (Warning ID) | 분류 | 진단 메시지 (Message) |
| :---: | :---: | :---: | :---: | :--- |
| 418 | error | `wrongPrintfScanfArgNum` | ✅ 정탐 경고 (Genuine) | printf format string requires 2 parameters but only 1 is given. |

---

## 📌 Rule 28: Object 저장 값을 표현할 수 없는 데이터로의 형 변환을 하지 말아야 한다.

* **세부 조항**: 라
* **매핑 체커 (Clang-Tidy)**: `RepresentableCastCheck`
* **최종 진단 판정**: **⚠️ 미탐 (파생 경고만 발생) - 결함은 미탐지하고 부수적 경고만 발생**

### 💻 검증 소스코드
> 파일 위치: `dapa_test_suite.cpp` (L426 ~ L435)

```cpp
 426 | namespace Rule_28 {
 427 |     void foo(signed int sia)
 428 |     {
 429 |         signed char sca;
 430 |         if (sia > 127)
 431 |         {
 432 |             sca = sia;  // ❌ signed char은 -128~127만 표현 가능 → 데이터 손실 발생
 433 |         }
 434 |     }
 435 | }
```

### 🔍 Cppcheck 진단 경고 로그 (Raw Output)
| 라인 (Line) | 중요도 (Severity) | 경고 ID (Warning ID) | 분류 | 진단 메시지 (Message) |
| :---: | :---: | :---: | :---: | :--- |
| 429 | style | `variableScope` | ⚠️ 파생 경고 (Collateral) | The scope of the variable 'sca' can be reduced. |
| 432 | style | `unreadVariable` | ⚠️ 파생 경고 (Collateral) | Variable 'sca' is assigned a value that is never used. |

---

## 📌 Rule 29: 음수 값을 unsigned type으로 변환해서는 안된다.

* **세부 조항**: 마
* **매핑 체커 (Clang-Tidy)**: `UnknownChecker`
* **최종 진단 판정**: **⚠️ 미탐 (파생 경고만 발생) - 결함은 미탐지하고 부수적 경고만 발생**

### 💻 검증 소스코드
> 파일 위치: `dapa_test_suite.cpp` (L441 ~ L450)

```cpp
 441 | namespace Rule_29 {
 442 |     void foo(int sia)
 443 |     {
 444 |         unsigned char ucx;
 445 |         if (sia < 0)
 446 |         {
 447 |             ucx = sia;  // ❌ 음수를 unsigned로 변환 → 값 왜곡 발생
 448 |         }
 449 |     }
 450 | }
```

### 🔍 Cppcheck 진단 경고 로그 (Raw Output)
| 라인 (Line) | 중요도 (Severity) | 경고 ID (Warning ID) | 분류 | 진단 메시지 (Message) |
| :---: | :---: | :---: | :---: | :--- |
| 444 | style | `variableScope` | ⚠️ 파생 경고 (Collateral) | The scope of the variable 'ucx' can be reduced. |
| 447 | style | `unreadVariable` | ⚠️ 파생 경고 (Collateral) | Variable 'ucx' is assigned a value that is never used. |

---

## 📌 Rule 30: Character 문자열과 Wide character 문자열을 혼용하지 말아야 한다.

* **세부 조항**: 바
* **매핑 체커 (Clang-Tidy)**: `NoMixedCharWideStringCheck`
* **최종 진단 판정**: **❌ 미탐 (Silent / False Negative) - 아무런 경고도 감지되지 않음**

### 💻 검증 소스코드
> 파일 위치: `dapa_test_suite.cpp` (L456 ~ L465)

```cpp
 456 | namespace Rule_30 {
 457 |     #include <stddef.h>
 458 | 
 459 |     #define HN "Hello"
 460 |     #define WN " World.\n"
 461 |     #define HW L"Hello"
 462 |     #define WW L" World.\n"
 463 | 
 464 |     char *t2 = HN WW;  // ❌ HN은 char*, WW는 wchar_t* → 타입 혼용
 465 | }
```

### 🔍 Cppcheck 진단 경고 로그 (Raw Output)
> ❌ **검출된 경고 없음 (Silent)**

---

## 📌 Rule 31: 포인터 Cast의 결과로 인한 이전 포인터의 CONST 특성의 상실을 유의해야한다.

* **세부 조항**: 사
* **매핑 체커 (Clang-Tidy)**: `PointerCvQualifierDropCheck`
* **최종 진단 판정**: **⚠️ 미탐 (파생 경고만 발생) - 결함은 미탐지하고 부수적 경고만 발생**

### 💻 검증 소스코드
> 파일 위치: `dapa_test_suite.cpp` (L471 ~ L477)

```cpp
 471 | namespace Rule_31 {
 472 |     void foo(const int *pci)
 473 |     {
 474 |         int *pi;
 475 |         pi = (int *)pci;  // ❌ const 속성 제거 → 위험한 동작
 476 |     }
 477 | }
```

### 🔍 Cppcheck 진단 경고 로그 (Raw Output)
| 라인 (Line) | 중요도 (Severity) | 경고 ID (Warning ID) | 분류 | 진단 메시지 (Message) |
| :---: | :---: | :---: | :---: | :--- |
| 475 | style | `cstyleCast` | ⚠️ 파생 경고 (Collateral) | C-style pointer casting |
| 474 | style | `constVariablePointer` | ⚠️ 파생 경고 (Collateral) | Variable 'pi' can be declared as pointer to const |
| 475 | style | `unreadVariable` | ⚠️ 파생 경고 (Collateral) | Variable 'pi' is assigned a value that is never used. |

---

## 📌 Rule 32: 포인터 Cast의 결과로 인한 이전 포인터의 volatile 특성 상실을 유의해야 한다.

* **세부 조항**: 아
* **매핑 체커 (Clang-Tidy)**: `PointerCvQualifierDropCheck`
* **최종 진단 판정**: **⚠️ 미탐 (파생 경고만 발생) - 결함은 미탐지하고 부수적 경고만 발생**

### 💻 검증 소스코드
> 파일 위치: `dapa_test_suite.cpp` (L483 ~ L489)

```cpp
 483 | namespace Rule_32 {
 484 |     void foo(volatile int *pvi)
 485 |     {
 486 |         int *pi;
 487 |         pi = (int *)pvi;  // ❌ volatile 속성 제거 → 위험한 동작
 488 |     }
 489 | }
```

### 🔍 Cppcheck 진단 경고 로그 (Raw Output)
| 라인 (Line) | 중요도 (Severity) | 경고 ID (Warning ID) | 분류 | 진단 메시지 (Message) |
| :---: | :---: | :---: | :---: | :--- |
| 487 | style | `cstyleCast` | ⚠️ 파생 경고 (Collateral) | C-style pointer casting |
| 486 | style | `constVariablePointer` | ⚠️ 파생 경고 (Collateral) | Variable 'pi' can be declared as pointer to const |
| 487 | style | `unreadVariable` | ⚠️ 파생 경고 (Collateral) | Variable 'pi' is assigned a value that is never used. |

---
