
**CRTP(Curiously Recurring Template Pattern)**는 이름부터 좀 이상하죠? “기묘하게 되풀이되는 템플릿 패턴”이라는 뜻인데, C++에서 **정적 다형성(static polymorphism)**을 구현할 때 자주 쓰이는 강력한 기법이에요.

```cpp
template <typename Derived>
class Base 
{
public:
    void interface() 
    {
        // 파생 클래스의 구현을 호출
        static_cast<Derived*>(this)->implementation();
    }
};

class Derived : public Base<Derived> 
{
public:
    void implementation()
     {
        std::cout << "Derived implementation\n";
    }
};
```
- `Base` → 기본 클래스가 파생 클래스를 알고 있음
- `Derived`는 `Base`를 상속 → **자기 자신을 인자로 넘김**
- `interface()`는 `Derived`의 `implementation()`을 호출 → **정적 다형성**

---

🔍 왜 쓰나요?

|목적|설명|
|---|---|
|**정적 다형성**|가상 함수 없이도 파생 클래스의 메서드를 호출 가능|
|**성능 향상**|런타임 오버헤드(vtable, 가상 호출) 없이 컴파일 타임에 결정됨|
|**믹스인(Mixin)**|공통 기능을 템플릿 기반으로 재사용 가능|
|**정적 인터페이스**|파생 클래스가 특정 메서드를 구현하도록 강제 가능|

---

🧪 예시: 객체 수 세기

```cpp
template <typename T>
class ObjectCounter {
public:
    static int count;
    ObjectCounter() { ++count; }
    ~ObjectCounter() { --count; }
};

template <typename T>
int ObjectCounter<T>::count = 0;

class MyClass : public ObjectCounter<MyClass> {};
```
- `MyClass` 객체가 생성/소멸될 때마다 `count`가 증가/감소
- `ObjectCounter`는 `MyClass` 전용 카운터가 됨

---

⚠️ 주의할 점

- **복잡해 보일 수 있음**: 처음 보면 “왜 자기 자신을 인자로 넘기지?” 싶어요
- **템플릿 인스턴스가 많아지면 코드 부풀기(code bloat)** 가능성 있음
- **런타임 다형성은 불가능**: `Base*`로는 여러 타입을 담을 수 없음

---

✅ 요약

|특징|설명|
|---|---|
|구조|`class Derived : public Base`|
|장점|정적 다형성, 성능, 믹스인 구현|
|단점|복잡성, 코드 부풀기, 런타임 다형성 불가|

