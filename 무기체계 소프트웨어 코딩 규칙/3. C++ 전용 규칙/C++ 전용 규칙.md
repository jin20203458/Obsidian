1) **동적할당된 변수를 release 할 때에는 변수의 type과 일치하게 release 하여야 한다.**

2) **지역 변수의 주소 값을 처리하는 handle을 return하지 말아야 한다.**

3) **함수 parameter의 주소 값을 처리하는 handle을 return하지 말아야 한다.**





---

1) **동적할당된 변수를 release 할 때에는 변수의 type과 일치하게 release 하여야 한다.**

### ❌ 잘못된 예

```cpp
void bar(int num)
{
    int* p1 = new int[num];
    int* p2 = new int;
    delete p1;     // ❌ 배열 할당 → 단일 delete 사용
    delete[] p2;   // ❌ 단일 할당 → 배열 delete 사용
}
```
#### 🔍 문제점

- `new int[num]`는 **배열**을 동적으로 할당하므로 `delete[]`로 해제해야 함
- `new int`는 단일 객체 할당 → `delete`로 해제해야 함
- `delete` / `delete[]` 구분하지 않으면 **정의되지 않은 동작** 발생 가능
- 특히 배열의 크기가 클수록 **메모리 누수** 또는 프로그램 오작동 가능성 증가

### ✅ 바른 예

```cpp
void bar(int num)
{
    int* p1 = new int[num];
    int* p2 = new int;
    delete[] p1;   // ✅ 배열 → 배열 delete
    delete p2;     // ✅ 단일 → 단일 delete
}
```
#### 📌 설명

- 동적할당 시 **할당 방식에 따라 해제 방식도 맞춰야 함**
    
    - 배열 → `delete[]`
    - 단일 객체 → `delete`
        
- 잘못된 해제는 **컴파일 경고 없이 치명적인 버그** 유발할 수 있음


---

2) **지역 변수의 주소 값을 처리하는 handle을 return하지 말아야 한다.**

### ❌ 잘못된 예

```cpp
class A {
public:
    void f();
};

A& foo() {
    A a;
    return a;  // ❌ 지역 변수의 참조를 반환 → 위험한 동작
}

void bar() {
    foo().f();  // ❌ 미정의된 동작 발생 가능
}
```

#### 🔍 문제점

- `foo()` 안의 `a`는 **지역 변수** → 함수가 끝나면 **소멸**
- `return a;`에서 참조(`A&`)를 반환하면 **소멸된 객체의 주소를 반환**하게 됨
- 결과적으로 **미정의 동작(Undefined Behavior)** 발생 가능
- 디버깅이 어렵고, 특정 환경에서는 **접근 위반**(crash)으로 이어질 수 있음

### ✅ 바른 예

```cpp
class A {
public:
    void f();
};

A foo() {
    A a;
    return a;  // ✅ 객체 복사 혹은 이동 → 안전함
}

void bar() {
    foo().f();  // ✅ 정상 동작
}
```

#### 📌 설명

- 지역 변수의 **참조(&)**를 반환하는 것은 위험함
- 객체 자체를 반환하면 **복사 또는 이동**으로 처리되어 메모리 문제가 없음
- Modern C++에서는 **복사 생략**(Return Value Optimization, RVO)을 통해 성능도 좋음


---

3) **함수 parameter의 주소 값을 처리하는 handle을 return하지 말아야 한다.**

### ❌ 잘못된 예

```cpp
class A {
public:
    void f();
};

A& foo(A a) {
    return a;  // ❌ 함수 parameter는 복사된 지역 객체 → 참조 반환은 위험
}

void bar() {
    A a;
    foo(a).f();  // ❌ 소멸된 객체 참조로 인해 미정의 동작 발생
}
```

#### 🔍 문제점

- `foo(A a)`의 매개변수 `a`는 **복사된 지역 객체**
- `return a;`에서 참조(`A&`)를 반환하면 **지역 객체의 주소**를 리턴하게 됨
- 해당 객체는 `foo()`가 종료되면 소멸 → **dangling reference**
- 이후 `.f()` 호출은 **미정의 동작(Undefined Behavior)** 발생 가능

### ✅ 바른 예

```cpp
class A {
public:
    void f();
};

A foo(A a) {
    return a;  // ✅ 객체 복사 또는 이동 → 안전한 반환
}

void bar() {
    A a;
    foo(a).f();  // ✅ 정상 동작
}
```

#### 📌 설명

- 지역 변수든 매개변수든 함수가 종료되면 **수명이 끝남**
- 참조로 반환하면 해당 메모리는 **유효하지 않음**
- 객체 자체를 값으로 반환하면 **복사 또는 이동 생성자**가 호출되어 안전하게 사용 가능