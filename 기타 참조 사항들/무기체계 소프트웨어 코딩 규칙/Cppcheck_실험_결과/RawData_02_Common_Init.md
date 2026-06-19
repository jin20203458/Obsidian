# Cppcheck 실험 검증 코드 및 로우데이터 - 공통 적용 (초기화)

본 문서는 DAPA 무기체계 소프트웨어 코딩 규칙 중 **공통 적용 (초기화)**에 속하는 규칙들의 실제 테스트 코드와 Cppcheck 정적 분석 결과(진단 경고 로우데이터)를 매핑한 자료입니다.

---

## 📌 Rule 13: char 배열의 초기화시 char 배열의 크기는 초기화 문자열의 크기와 Null을 포함하는 크기로 선언되어야 한다.

* **세부 조항**: 가
* **매핑 체커 (Clang-Tidy)**: `CharArrayStringSizeCheck`
* **최종 진단 판정**: **❌ 미탐 (Silent / False Negative) - 아무런 경고도 감지되지 않음**

### 💻 검증 소스코드
> 파일 위치: `dapa_test_suite.cpp` (L207 ~ L212)

```cpp
 207 | namespace Rule_13 {
 208 | #if 0
 209 |     char h1[4] = "Hello";  // ❌ 배열 크기 부족 (5글자 + 널 문자 필요)
 210 |     char h2[5] = "Hello";  // ❌ 널 문자('\0') 저장 공간 없음
 211 | #endif
 212 | }
```

### 🔍 Cppcheck 진단 경고 로그 (Raw Output)
> ❌ **검출된 경고 없음 (Silent)**

---

## 📌 Rule 14: 변수는 사용전 반드시 초기화되어야 한다.

* **세부 조항**: 나
* **매핑 체커 (Clang-Tidy)**: `InitBeforeUseCheck`
* **최종 진단 판정**: **✅ 정탐 (True Positive) - 의도된 결함 정확히 검출**

### 💻 검증 소스코드
> 파일 위치: `dapa_test_suite.cpp` (L218 ~ L225)

```cpp
 218 | namespace Rule_14 {
 219 |     void foo(void)
 220 |     {
 221 |         int i;
 222 |         int j;
 223 |         j = i + 2; // ❌ i는 초기화되지 않았음 → 정의되지 않은 동작
 224 |     }
 225 | }
```

### 🔍 Cppcheck 진단 경고 로그 (Raw Output)
| 라인 (Line) | 중요도 (Severity) | 경고 ID (Warning ID) | 분류 | 진단 메시지 (Message) |
| :---: | :---: | :---: | :---: | :--- |
| 223 | error | `uninitvar` | ✅ 정탐 경고 (Genuine) | Uninitialized variable: i |
| 223 | style | `unreadVariable` | ⚠️ 파생 경고 (Collateral) | Variable 'j' is assigned a value that is never used. |

---

## 📌 Rule 15: 값이 초기화되지 않은 변수의 포인터를 함수의 Read-only(const)로 사용해서는 안된다.

* **세부 조항**: 다
* **매핑 체커 (Clang-Tidy)**: `UninitializedAddressToConstParamCheck`
* **최종 진단 판정**: **✅ 정탐 (True Positive) - 의도된 결함 정확히 검출**

### 💻 검증 소스코드
> 파일 위치: `dapa_test_suite.cpp` (L231 ~ L240)

```cpp
 231 | namespace Rule_15 {
 232 |     extern int foo(const int *p);
 233 | 
 234 |     void foo1(void)
 235 |     {
 236 |         int y;
 237 |         int r;
 238 |         r = foo(&y); // ❌ y는 초기화되지 않았음
 239 |     }
 240 | }
```

### 🔍 Cppcheck 진단 경고 로그 (Raw Output)
| 라인 (Line) | 중요도 (Severity) | 경고 ID (Warning ID) | 분류 | 진단 메시지 (Message) |
| :---: | :---: | :---: | :---: | :--- |
| 238 | error | `uninitvar` | ✅ 정탐 경고 (Genuine) | Uninitialized variable: &y |
| 238 | style | `unreadVariable` | ⚠️ 파생 경고 (Collateral) | Variable 'r' is assigned a value that is never used. |

---
