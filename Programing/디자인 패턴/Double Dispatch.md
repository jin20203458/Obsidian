**Double Dispatch**(더블 디스패치)는 함수 호출이 런타임에 두 객체의 실제 타입에 따라 결정되는 메커니즘입니다.  
C++에서는 기본적으로 **Single Dispatch**(수신 객체의 타입만으로 결정)만 지원하지만, [[Visitor Pattern]] 패턴 등으로 Double Dispatch를 구현할 수 있습니다.


## C++ Double Dispatch 예제


```cpp
#include <iostream>
using namespace std;

// 방문 대상 클래스 계층
class AnimalVisitor; // 전방 선언

class Animal
{
public:
    virtual void accept(AnimalVisitor* visitor) = 0;
};

class Cat;
class Dog;

// 방문자 클래스 계층
class AnimalVisitor
{
public:
    virtual void visit(Cat* c) = 0;
    virtual void visit(Dog* d) = 0;
};

class Cat : public Animal
{
public:
    void accept(AnimalVisitor* visitor) override
    {
        visitor->visit(this); // 두 번째 디스패치
    }
};

class Dog : public Animal
{
public:
    void accept(AnimalVisitor* visitor) override 
    {
        visitor->visit(this); // 두 번째 디스패치
    }
};

// 실제 동작 구현
class ReactVisitor : public AnimalVisitor 
{
public:
    void visit(Cat* c) override { cout << "고양이를 쓰다듬는다\n"; }
    void visit(Dog* d) override { cout << "개를 보고 도망간다\n"; }
};

int main() {
    Animal* animals[] = { new Cat(), new Dog() };
    ReactVisitor visitor;
    
    for (auto a : animals)
        a->accept(&visitor); // 첫 번째 디스패치
}
```
- **첫 번째 디스패치:** `a->accept(&visitor)`에서 Animal의 실제 타입(Cat/Dog)에 따라 accept가 호출됨
    
- **두 번째 디스패치:** `visitor->visit(this)`에서 Visitor의 visit 메서드가 Cat* 또는 Dog*에 따라 호출됨

---
## Single Dispatch vs Double Dispatch

|구분|Single Dispatch (단일 디스패치)|Double Dispatch (이중 디스패치)|
|---|---|---|
|기준|수신 객체의 타입|수신 객체 + 인자 객체의 타입|
|C++ 지원|기본 지원 (virtual 함수)|직접 구현 필요 (Visitor 패턴 등)|
|예시|`obj->method()`|`obj->accept(visitor)` + `visitor->visit(obj)`|
|활용|일반적인 다형성|Visitor 패턴, 충돌 처리, 이중 타입 분기 등|
