# Cppcheck 실험 검증 코드 및 로우데이터 - 공통 적용 (포인터 및 배열)

본 문서는 DAPA 무기체계 소프트웨어 코딩 규칙 중 **공통 적용 (포인터 및 배열)**에 속하는 규칙들의 실제 테스트 코드와 Cppcheck 정적 분석 결과(진단 경고 로우데이터)를 매핑한 자료입니다.

---

## 📌 Rule 33: 포인터는 참조전에 NULL 여부를 반드시 확인하여야 한다.

* **세부 조항**: 가
* **매핑 체커 (Clang-Tidy)**: `NullDereferenceGuardCheck`
* **최종 진단 판정**: **⚠️ 미탐 (파생 경고만 발생) - 결함은 미탐지하고 부수적 경고만 발생**

### 💻 검증 소스코드
> 파일 위치: `dapa_test_suite.cpp` (L495 ~ L503)

```cpp
 495 | namespace Rule_33 {
 496 |     #define NULL (void *)0
 497 | 
 498 |     void foo(int *p)
 499 |     {
 500 |         int r;
 501 |         r = *p;  // ❌ p가 NULL일 경우 → 시스템 크래시 발생 가능
 502 |     }
 503 | }
```

### 🔍 Cppcheck 진단 경고 로그 (Raw Output)
| 라인 (Line) | 중요도 (Severity) | 경고 ID (Warning ID) | 분류 | 진단 메시지 (Message) |
| :---: | :---: | :---: | :---: | :--- |
| 501 | style | `unreadVariable` | ⚠️ 파생 경고 (Collateral) | Variable 'r' is assigned a value that is never used. |

---

## 📌 Rule 34: 지역변수의 주소값을 더 넓은 scope를 가진 변수에 할당하지 말아야 한다.

* **세부 조항**: 나
* **매핑 체커 (Clang-Tidy)**: `ReturnStackAddressCheck`
* **최종 진단 판정**: **✅ 정탐 (True Positive) - 의도된 결함 정확히 검출**

### 💻 검증 소스코드
> 파일 위치: `dapa_test_suite.cpp` (L509 ~ L517)

```cpp
 509 | namespace Rule_34 {
 510 |     extern int *pi;
 511 | 
 512 |     void foo(void)
 513 |     {
 514 |         int a;
 515 |         pi = &a;  // ❌ a는 foo 함수가 끝나면 사라짐 → pi는 댕글링 포인터가 됨
 516 |     }
 517 | }
```

### 🔍 Cppcheck 진단 경고 로그 (Raw Output)
| 라인 (Line) | 중요도 (Severity) | 경고 ID (Warning ID) | 분류 | 진단 메시지 (Message) |
| :---: | :---: | :---: | :---: | :--- |
| 515 | error | `danglingLifetime` | ✅ 정탐 경고 (Genuine) | Non-local variable 'pi' will use pointer to local variable 'a'. |

---

## 📌 Rule 35: 지역변수의 주소값을 함수의 리턴값으로 사용하지 말아야 한다.

* **세부 조항**: 다
* **매핑 체커 (Clang-Tidy)**: `ReturnStackAddressCheck`
* **최종 진단 판정**: **✅ 정탐 (True Positive) - 의도된 결함 정확히 검출**

### 💻 검증 소스코드
> 파일 위치: `dapa_test_suite.cpp` (L523 ~ L529)

```cpp
 523 | namespace Rule_35 {
 524 |     int *func(void)
 525 |     {
 526 |         int a = 0;
 527 |         return &a;  // ❌ a는 함수 종료와 함께 사라짐 → 댕글링 포인터
 528 |     }
 529 | }
```

### 🔍 Cppcheck 진단 경고 로그 (Raw Output)
| 라인 (Line) | 중요도 (Severity) | 경고 ID (Warning ID) | 분류 | 진단 메시지 (Message) |
| :---: | :---: | :---: | :---: | :--- |
| 527 | error | `returnDanglingLifetime` | ✅ 정탐 경고 (Genuine) | Returning pointer to local variable 'a' that will be invalid when returning. |

---

## 📌 Rule 36: 선언된 배열의 크기를 초과하는 인덱스 값을 사용하지 말아야 한다.

* **세부 조항**: 라
* **매핑 체커 (Clang-Tidy)**: `ArrayIndexBoundCheck`
* **최종 진단 판정**: **✅ 정탐 (True Positive) - 의도된 결함 정확히 검출**

### 💻 검증 소스코드
> 파일 위치: `dapa_test_suite.cpp` (L535 ~ L546)

```cpp
 535 | namespace Rule_36 {
 536 |     extern int a[10];
 537 | 
 538 |     extern int foo(void)
 539 |     {
 540 |         int r;
 541 |         r = a[10];       // ❌ 유효한 인덱스는 0~9 → 범위 초과
 542 |         a[10] = 0;       // ❌ 쓰기 접근도 위험
 543 |         *(a + 10) = 0;   // ❌ 포인터 연산도 동일하게 위험
 544 |         return r;
 545 |     }
 546 | }
```

### 🔍 Cppcheck 진단 경고 로그 (Raw Output)
| 라인 (Line) | 중요도 (Severity) | 경고 ID (Warning ID) | 분류 | 진단 메시지 (Message) |
| :---: | :---: | :---: | :---: | :--- |
| 541 | error | `arrayIndexOutOfBounds` | ✅ 정탐 경고 (Genuine) | Array 'a[10]' accessed at index 10, which is out of bounds. |
| 542 | error | `arrayIndexOutOfBounds` | ✅ 정탐 경고 (Genuine) | Array 'a[10]' accessed at index 10, which is out of bounds. |

---

## 📌 Rule 37: Null Pointer를 산술연산 하지 않는다.

* **세부 조항**: 마
* **매핑 체커 (Clang-Tidy)**: `NullPointerArithmeticCheck`
* **최종 진단 판정**: **✅ 정탐 (True Positive) - 의도된 결함 정확히 검출**

### 💻 검증 소스코드
> 파일 위치: `dapa_test_suite.cpp` (L552 ~ L562)

```cpp
 552 | namespace Rule_37 {
 553 |     #define NULL (void *)0
 554 | 
 555 |     void foo(int *q)
 556 |     {
 557 |         if (q == NULL)
 558 |         {
 559 |             ++q;  // ❌ NULL 포인터에 산술 연산 → 정의되지 않은 동작
 560 |         }
 561 |     }
 562 | }
```

### 🔍 Cppcheck 진단 경고 로그 (Raw Output)
| 라인 (Line) | 중요도 (Severity) | 경고 ID (Warning ID) | 분류 | 진단 메시지 (Message) |
| :---: | :---: | :---: | :---: | :--- |
| 559 | warning | `uselessAssignmentPtrArg` | ⚠️ 파생 경고 (Collateral) | Assignment of function parameter has no effect outside the function. Did you forget dereferencing it? |
| 559 | warning | `nullPointerArithmeticRedundantCheck` | ✅ 정탐 경고 (Genuine) | Either the condition 'q==(void*)0' is redundant or there is pointer arithmetic with NULL pointer. |
| 559 | style | `unreadVariable` | ⚠️ 파생 경고 (Collateral) | Variable 'q' is assigned a value that is never used. |

---
