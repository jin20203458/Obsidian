# Cppcheck 실험 검증 코드 및 로우데이터 - 공통 적용 (조건식)

본 문서는 DAPA 무기체계 소프트웨어 코딩 규칙 중 **공통 적용 (조건식)**에 속하는 규칙들의 실제 테스트 코드와 Cppcheck 정적 분석 결과(진단 경고 로우데이터)를 매핑한 자료입니다.

---

## 📌 Rule 20: float 자료형에서 동등성 비교연산을 수행하지 말아야 한다.

* **세부 조항**: 가
* **매핑 체커 (Clang-Tidy)**: `NoFloatEqualityCheck`
* **최종 진단 판정**: **❌ 미탐 (Silent / False Negative) - 아무런 경고도 감지되지 않음**

### 💻 검증 소스코드
> 파일 위치: `dapa_test_suite.cpp` (L284 ~ L294)

```cpp
 284 | namespace Rule_20 {
 285 |     int foo(float a, float b)
 286 |     {
 287 |         int r = 0;
 288 |         if (a == b)  // ❌ 부동소수점 비교는 신뢰할 수 없음
 289 |         {
 290 |             r = 1;
 291 |         }
 292 |         return r;
 293 |     }
 294 | }
```

### 🔍 Cppcheck 진단 경고 로그 (Raw Output)
> ❌ **검출된 경고 없음 (Silent)**

---

## 📌 Rule 21: 조건문의 결과가 항상 True 혹은 False라면 이는 조건문으로 작성해서는 안된다.

* **세부 조항**: 나
* **매핑 체커 (Clang-Tidy)**: `AlwaysConstantConditionCheck`
* **최종 진단 판정**: **✅ 정탐 (True Positive) - 의도된 결함 정확히 검출**

### 💻 검증 소스코드
> 파일 위치: `dapa_test_suite.cpp` (L300 ~ L320)

```cpp
 300 | namespace Rule_21 {
 301 |     void foo(unsigned char x, int y)
 302 |     {
 303 |         if (y < 20)
 304 |         {
 305 |             if (y < 30)  // ❌ 항상 참 (y < 20이면 당연히 y < 30)
 306 |             {
 307 |                 y = 30;
 308 |             }
 309 |             if (y > 30)  // ❌ 항상 거짓 (y < 20이면 y > 30일 수 없음)
 310 |             {
 311 |                 y = 30;
 312 |             }
 313 |         }
 314 | 
 315 |         if (x < 0)  // ❌ 항상 거짓 (unsigned char는 음수가 될 수 없음)
 316 |         {
 317 |             x = 0;
 318 |         }
 319 |     }
 320 | }
```

### 🔍 Cppcheck 진단 경고 로그 (Raw Output)
| 라인 (Line) | 중요도 (Severity) | 경고 ID (Warning ID) | 분류 | 진단 메시지 (Message) |
| :---: | :---: | :---: | :---: | :--- |
| 311 | style | `uselessAssignmentArg` | ⚠️ 파생 경고 (Collateral) | Assignment of function parameter has no effect outside the function. |
| 317 | style | `uselessAssignmentArg` | ⚠️ 파생 경고 (Collateral) | Assignment of function parameter has no effect outside the function. |
| 305 | style | `knownConditionTrueFalse` | ✅ 정탐 경고 (Genuine) | Condition 'y<30' is always true |
| 309 | style | `knownConditionTrueFalse` | ✅ 정탐 경고 (Genuine) | Condition 'y>30' is always false |
| 315 | style | `unsignedLessThanZero` | ✅ 정탐 경고 (Genuine) | Checking if unsigned expression 'x' is less than zero. |
| 311 | style | `unreadVariable` | ⚠️ 파생 경고 (Collateral) | Variable 'y' is assigned a value that is never used. |
| 317 | style | `unreadVariable` | ⚠️ 파생 경고 (Collateral) | Variable 'x' is assigned a value that is never used. |

---

## 📌 Rule 22: switch의 case의 조건을 만족할 수 없는 Label을 사용하지 않는다.

* **세부 조항**: 다
* **매핑 체커 (Clang-Tidy)**: `UnknownChecker`
* **최종 진단 판정**: **❌ 미탐 (Silent / False Negative) - 아무런 경고도 감지되지 않음**

### 💻 검증 소스코드
> 파일 위치: `dapa_test_suite.cpp` (L326 ~ L341)

```cpp
 326 | namespace Rule_22 {
 327 |     extern int x;
 328 | 
 329 |     void foo(unsigned int n)
 330 |     {
 331 |         switch (n)
 332 |         {
 333 |             case -1:      // ❌ unsigned int는 음수가 될 수 없음 → dead code
 334 |                 ++x;
 335 |                 break;
 336 |             default:
 337 |                 --x;
 338 |                 break;
 339 |         }
 340 |     }
 341 | }
```

### 🔍 Cppcheck 진단 경고 로그 (Raw Output)
> ❌ **검출된 경고 없음 (Silent)**

---

## 📌 Rule 23: Switch 구문에서 Expression을 논리적 연산으로 사용하지 말아야 한다.

* **세부 조항**: 라
* **매핑 체커 (Clang-Tidy)**: `SwitchStyleCheck`
* **최종 진단 판정**: **❌ 미탐 (Silent / False Negative) - 아무런 경고도 감지되지 않음**

### 💻 검증 소스코드
> 파일 위치: `dapa_test_suite.cpp` (L347 ~ L362)

```cpp
 347 | namespace Rule_23 {
 348 |     extern int x;
 349 | 
 350 |     void foo(int option, int pick)
 351 |     {
 352 |         switch (option == pick)  // ❌ 논리 연산 결과를 switch에 사용
 353 |         {
 354 |             case 0:
 355 |                 ++x;
 356 |                 break;
 357 |             case 1:
 358 |                 x = 0;
 359 |                 break;
 360 |         }
 361 |     }
 362 | }
```

### 🔍 Cppcheck 진단 경고 로그 (Raw Output)
> ❌ **검출된 경고 없음 (Silent)**

---

## 📌 Rule 24: 수행되지 않는 소스코드를 작성하지 말아야 한다.

* **세부 조항**: 마
* **매핑 체커 (Clang-Tidy)**: `UnreachableCodeCheck`
* **최종 진단 판정**: **✅ 정탐 (True Positive) - 의도된 결함 정확히 검출**

### 💻 검증 소스코드
> 파일 위치: `dapa_test_suite.cpp` (L368 ~ L382)

```cpp
 368 | namespace Rule_24 {
 369 |     void foo(int n)
 370 |     {
 371 |         switch (n)
 372 |         {
 373 |             case 1:
 374 |                 --n;
 375 |                 break;
 376 |                 n = n + 1;  // ❌ 절대 실행되지 않음 (break 이후의 코드)
 377 |             default:
 378 |                 ++n;
 379 |                 break;
 380 |         }
 381 |     }
 382 | }
```

### 🔍 Cppcheck 진단 경고 로그 (Raw Output)
| 라인 (Line) | 중요도 (Severity) | 경고 ID (Warning ID) | 분류 | 진단 메시지 (Message) |
| :---: | :---: | :---: | :---: | :--- |
| 378 | style | `uselessAssignmentArg` | ⚠️ 파생 경고 (Collateral) | Assignment of function parameter has no effect outside the function. |
| 376 | style | `unreachableCode` | ✅ 정탐 경고 (Genuine) | Statements following 'break' will never be executed. |
| 374 | style | `unreadVariable` | ⚠️ 파생 경고 (Collateral) | Variable 'n' is assigned a value that is never used. |
| 378 | style | `unreadVariable` | ⚠️ 파생 경고 (Collateral) | Variable 'n' is assigned a value that is never used. |

---
