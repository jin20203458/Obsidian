# Cppcheck 실험 검증 코드 및 로우데이터 - 공통 적용 (코딩 스타일)

본 문서는 DAPA 무기체계 소프트웨어 코딩 규칙 중 **공통 적용 (코딩 스타일)**에 속하는 규칙들의 실제 테스트 코드와 Cppcheck 정적 분석 결과(진단 경고 로우데이터)를 매핑한 자료입니다.

---

## 📌 Rule 01: Placeholder - Parse Failed

* **세부 조항**: 가
* **매핑 체커 (Clang-Tidy)**: `UnknownChecker`
* **최종 진단 판정**: **⚪ 분석 제외 (정적분석 진단 범위 밖)**

### 💻 검증 소스코드
> 파일 위치: `dapa_test_suite.cpp` (L19 ~ L21)

```cpp
  19 | namespace Rule_01 {
  20 |     // Parse failed for this rule
  21 | }
```

### 🔍 Cppcheck 진단 경고 로그 (Raw Output)
> 정적 분석이 불가능한 규칙으로 제외되었습니다. (예: 주석 비율)

---

## 📌 Rule 02: Switch 구문에서 첫번째 Label 전에 코드 구문이 존재하면 안된다.

* **세부 조항**: 나
* **매핑 체커 (Clang-Tidy)**: `SwitchStyleCheck`
* **최종 진단 판정**: **❌ 미탐 (Silent / False Negative) - 아무런 경고도 감지되지 않음**

### 💻 검증 소스코드
> 파일 위치: `dapa_test_suite.cpp` (L27 ~ L41)

```cpp
  27 | namespace Rule_02 {
  28 |     void foo(int m)
  29 |     {
  30 |         switch (m)
  31 |         {
  32 |             n = n + 1; // ❌ 잘못된 위치: 어떤 case label보다 먼저 실행될 수 없음
  33 |             case 1:
  34 |                 --n;
  35 |                 break;
  36 |             default:
  37 |                 ++n;
  38 |                 break;
  39 |         }
  40 |     }
  41 | }
```

### 🔍 Cppcheck 진단 경고 로그 (Raw Output)
> ❌ **검출된 경고 없음 (Silent)**

---

## 📌 Rule 03: 함수/변수의 선언 시 type을 명시해야 한다. (사용자정의해더 검사사)

* **세부 조항**: 다
* **매핑 체커 (Clang-Tidy)**: `NoAutoTypeCheck`
* **최종 진단 판정**: **❌ 미탐 (Silent / False Negative) - 아무런 경고도 감지되지 않음**

### 💻 검증 소스코드
> 파일 위치: `dapa_test_suite.cpp` (L47 ~ L52)

```cpp
  47 | namespace Rule_03 {
  48 |     auto x = 10;         // ❌ 타입이 명시되지 않음
  49 |     auto foo() {         // ❌ 반환 타입 생략
  50 |         return 42;
  51 |     }
  52 | }
```

### 🔍 Cppcheck 진단 경고 로그 (Raw Output)
> ❌ **검출된 경고 없음 (Silent)**

---

## 📌 Rule 04: 의미 없는 구문은 사용하지 말아야 한다.(side effect)

* **세부 조항**: 라
* **매핑 체커 (Clang-Tidy)**: `NoMeaninglessExprCheck`
* **최종 진단 판정**: **❌ 미탐 (Silent / False Negative) - 아무런 경고도 감지되지 않음**

### 💻 검증 소스코드
> 파일 위치: `dapa_test_suite.cpp` (L58 ~ L64)

```cpp
  58 | namespace Rule_04 {
  59 |     void foo(int x)
  60 |     {
  61 |         int n = 0;
  62 |         n == x; // ❌ 의미 없는 비교: 결과를 사용하지 않음
  63 |     }
  64 | }
```

### 🔍 Cppcheck 진단 경고 로그 (Raw Output)
> ❌ **검출된 경고 없음 (Silent)**

---

## 📌 Rule 05: 함수 선언시 사용된 리턴형과 함수 내에서 사용되는 리턴형은 일치해야 한다.

* **세부 조항**: 마
* **매핑 체커 (Clang-Tidy)**: `SingleExitAndReturnTypeCheck`
* **최종 진단 판정**: **✅ 정탐 (True Positive) - 의도된 결함 정확히 검출**

### 💻 검증 소스코드
> 파일 위치: `dapa_test_suite.cpp` (L70 ~ L82)

```cpp
  70 | namespace Rule_05 {
  71 |     int f1(int value) {
  72 |         return;       // ❌ int를 반환해야 하는데 아무것도 반환하지 않음
  73 |     }
  74 | 
  75 |     void f2(int value) {
  76 |         return value; // ❌ void 함수는 값을 반환할 수 없음
  77 |     }
  78 | 
  79 |     int f1(int value) {
  80 |         // ❌ return 문이 없음 → 경고 또는 정의되지 않은 동작
  81 |     }
  82 | }
```

### 🔍 Cppcheck 진단 경고 로그 (Raw Output)
| 라인 (Line) | 중요도 (Severity) | 경고 ID (Warning ID) | 분류 | 진단 메시지 (Message) |
| :---: | :---: | :---: | :---: | :--- |
| 81 | error | `missingReturn` | ✅ 정탐 경고 (Genuine) | Found an exit path from function with non-void return type that has missing return statement |

---

## 📌 Rule 06: 외부 함수 사용시 이를 명시하고 사용해야 한다.

* **세부 조항**: 바
* **매핑 체커 (Clang-Tidy)**: `ExternFunctionDeclarationCheck`
* **최종 진단 판정**: **❌ 미탐 (Silent / False Negative) - 아무런 경고도 감지되지 않음**

### 💻 검증 소스코드
> 파일 위치: `dapa_test_suite.cpp` (L88 ~ L95)

```cpp
  88 | namespace Rule_06 {
  89 |     int foo(void)
  90 |     {
  91 |         int r;
  92 |         r = func(0); // ❌ func가 선언되지 않았음
  93 |         return r;
  94 |     }
  95 | }
```

### 🔍 Cppcheck 진단 경고 로그 (Raw Output)
> ❌ **검출된 경고 없음 (Silent)**

---

## 📌 Rule 07: 전처리에 사용되는 매크로는 사용 전에 반드시 정의되어야 한다.(사용자정의해더 검사사)

* **세부 조항**: 사
* **매핑 체커 (Clang-Tidy)**: `MacroDefinedBeforeUseCheck`
* **최종 진단 판정**: **❌ 미탐 (Silent / False Negative) - 아무런 경고도 감지되지 않음**

### 💻 검증 소스코드
> 파일 위치: `dapa_test_suite.cpp` (L101 ~ L107)

```cpp
 101 | namespace Rule_07 {
 102 |     #define M1 5
 103 | 
 104 |     #if (M2 == 5) // ❌ M2는 정의되지 않았음
 105 |     int y = 0;
 106 |     #endif
 107 | }
```

### 🔍 Cppcheck 진단 경고 로그 (Raw Output)
> ❌ **검출된 경고 없음 (Silent)**

---

## 📌 Rule 08: goto 문을 사용하지 말아야 한다.

* **세부 조항**: 아
* **매핑 체커 (Clang-Tidy)**: `NoGotoCheck`
* **최종 진단 판정**: **✅ 정탐 (True Positive) - 의도된 결함 정확히 검출**

### 💻 검증 소스코드
> 파일 위치: `dapa_test_suite.cpp` (L113 ~ L124)

```cpp
 113 | namespace Rule_08 {
 114 |     short test_1404(void)
 115 |     {
 116 |         if (abc > 10)
 117 |         {
 118 |             goto mylabel; // ❌ 흐름이 갑자기 점프함
 119 |         }
 120 |         abc++;
 121 |     mylabel:
 122 |         ...
 123 |     }
 124 | }
```

### 🔍 Cppcheck 진단 경고 로그 (Raw Output)
| 라인 (Line) | 중요도 (Severity) | 경고 ID (Warning ID) | 분류 | 진단 메시지 (Message) |
| :---: | :---: | :---: | :---: | :--- |
| 120 | error | `missingReturn` | ✅ 정탐 경고 (Genuine) | Found an exit path from function with non-void return type that has missing return statement |

---

## 📌 Rule 09: 하나의 함수는 하나의 Exit Point를 가져야 한다.

* **세부 조항**: 자
* **매핑 체커 (Clang-Tidy)**: `SingleExitAndReturnTypeCheck`
* **최종 진단 판정**: **❌ 미탐 (Silent / False Negative) - 아무런 경고도 감지되지 않음**

### 💻 검증 소스코드
> 파일 위치: `dapa_test_suite.cpp` (L130 ~ L142)

```cpp
 130 | namespace Rule_09 {
 131 |     int S16test_1407(void)
 132 |     {
 133 |         if (s16a > 0)
 134 |         {
 135 |             return 0;  // ❌ 첫 번째 exit point
 136 |         }
 137 |         else
 138 |         {
 139 |             return 1;  // ❌ 두 번째 exit point
 140 |         }
 141 |     }
 142 | }
```

### 🔍 Cppcheck 진단 경고 로그 (Raw Output)
> ❌ **검출된 경고 없음 (Silent)**

---

## 📌 Rule 10: switch ~ case 문은 default 문이 포함되어야 한다.

* **세부 조항**: 차
* **매핑 체커 (Clang-Tidy)**: `SwitchStyleCheck`
* **최종 진단 판정**: **❌ 미탐 (Silent / False Negative) - 아무런 경고도 감지되지 않음**

### 💻 검증 소스코드
> 파일 위치: `dapa_test_suite.cpp` (L148 ~ L158)

```cpp
 148 | namespace Rule_10 {
 149 |     switch (s16a)
 150 |     {
 151 |     case 0:
 152 |         x = 2;
 153 |         break;
 154 |     case 1:
 155 |         x = 1;
 156 |         break;
 157 |     }  /* default 없음 */
 158 | }
```

### 🔍 Cppcheck 진단 경고 로그 (Raw Output)
> ❌ **검출된 경고 없음 (Silent)**

---

## 📌 Rule 11: 한 줄에 하나의 명령문을 사용한다.

* **세부 조항**: 카
* **매핑 체커 (Clang-Tidy)**: `MultiStatementPerLineCheck`
* **최종 진단 판정**: **❌ 미탐 (Silent / False Negative) - 아무런 경고도 감지되지 않음**

### 💻 검증 소스코드
> 파일 위치: `dapa_test_suite.cpp` (L164 ~ L166)

```cpp
 164 | namespace Rule_11 {
 165 |     r = p; p++; // ❌ 두 개의 명령문이 한 줄에 있음
 166 | }
```

### 🔍 Cppcheck 진단 경고 로그 (Raw Output)
> ❌ **검출된 경고 없음 (Silent)**

---

## 📌 Rule 12: if - else  if 문은 else 문도 반드시 포함 시킨다.

* **세부 조항**: 타
* **매핑 체커 (Clang-Tidy)**: `IfStyleCheck`
* **최종 진단 판정**: **❌ 미탐 (Silent / False Negative) - 아무런 경고도 감지되지 않음**

### 💻 검증 소스코드
> 파일 위치: `dapa_test_suite.cpp` (L172 ~ L181)

```cpp
 172 | namespace Rule_12 {
 173 |     if (s16a == 2)
 174 |     {
 175 |         r = 3;
 176 |     }
 177 |     else if (s16a == 3) // ❌ else 없음
 178 |     {
 179 |         r = 2;
 180 |     }
 181 | }
```

### 🔍 Cppcheck 진단 경고 로그 (Raw Output)
> ❌ **검출된 경고 없음 (Silent)**

---
