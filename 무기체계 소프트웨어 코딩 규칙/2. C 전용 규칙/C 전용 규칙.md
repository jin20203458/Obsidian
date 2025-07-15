
1) **Scanf의 Argument 에는 Object Value가 저장된 주소값이 입력되어야 한다.**

2) `#include` **구문에서 표준에 맞지 않는 Character set을 사용하지 않아야 한다.**

3) **malloc에서 사용하는 인자값은 assign될 변수 type의 정수배이어야 한다.**

4) **함수의 Argument type과 개수는 함수의 Prototype, 선언, 정의가 모두 같아야 한다.**

5) **구조체/배열의 초기화 시 default 초기화 값(0)을 제외하고, 구조에 맞게 ‘{}’를 사용하여 선언된 Size에 맞게 초기화 해야 한다.**

---

1) **Scanf의 Argument 에는 Object Value가 저장된 주소값이 입력되어야 한다.**

### ❌ 잘못된 예

```c
#include <stdio.h>

extern void foo(void)
{
    int n;
    scanf("%d", n);  // ❌ 위반: n은 int → 주소가 아님
}
```

#### 문제점

- `scanf`는 입력받은 값을 **메모리에 저장**해야 하므로, **변수의 주소**를 인자로 받아야 함
- `n`은 값(value)이고, `&n`은 주소(pointer)
- `scanf("%d", n)`은 **컴파일러 경고 또는 런타임 오류** 발생 가능

### ✅ 바른 예

```c
#include <stdio.h>

extern void foo(void)
{
    int n;
    scanf("%d", &n);  // ✅ &n은 int의 주소 → 올바른 포인터 전달
}
```

#### 설명

- `scanf`는 포인터를 통해 메모리에 값을 저장하므로, 항상 **주소값을 넘겨야 함**
- `%d`는 `int *`를 요구하므로 `&n`이 정확한 인자


---

2) `#include` **구문에서 표준에 맞지 않는 Character set을 사용하지 않아야 한다.**

### ❌ 잘못된 예

```c
#include "taemin's.h"               // ❌ 작은따옴표 (') 포함
#include "donghyun.h/*temporary*/"  // ❌ 주석 포함
#include "Jady$.h"                  // ❌ 특수문자 $ 포함
#include "June's-header.h"         // ❌ 작은따옴표 및 하이픈 포함
```

### 문제점

- 파일 이름에 **비표준 문자**(`'`, `$`, `#`, 공백, 주석 등)를 포함하면:
    
    - 일부 컴파일러 또는 운영체제에서 **파일을 찾지 못하거나**
    - **컴파일 오류** 또는 **예기치 않은 동작**이 발생할 수 있음
        
- 특히 Windows, Linux, macOS 등 **플랫폼 간 이식성 문제**가 생길 수 있음
- 주석을 포함한 문자열은 **문법적으로 잘못된 include 경로**로 해석됨

### ✅ 바른 예

```c
#include "kim_tae_min.h"  // ✅ 알파벳, 숫자, 밑줄(_)만 사용
```

- 안전한 문자만 사용: **영문자, 숫자, 밑줄(_)**
- **가독성도 좋고**, **이식성도 뛰어남**


---

3) **malloc에서 사용하는 인자값은 assign될 변수 type의 정수배이어야 한다.**

### ❌ 잘못된 예

```c
void foo(void)
{
    long *ptd;
    ptd = (long *) malloc(3);   // ❌ long 크기의 정수배 아님
    ptd = (long *) malloc(11);  // ❌ long 크기의 정수배 아님
    ptd = (long *) malloc(13);  // ❌ long 크기의 정수배 아님
}
```

### 문제점

- `long *ptd`는 **long 타입의 데이터를 저장할 메모리 공간**을 가리켜야 함
- 하지만 `malloc(3)`, `malloc(11)` 등은 `sizeof(long)`**의 정수배가 아님**
    
- 결과적으로:
    
    - **메모리 정렬**(alignment)이 깨질 수 있음
    - **경계 침범**, **정렬 오류**, **성능 저하**, 심지어 **크래시** 발생 가능
    - 일부 플랫폼에서는 **정렬되지 않은 접근이 금지되어 있음**

### ✅ 바른 예

```c
void foo(void)
{
    long *ptd;
    ptd = (long *) malloc(12);  // ✅ sizeof(long) * 3 (정수배)
}
```

- `sizeof(long)`이 4바이트인 시스템이라면 `malloc(12)`는 **3개의 long 저장 가능**
- **정렬 보장**, **안정성 확보**, **이식성 향상**


---

4) **함수의 Argument type과 개수는 함수의 Prototype, 선언, 정의가 모두 같아야 한다.**

### ❌ 잘못된 예

```c
extern int foo(unsigned char a);  // 선언: unsigned char 1개

void test(float a, unsigned char b)
{
    int r;
    r = foo(a);       // ❌ 타입 불일치: float → unsigned char
    r = foo(b, a);    // ❌ 개수 불일치: 인자 2개 전달
}
```

### 문제점

- `foo`는 `unsigned char` 하나를 받는 함수로 선언되었지만,
    
- 호출 시:
    
    - `foo(a)`는 `float` → `unsigned char`로 **암묵적 변환** (정확한 의미 손실 가능)
    - `foo(b, a)`는 **인자 개수 초과** → 컴파일러가 경고하거나 정의되지 않은 동작 발생
    
- 특히 C에서는 함수 선언이 없거나 잘못된 경우, **암묵적 선언**(implicit declaration)으로 인해 **런타임 오류**로 이어질 수 있음

### ✅ 바른 예

```c
extern int foo(unsigned char a);  // 선언

void test(unsigned char a)        // 호출부도 일치
{
    int r;
    r = foo(a);                   // ✅ 타입과 개수 모두 일치
}
```

- 함수 선언, 정의, 호출 모두 **unsigned char 하나**로 일치
- **타입 변환 없음**, **안정성 보장**, **이식성 향상**


---

5) **구조체/배열의 초기화 시 default 초기화 값(0)을 제외하고, 구조에 맞게 ‘{}’를 사용하여 선언된 Size에 맞게 초기화 해야 한다.**

### ❌ 잘못된 예

```c
struct s {
    int i;
    int j;
};

const struct s s1 = {8};               // ❌ j는 명시적으로 초기화되지 않음
int arr[10] = {10};                    // ❌ arr[0]만 10, 나머지는 0 → 의도 불명확
int arr1[3][2] = {1, 2, 3, 4, 5, 6};   // ❌ 중첩 배열인데 flat하게 초기화 → 가독성 저하
```

### 문제점

- `{8}`은 `i = 8`, `j = 0`으로 초기화되지만, **j가 명시되지 않아 오해 소지**
- `arr[10] = {10}`은 `arr[0] = 10`, 나머지는 0이지만 **전체 초기화인지 헷갈릴 수 있음**
- 2차원 배열을 flat하게 초기화하면 **구조가 눈에 안 보임**
- 일부 컴파일러나 정적 분석 도구는 **경고 또는 오류**를 발생시킬 수 있음

### ✅ 바른 예

```c
struct s {
    int i;
    int j;
};

const struct s s0 = {0};           // ✅ 전체 0 초기화
const struct s s1 = {8, 0};        // ✅ 명시적으로 i = 8, j = 0
const struct s s2 = {0, 0};        // ✅ 명시적으로 모두 0

int arr[10] = {0};                 // ✅ 전체 0 초기화
int arr1[3][2] = {                 // ✅ 구조에 맞게 중첩 초기화
    {1, 2},
    {3, 4},
    {5, 6}
};
```

### 장점

- **가독성 향상**: 어떤 값이 어디에 들어가는지 명확
- **이식성 보장**: 컴파일러마다 다르게 해석될 여지 없음
- **오류 예방**: 초기화 누락으로 인한 미정의 동작 방지


---