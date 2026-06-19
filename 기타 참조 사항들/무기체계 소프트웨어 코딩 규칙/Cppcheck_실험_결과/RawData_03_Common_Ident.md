# Cppcheck 실험 검증 코드 및 로우데이터 - 공통 적용 (식별자)

본 문서는 DAPA 무기체계 소프트웨어 코딩 규칙 중 **공통 적용 (식별자)**에 속하는 규칙들의 실제 테스트 코드와 Cppcheck 정적 분석 결과(진단 경고 로우데이터)를 매핑한 자료입니다.

---

## 📌 Rule 16: external과 internal linkage의 특성을 동시에 가질 수 없다.

* **세부 조항**: 가
* **매핑 체커 (Clang-Tidy)**: `UnknownChecker`
* **최종 진단 판정**: **❌ 미탐 (Silent / False Negative) - 아무런 경고도 감지되지 않음**

### 💻 검증 소스코드
> 파일 위치: `dapa_test_suite.cpp` (L224 ~ L230)

```cpp
 224 | namespace Rule_16 {
 225 |     extern int foo(int value);
 226 |     extern int number;
 227 |     /* ... */
 228 |     static int number;       // ❌ 외부에서 참조하겠다고 해놓고 내부 전용으로 선언
 229 |     static int foo(int value); // ❌ 외부 함수라고 해놓고 내부 전용으로 선언
 230 | }
```

### 🔍 Cppcheck 진단 경고 로그 (Raw Output)
> ❌ **검출된 경고 없음 (Silent)**

---

## 📌 Rule 17: external linkage scope에서 선언된 함수나 Object의 이름은 유일해야 한다.

* **세부 조항**: 나
* **매핑 체커 (Clang-Tidy)**: `UnknownChecker`
* **최종 진단 판정**: **❌ 미탐 (Silent / False Negative) - 아무런 경고도 감지되지 않음**

### 💻 검증 소스코드
> 파일 위치: `dapa_test_suite.cpp` (L236 ~ L244)

```cpp
 236 | namespace Rule_17 {
 237 |     // TU1.c
 238 |     int press;
 239 |     void foo(void) { }
 240 | 
 241 |     // TU2.c
 242 |     float press;       // ❌ press가 중복 정의됨
 243 |     void foo(int) { }  // ❌ foo가 다른 시그니처로 중복 정의됨
 244 | }
```

### 🔍 Cppcheck 진단 경고 로그 (Raw Output)
> ❌ **검출된 경고 없음 (Silent)**

---

## 📌 Rule 18: external linkage scope에서 정의된 함수나 Object의 데이터형은 선언 시 정의와 동일해야 한다.

* **세부 조항**: 다
* **매핑 체커 (Clang-Tidy)**: `UnknownChecker`
* **최종 진단 판정**: **❌ 미탐 (Silent / False Negative) - 아무런 경고도 감지되지 않음**

### 💻 검증 소스코드
> 파일 위치: `dapa_test_suite.cpp` (L250 ~ L260)

```cpp
 250 | namespace Rule_18 {
 251 |     // TU1.c
 252 |     extern int i;            // ❌ 실제로는 float인데 int로 선언
 253 |     extern void foo(int);    // ❌ 실제로는 int 반환인데 void로 선언
 254 | 
 255 |     // TU2.c
 256 |     float i = 0.0;
 257 |     int foo(int x) {
 258 |         return x + 1;
 259 |     }
 260 | }
```

### 🔍 Cppcheck 진단 경고 로그 (Raw Output)
> ❌ **검출된 경고 없음 (Silent)**

---

## 📌 Rule 19: 외부 scope에서 정의된 변수를 내부 scope에서 재정의 해서는 안된다.

* **세부 조항**: 라
* **매핑 체커 (Clang-Tidy)**: `NoShadowingCheck`
* **최종 진단 판정**: **✅ 정탐 (True Positive) - 의도된 결함 정확히 검출**

### 💻 검증 소스코드
> 파일 위치: `dapa_test_suite.cpp` (L266 ~ L278)

```cpp
 266 | namespace Rule_19 {
 267 |     void func(int a)
 268 |     {
 269 |         int variable = 6;
 270 | 
 271 |         if (a > 0) {
 272 |             int variable;     // ❌ 같은 이름으로 재정의 (shadowing)
 273 |             variable = 10;
 274 |         }
 275 | 
 276 |         func2(variable);      // variable은 6 → 내부에서 10으로 바꾼 줄 착각할 수 있음
 277 |     }
 278 | }
```

### 🔍 Cppcheck 진단 경고 로그 (Raw Output)
| 라인 (Line) | 중요도 (Severity) | 경고 ID (Warning ID) | 분류 | 진단 메시지 (Message) |
| :---: | :---: | :---: | :---: | :--- |
| 272 | style | `shadowVariable` | ✅ 정탐 경고 (Genuine) | Local variable 'variable' shadows outer variable |
| 273 | style | `unreadVariable` | ⚠️ 파생 경고 (Collateral) | Variable 'variable' is assigned a value that is never used. |

---
