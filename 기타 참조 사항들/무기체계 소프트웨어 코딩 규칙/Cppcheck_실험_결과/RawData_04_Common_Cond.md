# Cppcheck 실험 검증 코드 및 로우데이터 - 공통 적용 (조건식)

본 문서는 DAPA 무기체계 소프트웨어 코딩 규칙 중 **공통 적용 (조건식)**에 속하는 규칙들의 실제 테스트 코드와 Cppcheck 정적 분석 결과(진단 경고 로우데이터)를 매핑한 자료입니다.

---

## 📌 Rule 20: float 자료형에서 동등성 비교연산을 수행하지 말아야 한다.

* **세부 조항**: 가
* **매핑 체커 (Clang-Tidy)**: `NoFloatEqualityCheck`
* **최종 진단 판정**: **❌ 미탐 (Silent / False Negative) - 아무런 경고도 감지되지 않음**

### 💻 검증 소스코드
> 파일 위치: `dapa_test_suite.cpp` (L313 ~ L323)

```cpp
 313 | namespace Rule_20 {
 314 |     int foo(float a, float b)
 315 |     {
 316 |         int r = 0;
 317 |         if (a == b)  // ❌ 부동소수점 비교는 신뢰할 수 없음
 318 |         {
 319 |             r = 1;
 320 |         }
 321 |         return r;
 322 |     }
 323 | }
```

### 🔍 Cppcheck 진단 경고 로그 (Raw Output)
> ❌ **검출된 경고 없음 (Silent)**

---

## 📌 Rule 21: 조건문의 결과가 항상 True 혹은 False라면 이는 조건문으로 작성해서는 안된다.

* **세부 조항**: 나
* **매핑 체커 (Clang-Tidy)**: `AlwaysConstantConditionCheck`
* **최종 진단 판정**: **✅ 정탐 (True Positive) - 의도된 결함 정확히 검출**

### 💻 검증 소스코드
> 파일 위치: `dapa_test_suite.cpp` (L329 ~ L349)

```cpp
 329 | namespace Rule_21 {
 330 |     void foo(unsigned char x, int y)
 331 |     {
 332 |         if (y < 20)
 333 |         {
 334 |             if (y < 30)  // ❌ 항상 참 (y < 20이면 당연히 y < 30)
 335 |             {
 336 |                 y = 30;
 337 |             }
 338 |             if (y > 30)  // ❌ 항상 거짓 (y < 20이면 y > 30일 수 없음)
 339 |             {
 340 |                 y = 30;
 341 |             }
 342 |         }
 343 | 
 344 |         if (x < 0)  // ❌ 항상 거짓 (unsigned char는 음수가 될 수 없음)
 345 |         {
 346 |             x = 0;
 347 |         }
 348 |     }
 349 | }
```

### 🔍 Cppcheck 진단 경고 로그 (Raw Output)
| 라인 (Line) | 중요도 (Severity) | 경고 ID (Warning ID) | 분류 | 진단 메시지 (Message) |
| :---: | :---: | :---: | :---: | :--- |
| 340 | style | `uselessAssignmentArg` | ⚠️ 파생 경고 (Collateral) | Assignment of function parameter has no effect outside the function. |
| 346 | style | `uselessAssignmentArg` | ⚠️ 파생 경고 (Collateral) | Assignment of function parameter has no effect outside the function. |
| 334 | style | `knownConditionTrueFalse` | ✅ 정탐 경고 (Genuine) | Condition 'y<30' is always true |
| 338 | style | `knownConditionTrueFalse` | ✅ 정탐 경고 (Genuine) | Condition 'y>30' is always false |
| 344 | style | `unsignedLessThanZero` | ✅ 정탐 경고 (Genuine) | Checking if unsigned expression 'x' is less than zero. |
| 340 | style | `unreadVariable` | ⚠️ 파생 경고 (Collateral) | Variable 'y' is assigned a value that is never used. |
| 346 | style | `unreadVariable` | ⚠️ 파생 경고 (Collateral) | Variable 'x' is assigned a value that is never used. |

---

## 📌 Rule 22: switch의 case의 조건을 만족할 수 없는 Label을 사용하지 않는다.

* **세부 조항**: 다
* **매핑 체커 (Clang-Tidy)**: `UnknownChecker`
* **최종 진단 판정**: **❌ 미탐 (Silent / False Negative) - 아무런 경고도 감지되지 않음**

### 💻 검증 소스코드
> 파일 위치: `dapa_test_suite.cpp` (L355 ~ L370)

```cpp
 355 | namespace Rule_22 {
 356 |     extern int x;
 357 | 
 358 |     void foo(unsigned int n)
 359 |     {
 360 |         switch (n)
 361 |         {
 362 |             case (unsigned int)-1:      // ❌ unsigned int는 음수가 될 수 없음 → dead code (cast to compile on C++)
 363 |                 ++x;
 364 |                 break;
 365 |             default:
 366 |                 --x;
 367 |                 break;
 368 |         }
 369 |     }
 370 | }
```

### 🔍 Cppcheck 진단 경고 로그 (Raw Output)
> ❌ **검출된 경고 없음 (Silent)**

---

## 📌 Rule 23: Switch 구문에서 Expression을 논리적 연산으로 사용하지 말아야 한다.

* **세부 조항**: 라
* **매핑 체커 (Clang-Tidy)**: `SwitchStyleCheck`
* **최종 진단 판정**: **❌ 미탐 (Silent / False Negative) - 아무런 경고도 감지되지 않음**

### 💻 검증 소스코드
> 파일 위치: `dapa_test_suite.cpp` (L376 ~ L391)

```cpp
 376 | namespace Rule_23 {
 377 |     extern int x;
 378 | 
 379 |     void foo(int option, int pick)
 380 |     {
 381 |         switch (option == pick)  // ❌ 논리 연산 결과를 switch에 사용
 382 |         {
 383 |             case 0:
 384 |                 ++x;
 385 |                 break;
 386 |             case 1:
 387 |                 x = 0;
 388 |                 break;
 389 |         }
 390 |     }
 391 | }
```

### 🔍 Cppcheck 진단 경고 로그 (Raw Output)
> ❌ **검출된 경고 없음 (Silent)**

---

## 📌 Rule 24: 수행되지 않는 소스코드를 작성하지 말아야 한다.

* **세부 조항**: 마
* **매핑 체커 (Clang-Tidy)**: `UnreachableCodeCheck`
* **최종 진단 판정**: **✅ 정탐 (True Positive) - 의도된 결함 정확히 검출**

### 💻 검증 소스코드
> 파일 위치: `dapa_test_suite.cpp` (L397 ~ L411)

```cpp
 397 | namespace Rule_24 {
 398 |     void foo(int n)
 399 |     {
 400 |         switch (n)
 401 |         {
 402 |             case 1:
 403 |                 --n;
 404 |                 break;
 405 |                 n = n + 1;  // ❌ 절대 실행되지 않음 (break 이후의 코드)
 406 |             default:
 407 |                 ++n;
 408 |                 break;
 409 |         }
 410 |     }
 411 | }
```

### 🔍 Cppcheck 진단 경고 로그 (Raw Output)
| 라인 (Line) | 중요도 (Severity) | 경고 ID (Warning ID) | 분류 | 진단 메시지 (Message) |
| :---: | :---: | :---: | :---: | :--- |
| 407 | style | `uselessAssignmentArg` | ⚠️ 파생 경고 (Collateral) | Assignment of function parameter has no effect outside the function. |
| 405 | style | `unreachableCode` | ✅ 정탐 경고 (Genuine) | Statements following 'break' will never be executed. |
| 403 | style | `unreadVariable` | ⚠️ 파생 경고 (Collateral) | Variable 'n' is assigned a value that is never used. |
| 407 | style | `unreadVariable` | ⚠️ 파생 경고 (Collateral) | Variable 'n' is assigned a value that is never used. |

---
