1) **동적할당된 변수를 release 할 때에는 변수의 type과 일치하게 release 하여야 한다.**

2) **지역 변수의 주소 값을 처리하는 handle을 return하지 말아야 한다.**

3) **함수 parameter의 주소 값을 처리하는 handle을 return하지 말아야 한다.**

4) **소멸자내에서 처리할 수 없는 예외 상황을 발생시키지 말아야 한다.**

5) **사용되지 않는 예외 처리 문을 작성하지 말아야 한다.**

6) exception specification에 기술되지 않은 모든 throw에 대하여 예외처리를 해야만 한다.

7) main 함수에서 처리되지 않는 throw를 작성하지 말아야 한다.

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

- 지역 변수의 **참조**(&)를 반환하는 것은 위험함
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


---

4) **소멸자내에서 처리할 수 없는 예외 상황을 발생시키지 말아야 한다.**

### ❌ 잘못된 예

```cpp
void funcThrowsEverything();

class C {
public:
    ~C();
private:
    bool b;
};

C::~C() {
    funcThrowsEverything(); // ❌ 예외 발생 가능
    if (b) {
        throw int();         // ❌ 예외 발생 → 소멸자에서 예외 전파
    }
}

C gc;  // 생성과 소멸 과정에서 프로그램 강제 종료 가능성
```
#### 🔍 문제점

- C++에서 **소멸자에서 던진 예외가 외부로 전파될 경우**, 그 순간 또 다른 예외가 발생하면 `std::terminate()` 호출됨 → 프로그램 강제 종료
- 소멸자는 **예외 안전성**이 중요하므로, **예외 발생을 반드시 내부에서 처리해야 함**
- 특히 복구 불가능한 상황일지라도 **try-catch**로 반드시 포착해야 함

### ✅ 바른 예

```cpp
void funcThrowsEverything();

class C {
public:
    ~C();
private:
    bool b;
};

C::~C() {
    try {
        funcThrowsEverything();
        if (b) {
            throw int();   // 예외 발생 가능
        }
    } catch (...) {
        // 예외는 무조건 잡고 종료해야 함
    }
}

C gc;  // ✅ 안전하게 소멸자 종료
```
#### 📌 설명

- **소멸자에서 예외를 던지는 것은 위험한 코드**이며, C++의 예외 처리 모델상 **catch되지 않으면 종료**됩니다.
- `catch(...)`를 통해 **모든 예외를 처리**하고, 필요 시 로그 남기고 조용히 종료하거나 재처리를 유도해야 함
- 특히 소멸자가 호출되는 상황은 대개 예외 처리 중이므로, **예외 중첩은 절대 피해야 함**


---

5) **사용되지 않는 예외 처리 문을 작성하지 말아야 한다.**

### ❌ 잘못된 예

```cpp
class E {};
void boo() throw(int, E);

void foo() throw() {
    try 
    {
        boo();
    } 
    catch (...) 
    {
        // 잡음
    } 
    catch (E& e) 
    {  // ❌ 위반: 도달하지 않음
        // 이 블록은 절대 실행되지 않음
    }
}
```
#### 🔍 문제점

- `catch(...)`는 모든 예외를 포괄적으로 잡기 때문에 **그 이후에 나오는 catch 블록은 절대 실행되지 않음**
    
- `catch(E& e)`는 절대 도달하지 않음 → **불필요한 코드**
    
- 컴파일은 되지만, **코드 가독성과 의도 파악에 혼란**을 줄 수 있음

### ✅ 바른 예

```cpp
class E {};
void boo() throw(int, E);

void foo() throw() {
    try {
        boo();
    } catch (E& e) {
        // 특정 예외만 처리
    } catch (...) {
        // 나머지 예외 포괄 처리
    }
}
```
#### 📌 설명

- **catch 블록은 순서가 중요**함
    
    - 구체적인 예외 → 먼저 처리
    - 포괄적 예외 (`catch(...)`) → 맨 마지막에 둠
        
- 사용되지 않을 `catch` 구문은 제거해야
    
    - **코드 간결성 확보**
    - **오해 방지**
    - **불필요한 리소스 낭비 방지**


---

6) **exception specification에 기술되지 않은 모든 throw에 대하여 예외처리를 해야만 한다.**

### ❌ 잘못된 예

```cpp
void foo(int a) throw(int) {
    if (a > 0) {
        throw int();     // ✅ int는 명시된 예외 → 가능
    } else {
        throw float();   // ❌ float은 명시되지 않은 예외 → 위반!
    }
}
```
#### 🔍 문제점

- `foo()` 함수는 `throw(int)`로 예외 명시 → **int 타입 외 예외는 허용되지 않음**
- `throw float();` → **예외 명세를 벗어남**
- C++에서는 명세 외의 예외 발생 시 `std::unexpected()` 호출됨
- `unexpected()`에서 적절히 처리하지 못하면 `std::terminate()` → **프로그램 강제 종료**

### ✅ 바른 예

```cpp
void foo(int a) throw(int) {
    try {
        throw float();     // float은 명세 외 → 직접 처리
    } catch (...) {
        // 모든 예외를 안전하게 처리
    }
}
```
#### 📌 설명

- 예외 명세(`throw(int)`)는 해당 함수가 던질 수 있는 **예외 타입을 제한**
- 명세에 없는 타입을 던질 경우 **함수 외부에서 catch할 수 없게 됨**
- 반드시 `try-catch`로 감싸서 **함수 내부에서 처리**해야 안전함
- 특히 사용자 정의 타입이 예외로 던져질 경우 `unexpected()`에서도 처리 불가능 → 종료 위험 증가


---

7) **main 함수에서 처리되지 않는 throw를 작성하지 말아야 한다.**

### ❌ 잘못된 예

```cpp
int main() {
    try {
        throw int();      // ✅ 예외 처리됨
    } catch (int) {
        // 처리 완료
    }

    throw int();          // ❌ 위반: 처리되지 않은 예외
}
```
#### 🔍 문제점

- 첫 번째 `throw int();`는 `try-catch`로 안전하게 처리됨
- 두 번째 `throw int();`는 **catch 없이 직접 호출됨**
- `main()`은 예외가 **끝까지 처리되지 않으면** → `std::terminate()` 호출
- 예외가 처리되지 않으면 시스템은 **프로그램을 강제 종료**

### ✅ 바른 예

```cpp
int main() {
    try {
        throw int();      // ✅ 예외 처리
    } catch (int) {
        // 예외 safely 처리됨
    }

    // 이후 throw 없음 → 안전
}
```
#### 📌 설명

- `main()` 함수는 C++에서 프로그램이 시작되는 **최종 루트 함수**
- 이 함수 내에서 **예외가 발생하면 반드시 처리**되어야 함
- `main()`에서 발생한 예외가 처리되지 않으면 **전역적으로 catch할 수 있는 곳이 없기 때문에** → 시스템이 `terminate()`를 호출해 프로그램을 종료
- 특히 **라이브러리 초기화, 리소스 설정 중에 예외가 나면 디버깅이 어려워짐**

---
