

### [Cpp-1] new/delete 짝 맞춤
- checkerName: CXXDeleteChecker
- category: C++-Specific, Memory
- severity: CRITICAL
- summary: "배열은 new[]/delete[]로, 단일 객체는 new/delete로 할당과 해제를 일치시켜야 합니다."
- violation: `int* p = new int[10]; delete p;`
- correction: `int* p = new int[10]; delete[] p;`

### [Cpp-2] 지역 변수 참조 반환 금지
- checkerName: StackAddrEscapeChecker
- category: C++-Specific, Dangling Reference
- severity: ERROR
- summary: "함수 내에서 소멸될 지역 변수의 주소나 참조를 반환해서는 안 됩니다."
- violation: `A& foo() { A a; return a; }`
- correction: `A foo() { A a; return a; }`

### [Cpp-3] 매개변수 참조 반환 금지
- checkerName: StackAddrEscapeChecker
- category: C++-Specific, Dangling Reference
- severity: ERROR
- summary: "값으로 전달받아 함수 내에서 소멸될 매개변수의 주소나 참조를 반환해서는 안 됩니다."
- violation: `A& foo(A a) { return a; }`
- correction: `A foo(A a) { return a; }`

### [Cpp-4] 소멸자에서 예외 전파 금지
- checkerName: NoThrowInDestructorCheck
- category: C++-Specific, Exception Safety
- severity: CRITICAL
- summary: "소멸자(destructor) 내에서 발생한 예외는 반드시 내부에서 처리하고 외부로 전파시키지 않아야 합니다."
- violation: `~MyClass() { throw 1; }`
- correction: `~MyClass() { try { throw 1; } catch(...) { /* log and absorb */ } }`

### [Cpp-5] 도달할 수 없는 catch 블록 금지
- checkerName: UnreachableCatchCheck
- category: C++-Specific, Exception Handling
- severity: WARNING
- summary: "모든 예외를 잡는 catch(...) 블록 뒤에는 다른 catch 블록을 작성하지 않습니다."
- violation: `try {...} catch(...) {...} catch(E& e) {...}`
- correction: `try {...} catch(E& e) {...} catch(...) {...}`

### [Cpp-6] 예외 명세 위반 금지
- checkerName: ExceptionSpecificationCheck
- category: C++-Specific, Exception Handling
- severity: MAJOR
- summary: "동적 예외 명세(throw(...))에 명시되지 않은 타입의 예외를 던지지 않습니다."
- violation: `void f() throw(int) { throw 1.0f; }`
- correction: |
  // noexcept 또는 내부 try-catch로 변경
  void f() noexcept { /* ... */ }

### [Cpp-7] main에서 처리되지 않는 예외 금지
- checkerName: UnhandledThrowInMainCheck
- category: C++-Specific, Exception Handling
- severity: CRITICAL
- summary: "main 함수에서 발생하는 모든 예외는 반드시 main 함수 내에서 처리되어야 합니다."
- violation: `int main() { throw 1; }`
- correction: `int main() { try { throw 1; } catch(...) { /* ... */ } }`


### [Cpp-8] 해제된 메모리 사용 금지 (Use-After-Free)
- checkerName: UseAfterFreeCheck
- category: C++-Specific, Memory
- severity: CRITICAL
- summary: "해제(delete)된 메모리 영역을 다시 사용(역참조)해서는 안 됩니다."
- violation: `int* i = new int; delete i; bar(*i);`
- correction: `delete i; i = nullptr;`

### [Cpp-9] 복사 연산 시 모든 멤버 복사
- checkerName: IncompleteCopyOperationCheck
- category: C++-Specific, Object Model
- severity: MAJOR
- summary: "복사 생성자 및 복사 대입 연산자는 클래스의 모든 멤버 변수를 적절하게 복사(특히 깊은 복사)해야 합니다."
- violation: `Example(const Example& other) : x(other.x) { /* y는 복사 안함 */ }`
- correction: `y = new int(*other.y); // 깊은 복사`

### [Cpp-10] C++ 동적 할당은 new 사용
- checkerName: CXXNewDeleteMismatchCheck
- category: C++-Specific, Memory
- severity: CRITICAL
- summary: "C++ 객체의 동적 할당은 생성자를 호출하는 new 연산자를 사용해야 하며, malloc을 사용하면 안 됩니다."
- violation: `C* p = (C*)malloc(sizeof(C));`
- correction: `C* p = new C;`

### [Cpp-11] 순수 가상 함수는 '= 0'으로 초기화
- checkerName: PureVirtualFunctionInitCheck
- category: C++-Specific, Polymorphism
- severity: ERROR
- summary: "순수 가상 함수는 반드시 '= 0'으로 초기화하여 선언해야 합니다."
- violation: `virtual void f() = 1;`
- correction: `virtual void f() = 0;`

### [Cpp-12] 순수 가상 함수에 virtual 키워드 필수
- checkerName: PureVirtualNeedsVirtualSpecifierCheck
- category: C++-Specific, Polymorphism
- severity: ERROR
- summary: "순수 가상 함수로 선언하려면 반드시 virtual 키워드를 명시해야 합니다."
- violation: `void foo() = 0;`
- correction: `virtual void foo() = 0;`

### [Cpp-13] 가상 상속 다운캐스팅은 dynamic_cast 사용
- checkerName: DynamicCastForVirtualInheritanceCheck
- category: C++-Specific, Type Casting
- severity: MAJOR
- summary: "가상 상속(virtual inheritance) 관계에서의 안전한 다운캐스팅은 반드시 dynamic_cast를 사용해야 합니다."
- violation: `dp = static_cast<Derived*>(bp);`
- correction: `dp = dynamic_cast<Derived*>(bp);`

### [Cpp-14] 생성자/소멸자 내 가상 함수 호출 주의
- checkerName: VirtualCallInCtorDtorCheck
- category: C++-Specific, Object Lifecycle
- severity: ERROR
- summary: "생성자나 소멸자 내에서 가상 함수를 호출하면 다형성이 동작하지 않으므로, 클래스 식별자를 명시하여 호출해야 합니다."
- violation: `B::B() { func(); }`
- correction: `B::B() { B::func(); }`

### [Cpp-15] 생성자/소멸자 내 dynamic_cast 사용 금지
- checkerName: DynamicCastInCtorDtorCheck
- category: C++-Specific, Object Lifecycle
- severity: ERROR
- summary: "객체가 완전히 생성/소멸되지 않은 생성자/소멸자 내에서는 런타임 타입 정보(RTTI)를 사용하는 dynamic_cast를 사용할 수 없습니다."
- violation: `B2() { dynamic_cast<B2*>(this); }`
- correction: "[translate:생성자/소멸자 외부에서 사용합니다.]"
