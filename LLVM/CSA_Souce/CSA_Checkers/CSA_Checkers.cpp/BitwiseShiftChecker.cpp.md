
>**BitwiseShiftChecker.cpp**는 LLVM 프로젝트의 일부로, C++ 코드에서 비트 시프트 연산(`<<`, `>>`)의 피연산자가 잘못된 경우(음수이거나 너무 큰 값일 때) 발생할 수 있는 정의되지 않은 동작(undefined behavior)을 분석하는 체크 도구입니다.

### 1. **오른쪽 피연산자가 비트폭 이상 (Overshift / Shift Count Too Large)**

- **조건:**  
    오른쪽 피연산자(shift count, 즉 몇 비트 이동할지 값)가 왼쪽 피연산자의 타입 비트 수보다 크거나 같으면 **정의되지 않은 동작**(Undefined Behavior)입니다.

```cpp
int x = 1;
int y = 33; // int는 32비트라면
int z = x << y; // UB!
```
- **적발 근거:**  
    `checkOvershift()`에서 검출.
    

---

### 2. **오른쪽 피연산자가 음수 (Negative Shift Count)**

- **조건:**  
    오른쪽 피연산자가 음수라면 역시 **정의되지 않은 동작**입니다.
    
```cpp
int x = 1;
int y = -3;
int z = x << y; // UB!
```