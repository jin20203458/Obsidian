
> - **`DeleteWithNonVirtualDtorChecker`**: 이 체커는 **가상 소멸자가 없는 다형성 객체를 삭제**하는 경우를 탐지합니다. 부모 클래스 포인터를 통해 자식 클래스 객체를 삭제할 때, 부모 클래스의 소멸자가 가상(virtual)이 아니면 자식 클래스의 소멸자가 호출되지 않아 리소스 누수가 발생할 수 있습니다. 이 체커는 객체의 정적 타입(포인터의 타입)과 동적 타입(실제 할당된 객체의 타입)을 비교하여 이러한 위험한 상황을 경고합니다.

e.g 
```cpp
class Base {
public:
    virtual void func() {} // Base를 다형성 클래스로 만듦
    // 소멸자가 virtual이 아님
    ~Base() {}
};

class Derived : public Base {
public:
    ~Derived() {
        // 이 소멸자는 호출되지 않음
    }
};

void example() {
    Base* b = new Derived();
    delete b; // 경고: 가상 소멸자가 없는 다형성 객체 'Base'를 삭제합니다.
              // 자식 클래스 'Derived'의 소멸자가 호출되지 않습니다.
}
```



> - **`CXXArrayDeleteChecker`**: 이 체커는 **잘못된 타입의 포인터를 통해 배열을 삭제**하는 경우를 탐지합니다. 예를 들어, `Base* b = new Derived;`과 같이 할당된 배열을 `delete[] b;`로 삭제하면, 컴파일러는 `Base` 클래스의 크기를 기준으로 각 객체의 소멸자를 호출하려 합니다. 만약 `sizeof(Derived)`가 `sizeof(Base)`보다 크면, 메모리 주소 계산이 잘못되어 정의되지 않은 동작이 발생합니다. 이 체커는 이러한 패턴을 찾아 경고합니다.

e.g
```cpp
class Base {
public:
    // 소멸자가 virtual이라도 배열 삭제 문제는 여전히 발생
    virtual ~Base() {}
};

class Derived : public Base {
    int data[10]; // sizeof(Derived) > sizeof(Base)
};

void array_example() {
    Base* b_array = new Derived[5];
    delete[] b_array; // 경고: 'Derived' 타입의 배열을 부모 클래스 'Base'의 포인터로 삭제하는 것은
                      // 정의되지 않은 동작입니다.
}
```