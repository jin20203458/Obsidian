# Cppcheck 실험 검증 코드 및 로우데이터 - 공통 적용 (연산식)

본 문서는 DAPA 무기체계 소프트웨어 코딩 규칙 중 **공통 적용 (연산식)**에 속하는 규칙들의 실제 테스트 코드와 Cppcheck 정적 분석 결과(진단 경고 로우데이터)를 매핑한 자료입니다.

---

## 📌 Rule 38: 하나의 Sequence Point 내에서 하나의 Object Value를 두 번 이상 변경하지 않아야 한다.

* **세부 조항**: 가
* **매핑 체커 (Clang-Tidy)**: `SequencingRulesCheck`
* **최종 진단 판정**: **✅ 정탐 (True Positive) - 의도된 결함 정확히 검출**

### 💻 검증 소스코드
> 파일 위치: `dapa_test_suite.cpp` (L597 ~ L602)

```cpp
 597 | namespace Rule_38 {
 598 |     extern void foo(void) {
 599 |         int x = 7;
 600 |         x = x++ * x++;  // ❌ 위반: x의 값을 두 번 이상 변경 + 순서 미정
 601 |     }
 602 | }
```

### 🔍 Cppcheck 진단 경고 로그 (Raw Output)
| 라인 (Line) | 중요도 (Severity) | 경고 ID (Warning ID) | 분류 | 진단 메시지 (Message) |
| :---: | :---: | :---: | :---: | :--- |
| 600 | error | `unknownEvaluationOrder` | ✅ 정탐 경고 (Genuine) | Expression 'x++*x++' depends on order of evaluation of side effects |
| 600 | style | `unreadVariable` | ⚠️ 파생 경고 (Collateral) | Variable 'x' is assigned a value that is never used. |

---

## 📌 Rule 39: 나누는 값이 변수일 경우 0인지를 반드시 확인하여야 한다.

* **세부 조항**: 나
* **매핑 체커 (Clang-Tidy)**: `NonZeroDivisorGuardCheck`
* **최종 진단 판정**: **⚠️ 미탐 (파생 경고만 발생) - 결함은 미탐지하고 부수적 경고만 발생**

### 💻 검증 소스코드
> 파일 위치: `dapa_test_suite.cpp` (L608 ~ L613)

```cpp
 608 | namespace Rule_39 {
 609 |     void foo(int x, int n) {
 610 |         int r;
 611 |         r = x / n;  // ❌ n이 0일 경우 → 런타임 오류 (division by zero)
 612 |     }
 613 | }
```

### 🔍 Cppcheck 진단 경고 로그 (Raw Output)
| 라인 (Line) | 중요도 (Severity) | 경고 ID (Warning ID) | 분류 | 진단 메시지 (Message) |
| :---: | :---: | :---: | :---: | :--- |
| 611 | style | `unreadVariable` | ⚠️ 파생 경고 (Collateral) | Variable 'r' is assigned a value that is never used. |

---

## 📌 Rule 40: 하나의 Sequence Point내에서 Object의 값을 변경하고 Access 하지 않아야 한다.

* **세부 조항**: 다
* **매핑 체커 (Clang-Tidy)**: `SequencingRulesCheck`
* **최종 진단 판정**: **✅ 정탐 (True Positive) - 의도된 결함 정확히 검출**

### 💻 검증 소스코드
> 파일 위치: `dapa_test_suite.cpp` (L619 ~ L625)

```cpp
 619 | namespace Rule_40 {
 620 |     extern void foo(int index)
 621 |     {
 622 |         int r;
 623 |         r = (2 * index) + index++;  // ❌ 위반: index를 읽고 동시에 변경
 624 |     }
 625 | }
```

### 🔍 Cppcheck 진단 경고 로그 (Raw Output)
| 라인 (Line) | 중요도 (Severity) | 경고 ID (Warning ID) | 분류 | 진단 메시지 (Message) |
| :---: | :---: | :---: | :---: | :--- |
| 623 | error | `unknownEvaluationOrder` | ✅ 정탐 경고 (Genuine) | Expression '(2*index)+index++' depends on order of evaluation of side effects |
| 623 | style | `unreadVariable` | ⚠️ 파생 경고 (Collateral) | Variable 'r' is assigned a value that is never used. |

---

## 📌 Rule 41: 음수 값 또는 데이터 사이즈를 초과하는 값을 사용하여 Shift operator를 하지 않는다.

* **세부 조항**: 라
* **매핑 체커 (Clang-Tidy)**: `ShiftAmountRangeCheck`
* **최종 진단 판정**: **✅ 정탐 (True Positive) - 의도된 결함 정확히 검출**

### 💻 검증 소스코드
> 파일 위치: `dapa_test_suite.cpp` (L631 ~ L639)

```cpp
 631 | namespace Rule_41 {
 632 |     extern unsigned int uia;
 633 | 
 634 |     extern void foo(void)
 635 |     {
 636 |         uia = uia << -2;  // ❌ 음수 shift → 정의되지 않은 동작
 637 |         uia = uia << 40;  // ❌ 데이터 타입 크기 초과 → 정의되지 않은 동작
 638 |     }
 639 | }
```

### 🔍 Cppcheck 진단 경고 로그 (Raw Output)
| 라인 (Line) | 중요도 (Severity) | 경고 ID (Warning ID) | 분류 | 진단 메시지 (Message) |
| :---: | :---: | :---: | :---: | :--- |
| 636 | error | `shiftTooManyBits` | ✅ 정탐 경고 (Genuine) | Shifting 32-bit value by 4294967294 bits is undefined behaviour |
| 637 | error | `shiftTooManyBits` | ✅ 정탐 경고 (Genuine) | Shifting 32-bit value by 40 bits is undefined behaviour |
| 636 | warning | `signConversion` | ✅ 정탐 경고 (Genuine) | Expression '-2' has a negative value. That is converted to an unsigned value and used in an unsigned calculation. |

---

## 📌 Rule 42: Underlying type이 Unsigned인 데이터에 '-'를 사용하여 결과를 assign하지 말아야 한다.

* **세부 조항**: 마
* **매핑 체커 (Clang-Tidy)**: `UnsignedMinusAssignmentCheck`
* **최종 진단 판정**: **⚠️ 미탐 (파생 경고만 발생) - 결함은 미탐지하고 부수적 경고만 발생**

### 💻 검증 소스코드
> 파일 위치: `dapa_test_suite.cpp` (L645 ~ L651)

```cpp
 645 | namespace Rule_42 {
 646 |     void foo(unsigned char i)
 647 |     {
 648 |         long j;
 649 |         j = -i;  // ❌ unsigned 타입에 음수 연산 → 의도와 다른 결과
 650 |     }
 651 | }
```

### 🔍 Cppcheck 진단 경고 로그 (Raw Output)
| 라인 (Line) | 중요도 (Severity) | 경고 ID (Warning ID) | 분류 | 진단 메시지 (Message) |
| :---: | :---: | :---: | :---: | :--- |
| 649 | style | `unreadVariable` | ⚠️ 파생 경고 (Collateral) | Variable 'j' is assigned a value that is never used. |

---

## 📌 Rule 43: ’sizeof’의 인자로 side effect를 포함하는 식을 사용하면 안된다.

* **세부 조항**: 바
* **매핑 체커 (Clang-Tidy)**: `UnevaluatedSideEffectCheck`
* **최종 진단 판정**: **✅ 정탐 (True Positive) - 의도된 결함 정확히 검출**

### 💻 검증 소스코드
> 파일 위치: `dapa_test_suite.cpp` (L657 ~ L666)

```cpp
 657 | namespace Rule_43 {
 658 |     long foo(int x)
 659 |     {
 660 |         long y;
 661 |         unsigned long un;
 662 |         y = x;
 663 |         un = sizeof(++y);  // ❌ 위반: ++y는 side effect → 실행되지 않음
 664 |         return y;
 665 |     }
 666 | }
```

### 🔍 Cppcheck 진단 경고 로그 (Raw Output)
| 라인 (Line) | 중요도 (Severity) | 경고 ID (Warning ID) | 분류 | 진단 메시지 (Message) |
| :---: | :---: | :---: | :---: | :--- |
| 663 | warning | `sizeofCalculation` | ✅ 정탐 경고 (Genuine) | Found calculation inside sizeof(). |
| 663 | style | `unreadVariable` | ⚠️ 파생 경고 (Collateral) | Variable 'un' is assigned a value that is never used. |

---

## 📌 Rule 44: Boolean 표현 값에는 논리 연산자(&&, ||, !)만을 사용하여야 한다.

* **세부 조항**: 사
* **매핑 체커 (Clang-Tidy)**: `BoolLogicOnlyOpsCheck`
* **최종 진단 판정**: **⚠️ 미탐 (파생 경고만 발생) - 결함은 미탐지하고 부수적 경고만 발생**

### 💻 검증 소스코드
> 파일 위치: `dapa_test_suite.cpp` (L672 ~ L681)

```cpp
 672 | namespace Rule_44 {
 673 |     int foo(int a, int b, int c, int d)
 674 |     {
 675 |         int x;
 676 |         x = ~((a > b) && (b > 10));     // ❌ 비트 반전 연산자(~) 사용
 677 |         x = (a > b) + (c > d);          // ❌ 논리값을 산술 덧셈에 사용
 678 |         x = (a + b) > (c > d);          // ❌ 논리값을 산술 비교에 사용
 679 |         return x;
 680 |     }
 681 | }
```

### 🔍 Cppcheck 진단 경고 로그 (Raw Output)
| 라인 (Line) | 중요도 (Severity) | 경고 ID (Warning ID) | 분류 | 진단 메시지 (Message) |
| :---: | :---: | :---: | :---: | :--- |
| 677 | style | `redundantAssignment` | ⚠️ 파생 경고 (Collateral) | Variable 'x' is reassigned a value before the old one has been used. |
| 678 | style | `redundantAssignment` | ⚠️ 파생 경고 (Collateral) | Variable 'x' is reassigned a value before the old one has been used. |

---

## 📌 Rule 45: condition에 assignment operator를 사용하지 말아야 한다.

* **세부 조항**: 아
* **매핑 체커 (Clang-Tidy)**: `NoAssignmentInConditionCheck`
* **최종 진단 판정**: **✅ 정탐 (True Positive) - 의도된 결함 정확히 검출**

### 💻 검증 소스코드
> 파일 위치: `dapa_test_suite.cpp` (L687 ~ L696)

```cpp
 687 | namespace Rule_45 {
 688 |     void foo()
 689 |     {
 690 |         int var1 = 0;
 691 |         int var2 = 0;
 692 |         if (var2 = var1) {
 693 |             // ...
 694 |         }
 695 |     }
 696 | }
```

### 🔍 Cppcheck 진단 경고 로그 (Raw Output)
| 라인 (Line) | 중요도 (Severity) | 경고 ID (Warning ID) | 분류 | 진단 메시지 (Message) |
| :---: | :---: | :---: | :---: | :--- |
| 692 | style | `knownConditionTrueFalse` | ✅ 정탐 경고 (Genuine) | Condition 'var2=var1' is always false |
| 692 | style | `unreadVariable` | ⚠️ 파생 경고 (Collateral) | Variable 'var2' is assigned a value that is never used. |

---

## 📌 Rule 46: Signed Value에서 Bitwise연산자(<<, ~, |, ^ 등)로 인한 Negative Value를 유의해야 한다.

* **세부 조항**: 자
* **매핑 체커 (Clang-Tidy)**: `SignedBitwiseSafetyCheck`
* **최종 진단 판정**: **⚠️ 미탐 (파생 경고만 발생) - 결함은 미탐지하고 부수적 경고만 발생**

### 💻 검증 소스코드
> 파일 위치: `dapa_test_suite.cpp` (L702 ~ L708)

```cpp
 702 | namespace Rule_46 {
 703 |     void foo(long sl)
 704 |     {
 705 |         long slr;
 706 |         slr = sl << 4;  // ❌ sl이 음수일 경우 → 부호 비트가 밀려나며 예기치 않은 결과 발생
 707 |     }
 708 | }
```

### 🔍 Cppcheck 진단 경고 로그 (Raw Output)
| 라인 (Line) | 중요도 (Severity) | 경고 ID (Warning ID) | 분류 | 진단 메시지 (Message) |
| :---: | :---: | :---: | :---: | :--- |
| 706 | style | `unreadVariable` | ⚠️ 파생 경고 (Collateral) | Variable 'slr' is assigned a value that is never used. |

---
