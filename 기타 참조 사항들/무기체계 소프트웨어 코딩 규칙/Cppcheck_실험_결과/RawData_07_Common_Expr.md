# Cppcheck 실험 검증 코드 및 로우데이터 - 공통 적용 (연산식)

본 문서는 DAPA 무기체계 소프트웨어 코딩 규칙 중 **공통 적용 (연산식)**에 속하는 규칙들의 실제 테스트 코드와 Cppcheck 정적 분석 결과(진단 경고 로우데이터)를 매핑한 자료입니다.

---

## 📌 Rule 38: 하나의 Sequence Point 내에서 하나의 Object Value를 두 번 이상 변경하지 않아야 한다.

* **세부 조항**: 가
* **매핑 체커 (Clang-Tidy)**: `SequencingRulesCheck`
* **최종 진단 판정**: **✅ 정탐 (True Positive) - 의도된 결함 정확히 검출**

### 💻 검증 소스코드
> 파일 위치: `dapa_test_suite.cpp` (L568 ~ L573)

```cpp
 568 | namespace Rule_38 {
 569 |     extern void foo(void) {
 570 |         int x = 7;
 571 |         x = x++ * x++;  // ❌ 위반: x의 값을 두 번 이상 변경 + 순서 미정
 572 |     }
 573 | }
```

### 🔍 Cppcheck 진단 경고 로그 (Raw Output)
| 라인 (Line) | 중요도 (Severity) | 경고 ID (Warning ID) | 분류 | 진단 메시지 (Message) |
| :---: | :---: | :---: | :---: | :--- |
| 571 | error | `unknownEvaluationOrder` | ✅ 정탐 경고 (Genuine) | Expression 'x++*x++' depends on order of evaluation of side effects |
| 571 | style | `unreadVariable` | ⚠️ 파생 경고 (Collateral) | Variable 'x' is assigned a value that is never used. |

---

## 📌 Rule 39: 나누는 값이 변수일 경우 0인지를 반드시 확인하여야 한다.

* **세부 조항**: 나
* **매핑 체커 (Clang-Tidy)**: `NonZeroDivisorGuardCheck`
* **최종 진단 판정**: **⚠️ 미탐 (파생 경고만 발생) - 결함은 미탐지하고 부수적 경고만 발생**

### 💻 검증 소스코드
> 파일 위치: `dapa_test_suite.cpp` (L579 ~ L584)

```cpp
 579 | namespace Rule_39 {
 580 |     void foo(int x, int n) {
 581 |         int r;
 582 |         r = x / n;  // ❌ n이 0일 경우 → 런타임 오류 (division by zero)
 583 |     }
 584 | }
```

### 🔍 Cppcheck 진단 경고 로그 (Raw Output)
| 라인 (Line) | 중요도 (Severity) | 경고 ID (Warning ID) | 분류 | 진단 메시지 (Message) |
| :---: | :---: | :---: | :---: | :--- |
| 582 | style | `unreadVariable` | ⚠️ 파생 경고 (Collateral) | Variable 'r' is assigned a value that is never used. |

---

## 📌 Rule 40: 하나의 Sequence Point내에서 Object의 값을 변경하고 Access 하지 않아야 한다.

* **세부 조항**: 다
* **매핑 체커 (Clang-Tidy)**: `SequencingRulesCheck`
* **최종 진단 판정**: **✅ 정탐 (True Positive) - 의도된 결함 정확히 검출**

### 💻 검증 소스코드
> 파일 위치: `dapa_test_suite.cpp` (L590 ~ L596)

```cpp
 590 | namespace Rule_40 {
 591 |     extern void foo(int index)
 592 |     {
 593 |         int r;
 594 |         r = (2 * index) + index++;  // ❌ 위반: index를 읽고 동시에 변경
 595 |     }
 596 | }
```

### 🔍 Cppcheck 진단 경고 로그 (Raw Output)
| 라인 (Line) | 중요도 (Severity) | 경고 ID (Warning ID) | 분류 | 진단 메시지 (Message) |
| :---: | :---: | :---: | :---: | :--- |
| 594 | error | `unknownEvaluationOrder` | ✅ 정탐 경고 (Genuine) | Expression '(2*index)+index++' depends on order of evaluation of side effects |
| 594 | style | `unreadVariable` | ⚠️ 파생 경고 (Collateral) | Variable 'r' is assigned a value that is never used. |

---

## 📌 Rule 41: 음수 값 또는 데이터 사이즈를 초과하는 값을 사용하여 Shift operator를 하지 않는다.

* **세부 조항**: 라
* **매핑 체커 (Clang-Tidy)**: `ShiftAmountRangeCheck`
* **최종 진단 판정**: **✅ 정탐 (True Positive) - 의도된 결함 정확히 검출**

### 💻 검증 소스코드
> 파일 위치: `dapa_test_suite.cpp` (L602 ~ L610)

```cpp
 602 | namespace Rule_41 {
 603 |     extern unsigned int uia;
 604 | 
 605 |     extern void foo(void)
 606 |     {
 607 |         uia = uia << -2;  // ❌ 음수 shift → 정의되지 않은 동작
 608 |         uia = uia << 40;  // ❌ 데이터 타입 크기 초과 → 정의되지 않은 동작
 609 |     }
 610 | }
```

### 🔍 Cppcheck 진단 경고 로그 (Raw Output)
| 라인 (Line) | 중요도 (Severity) | 경고 ID (Warning ID) | 분류 | 진단 메시지 (Message) |
| :---: | :---: | :---: | :---: | :--- |
| 607 | error | `shiftTooManyBits` | ✅ 정탐 경고 (Genuine) | Shifting 32-bit value by 4294967294 bits is undefined behaviour |
| 608 | error | `shiftTooManyBits` | ✅ 정탐 경고 (Genuine) | Shifting 32-bit value by 40 bits is undefined behaviour |
| 607 | warning | `signConversion` | ✅ 정탐 경고 (Genuine) | Expression '-2' has a negative value. That is converted to an unsigned value and used in an unsigned calculation. |

---

## 📌 Rule 42: Underlying type이 Unsigned인 데이터에 '-'를 사용하여 결과를 assign하지 말아야 한다.

* **세부 조항**: 마
* **매핑 체커 (Clang-Tidy)**: `UnsignedMinusAssignmentCheck`
* **최종 진단 판정**: **⚠️ 미탐 (파생 경고만 발생) - 결함은 미탐지하고 부수적 경고만 발생**

### 💻 검증 소스코드
> 파일 위치: `dapa_test_suite.cpp` (L616 ~ L622)

```cpp
 616 | namespace Rule_42 {
 617 |     void foo(unsigned char i)
 618 |     {
 619 |         long j;
 620 |         j = -i;  // ❌ unsigned 타입에 음수 연산 → 의도와 다른 결과
 621 |     }
 622 | }
```

### 🔍 Cppcheck 진단 경고 로그 (Raw Output)
| 라인 (Line) | 중요도 (Severity) | 경고 ID (Warning ID) | 분류 | 진단 메시지 (Message) |
| :---: | :---: | :---: | :---: | :--- |
| 620 | style | `unreadVariable` | ⚠️ 파생 경고 (Collateral) | Variable 'j' is assigned a value that is never used. |

---

## 📌 Rule 43: ’sizeof’의 인자로 side effect를 포함하는 식을 사용하면 안된다.

* **세부 조항**: 바
* **매핑 체커 (Clang-Tidy)**: `UnevaluatedSideEffectCheck`
* **최종 진단 판정**: **✅ 정탐 (True Positive) - 의도된 결함 정확히 검출**

### 💻 검증 소스코드
> 파일 위치: `dapa_test_suite.cpp` (L628 ~ L637)

```cpp
 628 | namespace Rule_43 {
 629 |     long foo(int x)
 630 |     {
 631 |         long y;
 632 |         unsigned long un;
 633 |         y = x;
 634 |         un = sizeof(++y);  // ❌ 위반: ++y는 side effect → 실행되지 않음
 635 |         return y;
 636 |     }
 637 | }
```

### 🔍 Cppcheck 진단 경고 로그 (Raw Output)
| 라인 (Line) | 중요도 (Severity) | 경고 ID (Warning ID) | 분류 | 진단 메시지 (Message) |
| :---: | :---: | :---: | :---: | :--- |
| 634 | warning | `sizeofCalculation` | ✅ 정탐 경고 (Genuine) | Found calculation inside sizeof(). |
| 634 | style | `unreadVariable` | ⚠️ 파생 경고 (Collateral) | Variable 'un' is assigned a value that is never used. |

---

## 📌 Rule 44: Boolean 표현 값에는 논리 연산자(&&, ||, !)만을 사용하여야 한다.

* **세부 조항**: 사
* **매핑 체커 (Clang-Tidy)**: `BoolLogicOnlyOpsCheck`
* **최종 진단 판정**: **⚠️ 미탐 (파생 경고만 발생) - 결함은 미탐지하고 부수적 경고만 발생**

### 💻 검증 소스코드
> 파일 위치: `dapa_test_suite.cpp` (L643 ~ L652)

```cpp
 643 | namespace Rule_44 {
 644 |     int foo(int a, int b, int c, int d)
 645 |     {
 646 |         int x;
 647 |         x = ~((a > b) && (b > 10));     // ❌ 비트 반전 연산자(~) 사용
 648 |         x = (a > b) + (c > d);          // ❌ 논리값을 산술 덧셈에 사용
 649 |         x = (a + b) > (c > d);          // ❌ 논리값을 산술 비교에 사용
 650 |         return x;
 651 |     }
 652 | }
```

### 🔍 Cppcheck 진단 경고 로그 (Raw Output)
| 라인 (Line) | 중요도 (Severity) | 경고 ID (Warning ID) | 분류 | 진단 메시지 (Message) |
| :---: | :---: | :---: | :---: | :--- |
| 648 | style | `redundantAssignment` | ⚠️ 파생 경고 (Collateral) | Variable 'x' is reassigned a value before the old one has been used. |
| 649 | style | `redundantAssignment` | ⚠️ 파생 경고 (Collateral) | Variable 'x' is reassigned a value before the old one has been used. |

---

## 📌 Rule 45: condition에 assignment operator를 사용하지 말아야 한다.

* **세부 조항**: 아
* **매핑 체커 (Clang-Tidy)**: `NoAssignmentInConditionCheck`
* **최종 진단 판정**: **❌ 미탐 (Silent / False Negative) - 아무런 경고도 감지되지 않음**

### 💻 검증 소스코드
> 파일 위치: `dapa_test_suite.cpp` (L658 ~ L662)

```cpp
 658 | namespace Rule_45 {
 659 |     if (var2 = var1) {
 660 |         // ...
 661 |     }
 662 | }
```

### 🔍 Cppcheck 진단 경고 로그 (Raw Output)
> ❌ **검출된 경고 없음 (Silent)**

---

## 📌 Rule 46: Signed Value에서 Bitwise연산자(<<, ~, |, ^ 등)로 인한 Negative Value를 유의해야 한다.

* **세부 조항**: 자
* **매핑 체커 (Clang-Tidy)**: `SignedBitwiseSafetyCheck`
* **최종 진단 판정**: **⚠️ 미탐 (파생 경고만 발생) - 결함은 미탐지하고 부수적 경고만 발생**

### 💻 검증 소스코드
> 파일 위치: `dapa_test_suite.cpp` (L668 ~ L674)

```cpp
 668 | namespace Rule_46 {
 669 |     void foo(long sl)
 670 |     {
 671 |         long slr;
 672 |         slr = sl << 4;  // ❌ sl이 음수일 경우 → 부호 비트가 밀려나며 예기치 않은 결과 발생
 673 |     }
 674 | }
```

### 🔍 Cppcheck 진단 경고 로그 (Raw Output)
| 라인 (Line) | 중요도 (Severity) | 경고 ID (Warning ID) | 분류 | 진단 메시지 (Message) |
| :---: | :---: | :---: | :---: | :--- |
| 672 | style | `unreadVariable` | ⚠️ 파생 경고 (Collateral) | Variable 'slr' is assigned a value that is never used. |

---
