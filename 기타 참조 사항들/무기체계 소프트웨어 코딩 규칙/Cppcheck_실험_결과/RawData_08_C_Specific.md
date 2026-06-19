# Cppcheck 실험 검증 코드 및 로우데이터 - C 전용 코딩 규칙

본 문서는 DAPA 무기체계 소프트웨어 코딩 규칙 중 **C 전용 코딩 규칙**에 속하는 규칙들의 실제 테스트 코드와 Cppcheck 정적 분석 결과(진단 경고 로우데이터)를 매핑한 자료입니다.

---

## 📌 Rule 47: Scanf의 Argument 에는 Object Value가 저장된 주소값이 입력되어야 한다.

* **세부 조항**: 1
* **매핑 체커 (Clang-Tidy)**: `VarargsFormatCheck`
* **최종 진단 판정**: **✅ 정탐 (True Positive) - 의도된 결함 정확히 검출**

### 💻 검증 소스코드
> 파일 위치: `dapa_test_suite.cpp` (L680 ~ L688)

```cpp
 680 | namespace Rule_47 {
 681 |     #include <stdio.h>
 682 | 
 683 |     extern void foo(void)
 684 |     {
 685 |         int n;
 686 |         scanf("%d", n);  // ❌ 위반: n은 int → 주소가 아님
 687 |     }
 688 | }
```

### 🔍 Cppcheck 진단 경고 로그 (Raw Output)
| 라인 (Line) | 중요도 (Severity) | 경고 ID (Warning ID) | 분류 | 진단 메시지 (Message) |
| :---: | :---: | :---: | :---: | :--- |
| 686 | warning | `invalidScanfArgType_int` | ✅ 정탐 경고 (Genuine) | %d in format string (no. 1) requires 'int *' but the argument type is 'signed int'. |

---

## 📌 Rule 48: `#include` 구문에서 표준에 맞지 않는 Character set을 사용하지 않아야 한다.

* **세부 조항**: 2
* **매핑 체커 (Clang-Tidy)**: `IncludeCharsetCheck`
* **최종 진단 판정**: **⚠️ 미탐 (파생 경고만 발생) - 결함은 미탐지하고 부수적 경고만 발생**

### 💻 검증 소스코드
> 파일 위치: `dapa_test_suite.cpp` (L694 ~ L699)

```cpp
 694 | namespace Rule_48 {
 695 |     #include "taemin's.h"               // ❌ 작은따옴표 (') 포함
 696 |     #include "donghyun.h/*temporary*/"  // ❌ 주석 포함
 697 |     #include "Jady$.h"                  // ❌ 특수문자 $ 포함
 698 |     #include "June's-header.h"         // ❌ 작은따옴표 및 하이픈 포함
 699 | }
```

### 🔍 Cppcheck 진단 경고 로그 (Raw Output)
| 라인 (Line) | 중요도 (Severity) | 경고 ID (Warning ID) | 분류 | 진단 메시지 (Message) |
| :---: | :---: | :---: | :---: | :--- |
| 695 | information | `missingInclude` | ⚠️ 파생 경고 (Collateral) | Include file: "taemin's.h" not found. |
| 696 | information | `missingInclude` | ⚠️ 파생 경고 (Collateral) | Include file: "donghyun.h/*temporary*/" not found. |
| 697 | information | `missingInclude` | ⚠️ 파생 경고 (Collateral) | Include file: "Jady$.h" not found. |
| 698 | information | `missingInclude` | ⚠️ 파생 경고 (Collateral) | Include file: "June's-header.h" not found. |

---

## 📌 Rule 49: malloc에서 사용하는 인자값은 assign될 변수 type의 정수배이어야 한다.

* **세부 조항**: 3
* **매핑 체커 (Clang-Tidy)**: `MallocSizeMultipleCheck`
* **최종 진단 판정**: **⚠️ 미탐 (파생 경고만 발생) - 결함은 미탐지하고 부수적 경고만 발생**

### 💻 검증 소스코드
> 파일 위치: `dapa_test_suite.cpp` (L705 ~ L713)

```cpp
 705 | namespace Rule_49 {
 706 |     void foo(void)
 707 |     {
 708 |         long *ptd;
 709 |         ptd = (long *) malloc(3);   // ❌ long 크기의 정수배 아님
 710 |         ptd = (long *) malloc(11);  // ❌ long 크기의 정수배 아님
 711 |         ptd = (long *) malloc(13);  // ❌ long 크기의 정수배 아님
 712 |     }
 713 | }
```

### 🔍 Cppcheck 진단 경고 로그 (Raw Output)
| 라인 (Line) | 중요도 (Severity) | 경고 ID (Warning ID) | 분류 | 진단 메시지 (Message) |
| :---: | :---: | :---: | :---: | :--- |
| 710 | error | `memleak` | ⚠️ 파생 경고 (Collateral) | Memory leak: ptd |
| 711 | error | `memleak` | ⚠️ 파생 경고 (Collateral) | Memory leak: ptd |
| 712 | error | `memleak` | ⚠️ 파생 경고 (Collateral) | Memory leak: ptd |
| 709 | style | `cstyleCast` | ⚠️ 파생 경고 (Collateral) | C-style pointer casting |
| 710 | style | `cstyleCast` | ⚠️ 파생 경고 (Collateral) | C-style pointer casting |
| 711 | style | `cstyleCast` | ⚠️ 파생 경고 (Collateral) | C-style pointer casting |
| 710 | style | `redundantAssignment` | ⚠️ 파생 경고 (Collateral) | Variable 'ptd' is reassigned a value before the old one has been used. |
| 711 | style | `redundantAssignment` | ⚠️ 파생 경고 (Collateral) | Variable 'ptd' is reassigned a value before the old one has been used. |
| 708 | style | `constVariablePointer` | ⚠️ 파생 경고 (Collateral) | Variable 'ptd' can be declared as pointer to const |
| 711 | style | `unreadVariable` | ⚠️ 파생 경고 (Collateral) | Variable 'ptd' is assigned a value that is never used. |
| 711 | style | `unusedAllocatedMemory` | ⚠️ 파생 경고 (Collateral) | Variable 'ptd' is allocated memory that is never used. |

---

## 📌 Rule 50: 함수의 Argument type과 개수는 함수의 Prototype, 선언, 정의가 모두 같아야 한다.

* **세부 조항**: 4
* **매핑 체커 (Clang-Tidy)**: `FunctionSignatureConsistencyCheck`
* **최종 진단 판정**: **⚠️ 미탐 (파생 경고만 발생) - 결함은 미탐지하고 부수적 경고만 발생**

### 💻 검증 소스코드
> 파일 위치: `dapa_test_suite.cpp` (L719 ~ L728)

```cpp
 719 | namespace Rule_50 {
 720 |     extern int foo(unsigned char a);  // 선언: unsigned char 1개
 721 | 
 722 |     void test(float a, unsigned char b)
 723 |     {
 724 |         int r;
 725 |         r = foo(a);       // ❌ 타입 불일치: float → unsigned char
 726 |         r = foo(b, a);    // ❌ 개수 불일치: 인자 2개 전달
 727 |     }
 728 | }
```

### 🔍 Cppcheck 진단 경고 로그 (Raw Output)
| 라인 (Line) | 중요도 (Severity) | 경고 ID (Warning ID) | 분류 | 진단 메시지 (Message) |
| :---: | :---: | :---: | :---: | :--- |
| 726 | style | `redundantAssignment` | ⚠️ 파생 경고 (Collateral) | Variable 'r' is reassigned a value before the old one has been used. |
| 726 | style | `unreadVariable` | ⚠️ 파생 경고 (Collateral) | Variable 'r' is assigned a value that is never used. |

---

## 📌 Rule 51: 구조체/배열의 초기화 시 default 초기화 값(0)을 제외하고, 구조에 맞게 ‘{}’를 사용하여 선언된 Size에 맞게 초기화 해야 한다.

* **세부 조항**: 5
* **매핑 체커 (Clang-Tidy)**: `AggregateInitStyleCheck`
* **최종 진단 판정**: **⚠️ 미탐 (파생 경고만 발생) - 결함은 미탐지하고 부수적 경고만 발생**

### 💻 검증 소스코드
> 파일 위치: `dapa_test_suite.cpp` (L734 ~ L743)

```cpp
 734 | namespace Rule_51 {
 735 |     struct s {
 736 |         int i;
 737 |         int j;
 738 |     };
 739 | 
 740 |     const struct s s1 = {8};               // ❌ j는 명시적으로 초기화되지 않음
 741 |     int arr[10] = {10};                    // ❌ arr[0]만 10, 나머지는 0 → 의도 불명확
 742 |     int arr1[3][2] = {1, 2, 3, 4, 5, 6};   // ❌ 중첩 배열인데 flat하게 초기화 → 가독성 저하
 743 | }
```

### 🔍 Cppcheck 진단 경고 로그 (Raw Output)
| 라인 (Line) | 중요도 (Severity) | 경고 ID (Warning ID) | 분류 | 진단 메시지 (Message) |
| :---: | :---: | :---: | :---: | :--- |
| 736 | style | `unusedStructMember` | ⚠️ 파생 경고 (Collateral) | struct member 's::i' is never used. |
| 737 | style | `unusedStructMember` | ⚠️ 파생 경고 (Collateral) | struct member 's::j' is never used. |

---
