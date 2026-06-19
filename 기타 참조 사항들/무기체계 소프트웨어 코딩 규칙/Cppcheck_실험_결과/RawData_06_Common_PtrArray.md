# Cppcheck 실험 검증 코드 및 로우데이터 - 공통 적용 (포인터 및 배열)

본 문서는 DAPA 무기체계 소프트웨어 코딩 규칙 중 **공통 적용 (포인터 및 배열)**에 속하는 규칙들의 실제 테스트 코드와 Cppcheck 정적 분석 결과(진단 경고 로우데이터)를 매핑한 자료입니다.

---

## 📌 Rule 33: 포인터는 참조전에 NULL 여부를 반드시 확인하여야 한다.

* **세부 조항**: 가
* **매핑 체커 (Clang-Tidy)**: `NullDereferenceGuardCheck`
* **최종 진단 판정**: **⚠️ 미탐 (파생 경고만 발생) - 결함은 미탐지하고 부수적 경고만 발생**

### 💻 검증 소스코드
> 파일 위치: `dapa_test_suite.cpp` (L524 ~ L532)

```cpp
 524 | namespace Rule_33 {
 525 |     // Removed duplicate NULL macro definition to compile on C++
 526 | 
 527 |     void foo(int *p)
 528 |     {
 529 |         int r;
 530 |         r = *p;  // ❌ p가 NULL일 경우 → 시스템 크래시 발생 가능
 531 |     }
 532 | }
```

### 🔍 Cppcheck 진단 경고 로그 (Raw Output)
| 라인 (Line) | 중요도 (Severity) | 경고 ID (Warning ID) | 분류 | 진단 메시지 (Message) |
| :---: | :---: | :---: | :---: | :--- |
| 530 | style | `unreadVariable` | ⚠️ 파생 경고 (Collateral) | Variable 'r' is assigned a value that is never used. |

---

## 📌 Rule 34: 지역변수의 주소값을 더 넓은 scope를 가진 변수에 할당하지 말아야 한다.

* **세부 조항**: 나
* **매핑 체커 (Clang-Tidy)**: `ReturnStackAddressCheck`
* **최종 진단 판정**: **✅ 정탐 (True Positive) - 의도된 결함 정확히 검출**

### 💻 검증 소스코드
> 파일 위치: `dapa_test_suite.cpp` (L538 ~ L546)

```cpp
 538 | namespace Rule_34 {
 539 |     extern int *pi;
 540 | 
 541 |     void foo(void)
 542 |     {
 543 |         int a;
 544 |         pi = &a;  // ❌ a는 foo 함수가 끝나면 사라짐 → pi는 댕글링 포인터가 됨
 545 |     }
 546 | }
```

### 🔍 Cppcheck 진단 경고 로그 (Raw Output)
| 라인 (Line) | 중요도 (Severity) | 경고 ID (Warning ID) | 분류 | 진단 메시지 (Message) |
| :---: | :---: | :---: | :---: | :--- |
| 544 | error | `danglingLifetime` | ✅ 정탐 경고 (Genuine) | Non-local variable 'pi' will use pointer to local variable 'a'. |

---

## 📌 Rule 35: 지역변수의 주소값을 함수의 리턴값으로 사용하지 말아야 한다.

* **세부 조항**: 다
* **매핑 체커 (Clang-Tidy)**: `ReturnStackAddressCheck`
* **최종 진단 판정**: **✅ 정탐 (True Positive) - 의도된 결함 정확히 검출**

### 💻 검증 소스코드
> 파일 위치: `dapa_test_suite.cpp` (L552 ~ L558)

```cpp
 552 | namespace Rule_35 {
 553 |     int *func(void)
 554 |     {
 555 |         int a = 0;
 556 |         return &a;  // ❌ a는 함수 종료와 함께 사라짐 → 댕글링 포인터
 557 |     }
 558 | }
```

### 🔍 Cppcheck 진단 경고 로그 (Raw Output)
| 라인 (Line) | 중요도 (Severity) | 경고 ID (Warning ID) | 분류 | 진단 메시지 (Message) |
| :---: | :---: | :---: | :---: | :--- |
| 556 | error | `returnDanglingLifetime` | ✅ 정탐 경고 (Genuine) | Returning pointer to local variable 'a' that will be invalid when returning. |

---

## 📌 Rule 36: 선언된 배열의 크기를 초과하는 인덱스 값을 사용하지 말아야 한다.

* **세부 조항**: 라
* **매핑 체커 (Clang-Tidy)**: `ArrayIndexBoundCheck`
* **최종 진단 판정**: **✅ 정탐 (True Positive) - 의도된 결함 정확히 검출**

### 💻 검증 소스코드
> 파일 위치: `dapa_test_suite.cpp` (L564 ~ L575)

```cpp
 564 | namespace Rule_36 {
 565 |     extern int a[10];
 566 | 
 567 |     extern int foo(void)
 568 |     {
 569 |         int r;
 570 |         r = a[10];       // ❌ 유효한 인덱스는 0~9 → 범위 초과
 571 |         a[10] = 0;       // ❌ 쓰기 접근도 위험
 572 |         *(a + 10) = 0;   // ❌ 포인터 연산도 동일하게 위험
 573 |         return r;
 574 |     }
 575 | }
```

### 🔍 Cppcheck 진단 경고 로그 (Raw Output)
| 라인 (Line) | 중요도 (Severity) | 경고 ID (Warning ID) | 분류 | 진단 메시지 (Message) |
| :---: | :---: | :---: | :---: | :--- |
| 570 | error | `arrayIndexOutOfBounds` | ✅ 정탐 경고 (Genuine) | Array 'a[10]' accessed at index 10, which is out of bounds. |
| 571 | error | `arrayIndexOutOfBounds` | ✅ 정탐 경고 (Genuine) | Array 'a[10]' accessed at index 10, which is out of bounds. |

---

## 📌 Rule 37: Null Pointer를 산술연산 하지 않는다.

* **세부 조항**: 마
* **매핑 체커 (Clang-Tidy)**: `NullPointerArithmeticCheck`
* **최종 진단 판정**: **✅ 정탐 (True Positive) - 의도된 결함 정확히 검출**

### 💻 검증 소스코드
> 파일 위치: `dapa_test_suite.cpp` (L581 ~ L591)

```cpp
 581 | namespace Rule_37 {
 582 |     // Removed duplicate NULL macro definition to compile on C++
 583 | 
 584 |     void foo(int *q)
 585 |     {
 586 |         if (q == NULL)
 587 |         {
 588 |             ++q;  // ❌ NULL 포인터에 산술 연산 → 정의되지 않은 동작
 589 |         }
 590 |     }
 591 | }
```

### 🔍 Cppcheck 진단 경고 로그 (Raw Output)
| 라인 (Line) | 중요도 (Severity) | 경고 ID (Warning ID) | 분류 | 진단 메시지 (Message) |
| :---: | :---: | :---: | :---: | :--- |
| 588 | warning | `uselessAssignmentPtrArg` | ⚠️ 파생 경고 (Collateral) | Assignment of function parameter has no effect outside the function. Did you forget dereferencing it? |
| 588 | warning | `nullPointerArithmeticRedundantCheck` | ✅ 정탐 경고 (Genuine) | Either the condition 'q==NULL' is redundant or there is pointer arithmetic with NULL pointer. |
| 588 | style | `unreadVariable` | ⚠️ 파생 경고 (Collateral) | Variable 'q' is assigned a value that is never used. |

---
