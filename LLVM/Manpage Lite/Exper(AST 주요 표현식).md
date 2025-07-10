

## 요약 표

|노드 이름|역할/예시|
|---|---|
|DeclRefExpr|변수/함수 참조 (`x`, `foo`)|
|IntegerLiteral|정수 상수 (`42`)|
|FloatingLiteral|실수 상수 (`3.14`)|
|CallExpr|함수 호출 (`foo(1, 2)`)|
|ArraySubscriptExpr|배열 인덱싱 (`arr`)|
|BinaryOperator|이항 연산 (`x + y`, `a == b`)|
|UnaryOperator|단항 연산 (`-x`, `!flag`)|
|MemberExpr|멤버 접근 (`obj.value`)|
|ImplicitCastExpr|암시적 형변환|
|CStyleCastExpr|명시적 C 스타일 형변환|

---

## 1. DeclRefExpr (선언 참조 표현식)

- **설명:** 변수, 상수, 함수 등 **이름이 붙은 선언**을 참조할 때 사용됩니다.
- **예시:**
```cpp
int x = 10;
int y = x + 1; // 여기서 x가 DeclRefExpr
```

---

## 2. IntegerLiteral / FloatingLiteral (정수/실수 리터럴)

- **설명:** 코드 내에 직접 등장하는 **숫자 상수**를 나타냅니다.
- **예시:**
```cpp
int a = 42;      // 42가 IntegerLiteral
double d = 3.14; // 3.14가 FloatingLiteral
```

---

## 3. CallExpr (함수 호출 표현식)

- **설명:** **함수 호출**을 나타냅니다. 함수 이름, 인자 목록 등이 하위 노드로 포함됩니다.
- **예시:**
```cpp
foo(1, 2); // CallExpr, 함수명 foo와 인자 1, 2
```

---

## 4. ArraySubscriptExpr (배열 인덱싱 표현식)

- **설명:** 배열이나 포인터에 **[] 연산자**로 접근하는 부분을 나타냅니다.
- **예시:**
```cpp
arr[3] = 5; // arr[3]이 ArraySubscriptExpr
```

---

## 5. BinaryOperator (이항 연산자)

- **설명:** *_+, -, _, /, ==, !=  등 두 피연산자를 가지는 연산을 모두 표현합니다.
- **예시:**
```cpp
int z = x + y; // x + y가 BinaryOperator
```

---

## 6. UnaryOperator (단항 연산자)

- **설명:** **++, --, !, ~, -** 등 한 개의 피연산자를 가지는 연산을 표현합니다.
- **예시:**
```cpp
int a = -x; // -x가 UnaryOperator
```

---

## 7. MemberExpr (멤버 접근 표현식)

- **설명:** 구조체/클래스의 **멤버 접근**(`.` 또는 `->`)을 나타냅니다. 
- **예시:**
```cpp
obj.value;    // obj.value가 MemberExpr
ptr->member;  // ptr->member도 MemberExpr
```

---

## 8. ImplicitCastExpr / CStyleCastExpr (암시적/명시적 형변환)

- **설명:** 타입 변환이 일어나는 부분을 명시적으로 표현합니다.
- **예시:**
```cpp
double d = 1;       // 1이 double로 암시적 변환(ImplicitCastExpr)
int x = (int)3.14;  // (int)3.14가 CStyleCastExpr
```

---

