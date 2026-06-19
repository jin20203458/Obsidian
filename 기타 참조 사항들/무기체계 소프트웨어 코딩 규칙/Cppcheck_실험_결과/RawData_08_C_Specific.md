# Cppcheck 실험 검증 코드 및 로우데이터 - C 전용 코딩 규칙

본 문서는 DAPA 무기체계 소프트웨어 코딩 규칙 중 **C 전용 코딩 규칙**에 속하는 규칙들의 실제 테스트 코드와 Cppcheck 정적 분석 결과(진단 경고 로우데이터)를 매핑한 자료입니다.

---

## 📌 Rule 47: Scanf의 Argument 에는 Object Value가 저장된 주소값이 입력되어야 한다.

* **세부 조항**: 1
* **매핑 체커 (Clang-Tidy)**: `VarargsFormatCheck`
* **최종 진단 판정**: **✅ 정탐 (True Positive) - 의도된 결함 정확히 검출**

### 💻 검증 소스코드
> 파일 위치: `dapa_test_suite.cpp` (L714 ~ L722)

```cpp
 714 | namespace Rule_47 {
 715 |     #include <stdio.h>
 716 | 
 717 |     extern void foo(void)
 718 |     {
 719 |         int n;
 720 |         scanf("%d", n);  // ❌ 위반: n은 int → 주소가 아님
 721 |     }
 722 | }
```

### 🔍 Cppcheck 진단 경고 로그 (Raw Output)
| 라인 (Line) | 중요도 (Severity) | 경고 ID (Warning ID) | 분류 | 진단 메시지 (Message) |
| :---: | :---: | :---: | :---: | :--- |
| 720 | warning | `invalidScanfArgType_int` | ✅ 정탐 경고 (Genuine) | %d in format string (no. 1) requires 'int *' but the argument type is 'signed int'. |

---

## 📌 Rule 48: `#include` 구문에서 표준에 맞지 않는 Character set을 사용하지 않아야 한다.

* **세부 조항**: 2
* **매핑 체커 (Clang-Tidy)**: `IncludeCharsetCheck`
* **최종 진단 판정**: **❌ 미탐 (Silent / False Negative) - 아무런 경고도 감지되지 않음**

### 💻 검증 소스코드
> 파일 위치: `dapa_test_suite.cpp` (L728 ~ L735)

```cpp
 728 | namespace Rule_48 {
 729 | #if 0
 730 |     #include "taemin's.h"               // ❌ 작은따옴표 (') 포함
 731 |     #include "donghyun.h/*temporary*/"  // ❌ 주석 포함
 732 |     #include "Jady$.h"                  // ❌ 특수문자 $ 포함
 733 |     #include "June's-header.h"         // ❌ 작은따옴표 및 하이픈 포함
 734 | #endif
 735 | }
```

### 🔍 Cppcheck 진단 경고 로그 (Raw Output)
> ❌ **검출된 경고 없음 (Silent)**

---

## 📌 Rule 49: malloc에서 사용하는 인자값은 assign될 변수 type의 정수배이어야 한다.

* **세부 조항**: 3
* **매핑 체커 (Clang-Tidy)**: `MallocSizeMultipleCheck`
* **최종 진단 판정**: **⚠️ 미탐 (파생 경고만 발생) - 결함은 미탐지하고 부수적 경고만 발생**

### 💻 검증 소스코드
> 파일 위치: `dapa_test_suite.cpp` (L741 ~ L749)

```cpp
 741 | namespace Rule_49 {
 742 |     void foo(void)
 743 |     {
 744 |         long *ptd;
 745 |         ptd = (long *) malloc(3);   // ❌ long 크기의 정수배 아님
 746 |         ptd = (long *) malloc(11);  // ❌ long 크기의 정수배 아님
 747 |         ptd = (long *) malloc(13);  // ❌ long 크기의 정수배 아님
 748 |     }
 749 | }
```

### 🔍 Cppcheck 진단 경고 로그 (Raw Output)
| 라인 (Line) | 중요도 (Severity) | 경고 ID (Warning ID) | 분류 | 진단 메시지 (Message) |
| :---: | :---: | :---: | :---: | :--- |
| 746 | error | `memleak` | ⚠️ 파생 경고 (Collateral) | Memory leak: ptd |
| 747 | error | `memleak` | ⚠️ 파생 경고 (Collateral) | Memory leak: ptd |
| 748 | error | `memleak` | ⚠️ 파생 경고 (Collateral) | Memory leak: ptd |
| 745 | style | `cstyleCast` | ⚠️ 파생 경고 (Collateral) | C-style pointer casting |
| 746 | style | `cstyleCast` | ⚠️ 파생 경고 (Collateral) | C-style pointer casting |
| 747 | style | `cstyleCast` | ⚠️ 파생 경고 (Collateral) | C-style pointer casting |
| 746 | style | `redundantAssignment` | ⚠️ 파생 경고 (Collateral) | Variable 'ptd' is reassigned a value before the old one has been used. |
| 747 | style | `redundantAssignment` | ⚠️ 파생 경고 (Collateral) | Variable 'ptd' is reassigned a value before the old one has been used. |
| 744 | style | `constVariablePointer` | ⚠️ 파생 경고 (Collateral) | Variable 'ptd' can be declared as pointer to const |
| 747 | style | `unreadVariable` | ⚠️ 파생 경고 (Collateral) | Variable 'ptd' is assigned a value that is never used. |
| 747 | style | `unusedAllocatedMemory` | ⚠️ 파생 경고 (Collateral) | Variable 'ptd' is allocated memory that is never used. |

---

## 📌 Rule 50: 함수의 Argument type과 개수는 함수의 Prototype, 선언, 정의가 모두 같아야 한다.

* **세부 조항**: 4
* **매핑 체커 (Clang-Tidy)**: `FunctionSignatureConsistencyCheck`
* **최종 진단 판정**: **⚠️ 미탐 (파생 경고만 발생) - 결함은 미탐지하고 부수적 경고만 발생**

### 💻 검증 소스코드
> 파일 위치: `dapa_test_suite.cpp` (L755 ~ L766)

```cpp
 755 | namespace Rule_50 {
 756 |     extern int foo(unsigned char a);  // 선언: unsigned char 1개
 757 | 
 758 |     void test(float a, unsigned char b)
 759 |     {
 760 |         int r;
 761 |         r = foo(a);       // ❌ 타입 불일치: float → unsigned char
 762 | #if 0
 763 |         r = foo(b, a);    // ❌ 개수 불일치: 인자 2개 전달
 764 | #endif
 765 |     }
 766 | }
```

### 🔍 Cppcheck 진단 경고 로그 (Raw Output)
| 라인 (Line) | 중요도 (Severity) | 경고 ID (Warning ID) | 분류 | 진단 메시지 (Message) |
| :---: | :---: | :---: | :---: | :--- |
| 761 | style | `unreadVariable` | ⚠️ 파생 경고 (Collateral) | Variable 'r' is assigned a value that is never used. |

---

## 📌 Rule 51: 구조체/배열의 초기화 시 default 초기화 값(0)을 제외하고, 구조에 맞게 ‘{}’를 사용하여 선언된 Size에 맞게 초기화 해야 한다.

* **세부 조항**: 5
* **매핑 체커 (Clang-Tidy)**: `AggregateInitStyleCheck`
* **최종 진단 판정**: **⚠️ 미탐 (파생 경고만 발생) - 결함은 미탐지하고 부수적 경고만 발생**

### 💻 검증 소스코드
> 파일 위치: `dapa_test_suite.cpp` (L772 ~ L781)

```cpp
 772 | namespace Rule_51 {
 773 |     struct s {
 774 |         int i;
 775 |         int j;
 776 |     };
 777 | 
 778 |     const struct s s1 = {8};               // ❌ j는 명시적으로 초기화되지 않음
 779 |     int arr[10] = {10};                    // ❌ arr[0]만 10, 나머지는 0 → 의도 불명확
 780 |     int arr1[3][2] = {1, 2, 3, 4, 5, 6};   // ❌ 중첩 배열인데 flat하게 초기화 → 가독성 저하
 781 | }
```

### 🔍 Cppcheck 진단 경고 로그 (Raw Output)
| 라인 (Line) | 중요도 (Severity) | 경고 ID (Warning ID) | 분류 | 진단 메시지 (Message) |
| :---: | :---: | :---: | :---: | :--- |
| 774 | style | `unusedStructMember` | ⚠️ 파생 경고 (Collateral) | struct member 's::i' is never used. |
| 775 | style | `unusedStructMember` | ⚠️ 파생 경고 (Collateral) | struct member 's::j' is never used. |

---
