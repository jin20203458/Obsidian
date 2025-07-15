
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

- **적발 근거:**  
    `checkOperandNegative(Right)`에서 검출.
    

---

### 3. **왼쪽 피연산자가 음수 (Negative Value Shifted)**

- **조건:**  
    C/C++에서 왼쪽 피연산자가 **signed(부호 있는) 타입**이면서 **음수**일 경우 C++20 이전에는 정의되지 않은 동작입니다.

```cpp
int x = -2;
int y = 3;
int z = x << y; // C++20 이전에는 UB, 이후에는 허용
```

**적발 근거:**  
`checkOperandNegative(Left)`에서 검출 (단, C++20이전, Pedantic 모드에서만 검사).

---

### 4. **왼쪽 시프트에서 비트 오버플로우 (Left Shift Overflow)**

- **조건:**  
    왼쪽 쉬프트(<<) 연산 시, 시프트 결과가 타입이 표현 가능한 범위를 넘어서면 **오버플로우**로 정의되지 않은 동작입니다.
    
    - 특히 부호 있는 타입에서 **부호 비트까지 밀려 올라가면 UB**
    - unsigned 타입은 이 조건을 따지지 않음.

```cpp
int x = 0x40000000; // 2^30
int y = 2;
int z = x << y; // 0x100000000, 부호 비트까지 밀려 UB!
```

**적발 근거:**  
`checkLeftShiftOverflow()`에서 검출.