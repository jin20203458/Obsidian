# Cppcheck 실험 검증 코드 및 로우데이터 - 공통 적용 (타입 변환)

본 문서는 DAPA 무기체계 소프트웨어 코딩 규칙 중 **공통 적용 (타입 변환)**에 속하는 규칙들의 실제 테스트 코드와 Cppcheck 정적 분석 결과(진단 경고 로우데이터)를 매핑한 자료입니다.

---

## 📌 Rule 25: 선언된 데이터 형으로 표현할 수 있는 숫자의 영역을 초과하는 값을 할당하지 말아야 한다.

* **세부 조항**: 가
* **매핑 체커 (Clang-Tidy)**: `NoOutOfRangeAssignmentCheck`
* **최종 진단 판정**: **⚠️ 미탐 (파생 경고만 발생) - 결함은 미탐지하고 부수적 경고만 발생**

### 💻 검증 소스코드
> 파일 위치: `dapa_test_suite.cpp` (L417 ~ L423)

```cpp
 417 | namespace Rule_25 {
 418 |     void foo(short n)
 419 |     {
 420 |         short x;
 421 |         x = 10000 * 4;  // ❌ 40000은 short의 표현 범위(32767)를 초과
 422 |     }
 423 | }
```

### 🔍 Cppcheck 진단 경고 로그 (Raw Output)
| 라인 (Line) | 중요도 (Severity) | 경고 ID (Warning ID) |          분류           | 진단 메시지 (Message)                                     |
| :-------: | :------------: | :----------------: | :-------------------: | :--------------------------------------------------- |
|    421    |     style      |  `unreadVariable`  | ⚠️ 파생 경고 (Collateral) | Variable 'x' is assigned a value that is never used. |

---

## 📌 Rule 26: 가변인수를 받는 함수의 Conversion 지시자와 Argument의 type은 동일해야한다.

* **세부 조항**: 나
* **매핑 체커 (Clang-Tidy)**: `VarargsFormatCheck`
* **최종 진단 판정**: **✅ 정탐 (True Positive) - 의도된 결함 정확히 검출**

### 💻 검증 소스코드
> 파일 위치: `dapa_test_suite.cpp` (L429 ~ L436)

```cpp
 429 | namespace Rule_26 {
 430 |     #include <stdio.h>
 431 | 
 432 |     extern void foo(int n, unsigned char *s)
 433 |     {
 434 |         printf("The total is %s", n);  // ❌ %s는 문자열 포인터 기대, int 전달됨
 435 |     }
 436 | }
```

### 🔍 Cppcheck 진단 경고 로그 (Raw Output)
| 라인 (Line) | 중요도 (Severity) | 경고 ID (Warning ID) | 분류 | 진단 메시지 (Message) |
| :---: | :---: | :---: | :---: | :--- |
| 434 | warning | `invalidPrintfArgType_s` | ✅ 정탐 경고 (Genuine) | %s in format string (no. 1) requires 'char *' but the argument type is 'signed int'. |

---

## 📌 Rule 27: 가변인수를 받는 함수의 Conversion 지시자와 Argument의 개수는 동일해야한다.

* **세부 조항**: 다
* **매핑 체커 (Clang-Tidy)**: `VarargsFormatCheck`
* **최종 진단 판정**: **✅ 정탐 (True Positive) - 의도된 결함 정확히 검출**

### 💻 검증 소스코드
> 파일 위치: `dapa_test_suite.cpp` (L442 ~ L449)

```cpp
 442 | namespace Rule_27 {
 443 |     #include <stdio.h>
 444 | 
 445 |     extern void foo(int test)
 446 |     {
 447 |         printf("The score for test %d is %d", test);  // ❌ %d가 2개인데 인자는 1개
 448 |     }
 449 | }
```

### 🔍 Cppcheck 진단 경고 로그 (Raw Output)
| 라인 (Line) | 중요도 (Severity) | 경고 ID (Warning ID) | 분류 | 진단 메시지 (Message) |
| :---: | :---: | :---: | :---: | :--- |
| 447 | error | `wrongPrintfScanfArgNum` | ✅ 정탐 경고 (Genuine) | printf format string requires 2 parameters but only 1 is given. |

---

## 📌 Rule 28: Object 저장 값을 표현할 수 없는 데이터로의 형 변환을 하지 말아야 한다.

* **세부 조항**: 라
* **매핑 체커 (Clang-Tidy)**: `RepresentableCastCheck`
* **최종 진단 판정**: **⚠️ 미탐 (파생 경고만 발생) - 결함은 미탐지하고 부수적 경고만 발생**

### 💻 검증 소스코드
> 파일 위치: `dapa_test_suite.cpp` (L455 ~ L464)

```cpp
 455 | namespace Rule_28 {
 456 |     void foo(signed int sia)
 457 |     {
 458 |         signed char sca;
 459 |         if (sia > 127)
 460 |         {
 461 |             sca = sia;  // ❌ signed char은 -128~127만 표현 가능 → 데이터 손실 발생
 462 |         }
 463 |     }
 464 | }
```

### 🔍 Cppcheck 진단 경고 로그 (Raw Output)
| 라인 (Line) | 중요도 (Severity) | 경고 ID (Warning ID) | 분류 | 진단 메시지 (Message) |
| :---: | :---: | :---: | :---: | :--- |
| 458 | style | `variableScope` | ⚠️ 파생 경고 (Collateral) | The scope of the variable 'sca' can be reduced. |
| 461 | style | `unreadVariable` | ⚠️ 파생 경고 (Collateral) | Variable 'sca' is assigned a value that is never used. |

---

## 📌 Rule 29: 음수 값을 unsigned type으로 변환해서는 안된다.

* **세부 조항**: 마
* **매핑 체커 (Clang-Tidy)**: `UnknownChecker`
* **최종 진단 판정**: **⚠️ 미탐 (파생 경고만 발생) - 결함은 미탐지하고 부수적 경고만 발생**

### 💻 검증 소스코드
> 파일 위치: `dapa_test_suite.cpp` (L470 ~ L479)

```cpp
 470 | namespace Rule_29 {
 471 |     void foo(int sia)
 472 |     {
 473 |         unsigned char ucx;
 474 |         if (sia < 0)
 475 |         {
 476 |             ucx = sia;  // ❌ 음수를 unsigned로 변환 → 값 왜곡 발생
 477 |         }
 478 |     }
 479 | }
```

### 🔍 Cppcheck 진단 경고 로그 (Raw Output)
| 라인 (Line) | 중요도 (Severity) | 경고 ID (Warning ID) | 분류 | 진단 메시지 (Message) |
| :---: | :---: | :---: | :---: | :--- |
| 473 | style | `variableScope` | ⚠️ 파생 경고 (Collateral) | The scope of the variable 'ucx' can be reduced. |
| 476 | style | `unreadVariable` | ⚠️ 파생 경고 (Collateral) | Variable 'ucx' is assigned a value that is never used. |

---

## 📌 Rule 30: Character 문자열과 Wide character 문자열을 혼용하지 말아야 한다.

* **세부 조항**: 바
* **매핑 체커 (Clang-Tidy)**: `NoMixedCharWideStringCheck`
* **최종 진단 판정**: **❌ 미탐 (Silent / False Negative) - 아무런 경고도 감지되지 않음**

### 💻 검증 소스코드
> 파일 위치: `dapa_test_suite.cpp` (L485 ~ L494)

```cpp
 485 | namespace Rule_30 {
 486 |     #include <stddef.h>
 487 | 
 488 |     #define HN "Hello"
 489 |     #define WN " World.\n"
 490 |     #define HW L"Hello"
 491 |     #define WW L" World.\n"
 492 | 
 493 |     const wchar_t *t2 = HN WW;  // ❌ HN은 char*, WW는 wchar_t* → 타입 혼용 (const wchar_t* to compile on C++)
 494 | }
```

### 🔍 Cppcheck 진단 경고 로그 (Raw Output)
> ❌ **검출된 경고 없음 (Silent)**

---

## 📌 Rule 31: 포인터 Cast의 결과로 인한 이전 포인터의 CONST 특성의 상실을 유의해야한다.

* **세부 조항**: 사
* **매핑 체커 (Clang-Tidy)**: `PointerCvQualifierDropCheck`
* **최종 진단 판정**: **⚠️ 미탐 (파생 경고만 발생) - 결함은 미탐지하고 부수적 경고만 발생**

### 💻 검증 소스코드
> 파일 위치: `dapa_test_suite.cpp` (L500 ~ L506)

```cpp
 500 | namespace Rule_31 {
 501 |     void foo(const int *pci)
 502 |     {
 503 |         int *pi;
 504 |         pi = (int *)pci;  // ❌ const 속성 제거 → 위험한 동작
 505 |     }
 506 | }
```

### 🔍 Cppcheck 진단 경고 로그 (Raw Output)
| 라인 (Line) | 중요도 (Severity) | 경고 ID (Warning ID) | 분류 | 진단 메시지 (Message) |
| :---: | :---: | :---: | :---: | :--- |
| 504 | style | `cstyleCast` | ⚠️ 파생 경고 (Collateral) | C-style pointer casting |
| 503 | style | `constVariablePointer` | ⚠️ 파생 경고 (Collateral) | Variable 'pi' can be declared as pointer to const |
| 504 | style | `unreadVariable` | ⚠️ 파생 경고 (Collateral) | Variable 'pi' is assigned a value that is never used. |

---

## 📌 Rule 32: 포인터 Cast의 결과로 인한 이전 포인터의 volatile 특성 상실을 유의해야 한다.

* **세부 조항**: 아
* **매핑 체커 (Clang-Tidy)**: `PointerCvQualifierDropCheck`
* **최종 진단 판정**: **⚠️ 미탐 (파생 경고만 발생) - 결함은 미탐지하고 부수적 경고만 발생**

### 💻 검증 소스코드
> 파일 위치: `dapa_test_suite.cpp` (L512 ~ L518)

```cpp
 512 | namespace Rule_32 {
 513 |     void foo(volatile int *pvi)
 514 |     {
 515 |         int *pi;
 516 |         pi = (int *)pvi;  // ❌ volatile 속성 제거 → 위험한 동작
 517 |     }
 518 | }
```

### 🔍 Cppcheck 진단 경고 로그 (Raw Output)
| 라인 (Line) | 중요도 (Severity) | 경고 ID (Warning ID) | 분류 | 진단 메시지 (Message) |
| :---: | :---: | :---: | :---: | :--- |
| 516 | style | `cstyleCast` | ⚠️ 파생 경고 (Collateral) | C-style pointer casting |
| 515 | style | `constVariablePointer` | ⚠️ 파생 경고 (Collateral) | Variable 'pi' can be declared as pointer to const |
| 516 | style | `unreadVariable` | ⚠️ 파생 경고 (Collateral) | Variable 'pi' is assigned a value that is never used. |

---
