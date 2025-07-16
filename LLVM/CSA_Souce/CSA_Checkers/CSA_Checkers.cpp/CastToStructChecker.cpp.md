[[AST-based Checking]]
이 체커의 **핵심 목적**은 포인터 캐스팅 시 발생할 수 있는 두 가지 주요 메모리 오용 패턴을 탐지하는 것입니다. 이 패턴들은 데이터 손상이나 메모리 접근 오류로 이어질 수 있는 심각한 버그의 원인이 됩니다. 이 체커는 LLVM 프로젝트의 일부로, Apache License v2.0 with LLVM Exceptions에 따라 라이선스가 부여됩니다[1](https://llvm.org/LICENSE.txt).

1. **구조체가 아닌 타입을 구조체로 캐스팅**하는 경우 (`char *` -> `struct T *`)
    
2. **작은 구조체를 더 큰 구조체로 캐스팅**하는 경우 (`struct Small *` -> `struct Large *`)


**이러한 검사는 CWE-588(비구조체 포인터의 자식 멤버에 접근 시도)과 같은 알려진 보안 취약점 범주에 해당합니다.**

```scss
코드: (struct User *)data_buffer
    |
    V
1. 캐스팅 발견 (`VisitCastExpr`)
    |
    V
2. 원본 및 목적지 타입 식별
   -> 원본 타입(`OrigTy`): char *
   -> 목적지 타입(`ToTy`): struct User *
    |
    V
3. 내부 타입 추출 (`getPointeeType`)
   -> 원본 내부 타입(`OrigPointeeTy`): char
   -> 목적지 내부 타입(`ToPointeeTy`): struct User
    |
    V
4. 핵심 검사 1: 목적지 타입이 구조체인가?
   -> `ToPointeeTy->isStructureOrClassType()` ?  -->  **TRUE**
    |
    V
5. 핵심 검사 2: 원본 타입이 구조체가 아닌가?
   -> `OrigPointeeTy->isRecordType()` ?  -->  **FALSE**
    |
    V
6. 경고 생성: `!isRecordType()` 조건이 참이 되므로,
   "Cast from non-struct type to struct type" 경고를 발생시킵니다.
```

## 이 체커가 탐지하는 두 가지 주요 오류 유형

`CastToStructChecker`는 소스 코드를 분석하여 다음 두 가지 시나리오를 찾아내고 경고를 발생시킵니다.

## 유형 1: 구조체가 아닌 타입을 구조체로 캐스팅 (Cast from non-struct)

가장 위험한 경우 중 하나는 단순히 바이트 덩어리를 가리키는 포인터(`char *` 등)를 특정 필드를 가진 구조체 포인터로 강제 변환하는 것입니다. 메모리 레이아웃, 정렬, 패딩 등이 전혀 맞지 않으므로 구조체의 필드에 접근하는 순간 정의되지 않은 동작(Undefined Behavior)이 발생합니다.

**오류 예시 (Bad Code)**
```cpp
#include <stdio.h>

struct User {
    long id;      // 8 bytes
    char name[16]; // 16 bytes
};

void process_data() {
    // 1. 임의의 데이터 버퍼를 생성합니다.
    char data_buffer[32];

    // 2. 구조체가 아닌 char*를 struct User*로 캐스팅합니다.
    //    이것이 체커가 탐지하는 지점입니다.
    struct User *user = (struct User *)data_buffer;

    // 3. 필드에 접근하면 메모리가 오염되거나 예상치 못한 값을 읽게 됩니다.
    user->id = 12345L; // *** BUG: Potential data corruption ***
}
```
**체커의 분석 과정:**

1. `(struct User *)data_buffer` 캐스팅을 발견합니다.
    
2. 원본 포인터 타입(`char *`)과 목적지 포인터 타입(`struct User *`)을 확인합니다.
    
3. 목적지 타입(`struct User`)은 구조체이지만, 원본이 가리키는 타입(`char`)은 구조체가 아님을 확인합니다.
    
4. `!OrigPointeeTy->isRecordType()` 조건이 참이 되어, **"Cast from non-struct type to struct type"** 이라는 경고를 생성합니다.
    

## 유형 2: 작은 구조체를 더 큰 구조체로 캐스팅 (Widening Cast)

한 구조체 타입의 변수를 더 많은 멤버를 가진(즉, 크기가 더 큰) 다른 구조체 타입으로 캐스팅하는 것도 위험합니다. 원래 할당된 메모리 크기를 넘어서는 필드에 접근하면 메모리 경계를 초과하게 됩니다.

**오류 예시 (Bad Code)**
```cpp
struct Point2D { // 8 바이트 크기
    int x;
    int y;
};

struct Point3D { // 12 바이트 크기
    int x;
    int y;
    int z; // Point2D에는 없는 멤버
};

void manipulate_points() {
    struct Point2D p2d;
    p2d.x = 10;
    p2d.y = 20;

    // 1. 작은 구조체(Point2D)의 주소를 더 큰 구조체(Point3D) 포인터로 캐스팅합니다.
    //    이것이 체커가 탐지하는 지점입니다.
    struct Point3D *p3d = (struct Point3D *)&p2d;

    // 2. Point3D에만 있는 'z' 필드에 접근하면 Point2D에 할당된 메모리 범위를 넘어섭니다.
    p3d->z = 30; // *** BUG: Out-of-bounds write ***
}
```
**체커의 분석 과정:**

1. `(struct Point3D *)&p2d` 캐스팅을 발견합니다.
    
2. 원본(`struct Point2D`)과 목적지(`struct Point3D`)가 모두 구조체 타입임을 확인합니다.
    
3. 두 구조체의 크기를 비교합니다: `sizeof(Point3D)` (12)가 `sizeof(Point2D)` (8)보다 큽니다.
    
4. `ToWidth > OrigWidth` 조건이 참이 되어, **"Widening cast to struct type"** 이라는 경고를 생성합니다.
    

## 체커가 허용하는 예외적인 경우 (정상 코드)

`CastToStructChecker`는 거짓 양성을 줄이기 위해 몇 가지 일반적인 코딩 관행을 예외로 처리합니다.

- **`void *`로부터의 캐스팅**: `malloc`과 같이 타입 정보가 없는 제네릭 포인터(`void *`)에서 특정 구조체 포인터로 캐스팅하는 것은 C에서 매우 흔하고 합법적인 패턴이므로 경고하지 않습니다.

```cpp
#include <stdlib.h>
struct User *u = malloc(sizeof(struct User)); // 반환 타입은 void*
struct User *user_ptr = (struct User *)u; // OK, 허용됨
```

**큰 구조체를 작은 구조체로 캐스팅 (Narrowing Cast)**: 더 큰 구조체를 작은 구조체로 캐스팅하는 것은 일부 필드 데이터가 손실될 수는 있지만, 메모리 경계를 초과하는 접근을 유발하지는 않으므로 체커는 이를 경고하지 않습니다. 이는 상속과 유사한 패턴에서 사용되기도 합니다.

```cpp
struct Point3D p3d;
// 큰 구조체를 작은 구조체로 캐스팅
struct Point2D *p2d = (struct Point2D *)&p3d; // OK, 경고 없음
p2d->x = 10; // 안전함
```
## 결론: 체커의 개요

`CastToStructChecker`는 다음과 같이 요약할 수 있습니다.

- **목표**: 구조체 포인터 캐스팅 시 발생하는 명백하고 위험한 두 가지 오용 사례를 탐지합니다.
    
- **핵심 로직**:
    
    1. 원본 타입이 구조체가 아닌데 구조체로 캐스팅하는지 확인합니다.
        
    2. 원본과 대상이 모두 구조체일 경우, 대상 구조체의 크기가 더 큰지(widening) 확인합니다.
        
- **의의**: 이 체커는 C/C++에서 타입 시스템을 우회하는 위험한 캐스팅을 방지하여 메모리 손상 및 예측 불가능한 동작을 사전에 막는 데 도움을 주는 실용적인 정적 분석 도구입니다. `CastSizeChecker`가 '배열' 관점의 크기 검사에 집중했다면, `CastToStructChecker`는 '구조체'의 필드 접근 관점에서 메모리 안전성을 검사합니다.