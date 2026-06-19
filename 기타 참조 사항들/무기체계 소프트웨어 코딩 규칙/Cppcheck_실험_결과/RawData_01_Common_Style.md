# Cppcheck 실험 검증 코드 및 로우데이터 - 공통 적용 (코딩 스타일)

본 문서는 DAPA 무기체계 소프트웨어 코딩 규칙 중 **공통 적용 (코딩 스타일)**에 속하는 규칙들의 실제 테스트 코드와 Cppcheck 정적 분석 결과(진단 경고 로우데이터)를 매핑한 자료입니다.

---

## 📌 Rule 01: Placeholder - Parse Failed

* **세부 조항**: 가
* **매핑 체커 (Clang-Tidy)**: `UnknownChecker`
* **최종 진단 판정**: **⚪ 분석 제외 (정적분석 진단 범위 밖)**

### 💻 검증 소스코드
> 파일 위치: `dapa_test_suite.cpp` (L20 ~ L22)

```cpp
  20 | namespace Rule_01 {
  21 |     // Parse failed for this rule
  22 | }
```

### 🔍 Cppcheck 진단 경고 로그 (Raw Output)
> 정적 분석이 불가능한 규칙으로 제외되었습니다. (예: 주석 비율)

---

## 📌 Rule 02: Switch 구문에서 첫번째 Label 전에 코드 구문이 존재하면 안된다.

* **세부 조항**: 나
* **매핑 체커 (Clang-Tidy)**: `SwitchStyleCheck`
* **최종 진단 판정**: **⚠️ 미탐 (파생 경고만 발생) - 결함은 미탐지하고 부수적 경고만 발생**

### 💻 검증 소스코드
> 파일 위치: `dapa_test_suite.cpp` (L28 ~ L43)

```cpp
  28 | namespace Rule_02 {
  29 |     void foo(int m)
  30 |     {
  31 |         int n = 0;
  32 |         switch (m)
  33 |         {
  34 |             n = n + 1; // ❌ 잘못된 위치: 어떤 case label보다 먼저 실행될 수 없음
  35 |             case 1:
  36 |                 --n;
  37 |                 break;
  38 |             default:
  39 |                 ++n;
  40 |                 break;
  41 |         }
  42 |     }
  43 | }
```

### 🔍 Cppcheck 진단 경고 로그 (Raw Output)
| 라인 (Line) | 중요도 (Severity) | 경고 ID (Warning ID) | 분류 | 진단 메시지 (Message) |
| :---: | :---: | :---: | :---: | :--- |
| 36 | style | `unreadVariable` | ⚠️ 파생 경고 (Collateral) | Variable 'n' is assigned a value that is never used. |
| 39 | style | `unreadVariable` | ⚠️ 파생 경고 (Collateral) | Variable 'n' is assigned a value that is never used. |

---

## 📌 Rule 03: 함수/변수의 선언 시 type을 명시해야 한다. (사용자정의해더 검사사)

* **세부 조항**: 다
* **매핑 체커 (Clang-Tidy)**: `NoAutoTypeCheck`
* **최종 진단 판정**: **❌ 미탐 (Silent / False Negative) - 아무런 경고도 감지되지 않음**

### 💻 검증 소스코드
> 파일 위치: `dapa_test_suite.cpp` (L49 ~ L54)

```cpp
  49 | namespace Rule_03 {
  50 |     auto x = 10;         // ❌ 타입이 명시되지 않음
  51 |     auto foo() {         // ❌ 반환 타입 생략
  52 |         return 42;
  53 |     }
  54 | }
```

### 🔍 Cppcheck 진단 경고 로그 (Raw Output)
> ❌ **검출된 경고 없음 (Silent)**

---

## 📌 Rule 04: 의미 없는 구문은 사용하지 말아야 한다.(side effect)

* **세부 조항**: 라
* **매핑 체커 (Clang-Tidy)**: `NoMeaninglessExprCheck`
* **최종 진단 판정**: **❌ 미탐 (Silent / False Negative) - 아무런 경고도 감지되지 않음**

### 💻 검증 소스코드
> 파일 위치: `dapa_test_suite.cpp` (L60 ~ L66)

```cpp
  60 | namespace Rule_04 {
  61 |     void foo(int x)
  62 |     {
  63 |         int n = 0;
  64 |         n == x; // ❌ 의미 없는 비교: 결과를 사용하지 않음
  65 |     }
  66 | }
```

### 🔍 Cppcheck 진단 경고 로그 (Raw Output)
> ❌ **검출된 경고 없음 (Silent)**

---

## 📌 Rule 05: 함수 선언시 사용된 리턴형과 함수 내에서 사용되는 리턴형은 일치해야 한다.

* **세부 조항**: 마
* **매핑 체커 (Clang-Tidy)**: `SingleExitAndReturnTypeCheck`
* **최종 진단 판정**: **✅ 정탐 (True Positive) - 의도된 결함 정확히 검출**

### 💻 검증 소스코드
> 파일 위치: `dapa_test_suite.cpp` (L72 ~ L86)

```cpp
  72 | namespace Rule_05 {
  73 | #if 0
  74 |     int f1(int value) {
  75 |         return;       // ❌ int를 반환해야 하는데 아무것도 반환하지 않음
  76 |     }
  77 | 
  78 |     void f2(int value) {
  79 |         return value; // ❌ void 함수는 값을 반환할 수 없음
  80 |     }
  81 | #endif
  82 | 
  83 |     int f3(int value) {
  84 |         // ❌ return 문이 없음 → 경고 또는 정의되지 않은 동작
  85 |     }
  86 | }
```

### 🔍 Cppcheck 진단 경고 로그 (Raw Output)
| 라인 (Line) | 중요도 (Severity) | 경고 ID (Warning ID) | 분류 | 진단 메시지 (Message) |
| :---: | :---: | :---: | :---: | :--- |
| 85 | error | `missingReturn` | ✅ 정탐 경고 (Genuine) | Found an exit path from function with non-void return type that has missing return statement |

---

## 📌 Rule 06: 외부 함수 사용시 이를 명시하고 사용해야 한다.

* **세부 조항**: 바
* **매핑 체커 (Clang-Tidy)**: `ExternFunctionDeclarationCheck`
* **최종 진단 판정**: **❌ 미탐 (Silent / False Negative) - 아무런 경고도 감지되지 않음**

### 💻 검증 소스코드
> 파일 위치: `dapa_test_suite.cpp` (L92 ~ L101)

```cpp
  92 | namespace Rule_06 {
  93 |     int foo(void)
  94 |     {
  95 |         int r = 0;
  96 | #if 0
  97 |         r = func(0); // ❌ func가 선언되지 않았음 (compiler error in C++)
  98 | #endif
  99 |         return r;
 100 |     }
 101 | }
```

### 🔍 Cppcheck 진단 경고 로그 (Raw Output)
> ❌ **검출된 경고 없음 (Silent)**

---

## 📌 Rule 07: 전처리에 사용되는 매크로는 사용 전에 반드시 정의되어야 한다.(사용자정의해더 검사사)

* **세부 조항**: 사
* **매핑 체커 (Clang-Tidy)**: `MacroDefinedBeforeUseCheck`
* **최종 진단 판정**: **❌ 미탐 (Silent / False Negative) - 아무런 경고도 감지되지 않음**

### 💻 검증 소스코드
> 파일 위치: `dapa_test_suite.cpp` (L107 ~ L113)

```cpp
 107 | namespace Rule_07 {
 108 |     #define M1 5
 109 | 
 110 |     #if (M2 == 5) // ❌ M2는 정의되지 않았음
 111 |     int y = 0;
 112 |     #endif
 113 | }
```

### 🔍 Cppcheck 진단 경고 로그 (Raw Output)
> ❌ **검출된 경고 없음 (Silent)**

---

## 📌 Rule 08: goto 문을 사용하지 말아야 한다.

* **세부 조항**: 아
* **매핑 체커 (Clang-Tidy)**: `NoGotoCheck`
* **최종 진단 판정**: **✅ 정탐 (True Positive) - 의도된 결함 정확히 검출**

### 💻 검증 소스코드
> 파일 위치: `dapa_test_suite.cpp` (L119 ~ L131)

```cpp
 119 | namespace Rule_08 {
 120 |     short test_1404(void)
 121 |     {
 122 |         int abc = 0;
 123 |         if (abc > 10)
 124 |         {
 125 |             goto mylabel; // ❌ 흐름이 갑자기 점프함
 126 |         }
 127 |         abc++;
 128 |     mylabel:
 129 |         return 0;
 130 |     }
 131 | }
```

### 🔍 Cppcheck 진단 경고 로그 (Raw Output)
| 라인 (Line) | 중요도 (Severity) | 경고 ID (Warning ID) | 분류 | 진단 메시지 (Message) |
| :---: | :---: | :---: | :---: | :--- |
| 123 | style | `knownConditionTrueFalse` | ✅ 정탐 경고 (Genuine) | Condition 'abc>10' is always false |
| 127 | style | `unreadVariable` | ⚠️ 파생 경고 (Collateral) | Variable 'abc' is assigned a value that is never used. |

---

## 📌 Rule 09: 하나의 함수는 하나의 Exit Point를 가져야 한다.

* **세부 조항**: 자
* **매핑 체커 (Clang-Tidy)**: `SingleExitAndReturnTypeCheck`
* **최종 진단 판정**: **❌ 미탐 (Silent / False Negative) - 아무런 경고도 감지되지 않음**

### 💻 검증 소스코드
> 파일 위치: `dapa_test_suite.cpp` (L137 ~ L149)

```cpp
 137 | namespace Rule_09 {
 138 |     int S16test_1407(int s16a)
 139 |     {
 140 |         if (s16a > 0)
 141 |         {
 142 |             return 0;  // ❌ 첫 번째 exit point
 143 |         }
 144 |         else
 145 |         {
 146 |             return 1;  // ❌ 두 번째 exit point
 147 |         }
 148 |     }
 149 | }
```

### 🔍 Cppcheck 진단 경고 로그 (Raw Output)
> ❌ **검출된 경고 없음 (Silent)**

---

## 📌 Rule 10: switch ~ case 문은 default 문이 포함되어야 한다.

* **세부 조항**: 차
* **매핑 체커 (Clang-Tidy)**: `SwitchStyleCheck`
* **최종 진단 판정**: **⚠️ 미탐 (파생 경고만 발생) - 결함은 미탐지하고 부수적 경고만 발생**

### 💻 검증 소스코드
> 파일 위치: `dapa_test_suite.cpp` (L155 ~ L169)

```cpp
 155 | namespace Rule_10 {
 156 |     void foo(int s16a)
 157 |     {
 158 |         int x = 0;
 159 |         switch (s16a)
 160 |         {
 161 |         case 0:
 162 |             x = 2;
 163 |             break;
 164 |         case 1:
 165 |             x = 1;
 166 |             break;
 167 |         }  /* default 없음 */
 168 |     }
 169 | }
```

### 🔍 Cppcheck 진단 경고 로그 (Raw Output)
| 라인 (Line) | 중요도 (Severity) | 경고 ID (Warning ID) | 분류 | 진단 메시지 (Message) |
| :---: | :---: | :---: | :---: | :--- |
| 162 | style | `unreadVariable` | ⚠️ 파생 경고 (Collateral) | Variable 'x' is assigned a value that is never used. |
| 165 | style | `unreadVariable` | ⚠️ 파생 경고 (Collateral) | Variable 'x' is assigned a value that is never used. |

---

## 📌 Rule 11: 한 줄에 하나의 명령문을 사용한다.

* **세부 조항**: 카
* **매핑 체커 (Clang-Tidy)**: `MultiStatementPerLineCheck`
* **최종 진단 판정**: **⚠️ 미탐 (파생 경고만 발생) - 결함은 미탐지하고 부수적 경고만 발생**

### 💻 검증 소스코드
> 파일 위치: `dapa_test_suite.cpp` (L175 ~ L182)

```cpp
 175 | namespace Rule_11 {
 176 |     void foo()
 177 |     {
 178 |         int r = 0;
 179 |         int p = 0;
 180 |         r = p; p++; // ❌ 두 개의 명령문이 한 줄에 있음
 181 |     }
 182 | }
```

### 🔍 Cppcheck 진단 경고 로그 (Raw Output)
| 라인 (Line) | 중요도 (Severity) | 경고 ID (Warning ID) | 분류 | 진단 메시지 (Message) |
| :---: | :---: | :---: | :---: | :--- |
| 180 | style | `unreadVariable` | ⚠️ 파생 경고 (Collateral) | Variable 'r' is assigned a value that is never used. |
| 180 | style | `unreadVariable` | ⚠️ 파생 경고 (Collateral) | Variable 'p' is assigned a value that is never used. |

---

## 📌 Rule 12: if - else  if 문은 else 문도 반드시 포함 시킨다.

* **세부 조항**: 타
* **매핑 체커 (Clang-Tidy)**: `IfStyleCheck`
* **최종 진단 판정**: **⚠️ 미탐 (파생 경고만 발생) - 결함은 미탐지하고 부수적 경고만 발생**

### 💻 검증 소스코드
> 파일 위치: `dapa_test_suite.cpp` (L188 ~ L201)

```cpp
 188 | namespace Rule_12 {
 189 |     void foo(int s16a)
 190 |     {
 191 |         int r = 0;
 192 |         if (s16a == 2)
 193 |         {
 194 |             r = 3;
 195 |         }
 196 |         else if (s16a == 3) // ❌ else 없음
 197 |         {
 198 |             r = 2;
 199 |         }
 200 |     }
 201 | }
```

### 🔍 Cppcheck 진단 경고 로그 (Raw Output)
| 라인 (Line) | 중요도 (Severity) | 경고 ID (Warning ID) | 분류 | 진단 메시지 (Message) |
| :---: | :---: | :---: | :---: | :--- |
| 191 | style | `unreadVariable` | ⚠️ 파생 경고 (Collateral) | Variable 'r' is assigned a value that is never used. |
| 194 | style | `unreadVariable` | ⚠️ 파생 경고 (Collateral) | Variable 'r' is assigned a value that is never used. |
| 198 | style | `unreadVariable` | ⚠️ 파생 경고 (Collateral) | Variable 'r' is assigned a value that is never used. |

---
