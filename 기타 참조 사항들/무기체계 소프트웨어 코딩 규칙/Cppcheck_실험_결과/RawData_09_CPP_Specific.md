# Cppcheck 실험 검증 코드 및 로우데이터 - C++ 전용 코딩 규칙

본 문서는 DAPA 무기체계 소프트웨어 코딩 규칙 중 **C++ 전용 코딩 규칙**에 속하는 규칙들의 실제 테스트 코드와 Cppcheck 정적 분석 결과(진단 경고 로우데이터)를 매핑한 자료입니다.

---

## 📌 Rule 52: 동적할당된 변수를 release 할 때에는 변수의 type과 일치하게 release 하여야 한다.

* **세부 조항**: 1
* **매핑 체커 (Clang-Tidy)**: `NewDeleteFormConsistencyCheck`
* **최종 진단 판정**: **✅ 정탐 (True Positive) - 의도된 결함 정확히 검출**

### 💻 검증 소스코드
> 파일 위치: `dapa_test_suite.cpp` (L749 ~ L757)

```cpp
 749 | namespace Rule_52 {
 750 |     void bar(int num)
 751 |     {
 752 |         int* p1 = new int[num];
 753 |         int* p2 = new int;
 754 |         delete p1;     // ❌ 배열 할당 → 단일 delete 사용
 755 |         delete[] p2;   // ❌ 단일 할당 → 배열 delete 사용
 756 |     }
 757 | }
```

### 🔍 Cppcheck 진단 경고 로그 (Raw Output)
| 라인 (Line) | 중요도 (Severity) | 경고 ID (Warning ID) | 분류 | 진단 메시지 (Message) |
| :---: | :---: | :---: | :---: | :--- |
| 754 | error | `mismatchAllocDealloc` | ✅ 정탐 경고 (Genuine) | Mismatching allocation and deallocation: p1 |
| 755 | error | `mismatchAllocDealloc` | ✅ 정탐 경고 (Genuine) | Mismatching allocation and deallocation: p2 |
| 754 | error | `uninitdata` | ⚠️ 파생 경고 (Collateral) | Memory is allocated but not initialized: p1 |
| 755 | error | `uninitdata` | ⚠️ 파생 경고 (Collateral) | Memory is allocated but not initialized: p2 |
| 752 | style | `unusedAllocatedMemory` | ⚠️ 파생 경고 (Collateral) | Variable 'p1' is allocated memory that is never used. |
| 753 | style | `unusedAllocatedMemory` | ⚠️ 파생 경고 (Collateral) | Variable 'p2' is allocated memory that is never used. |

---

## 📌 Rule 53: 지역 변수의 주소 값을 처리하는 handle을 return하지 말아야 한다.

* **세부 조항**: 2
* **매핑 체커 (Clang-Tidy)**: `ReturnStackAddressCheck`
* **최종 진단 판정**: **✅ 정탐 (True Positive) - 의도된 결함 정확히 검출**

### 💻 검증 소스코드
> 파일 위치: `dapa_test_suite.cpp` (L763 ~ L777)

```cpp
 763 | namespace Rule_53 {
 764 |     class A {
 765 |     public:
 766 |         void f();
 767 |     };
 768 | 
 769 |     A& foo() {
 770 |         A a;
 771 |         return a;  // ❌ 지역 변수의 참조를 반환 → 위험한 동작
 772 |     }
 773 | 
 774 |     void bar() {
 775 |         foo().f();  // ❌ 미정의된 동작 발생 가능
 776 |     }
 777 | }
```

### 🔍 Cppcheck 진단 경고 로그 (Raw Output)
| 라인 (Line) | 중요도 (Severity) | 경고 ID (Warning ID) | 분류 | 진단 메시지 (Message) |
| :---: | :---: | :---: | :---: | :--- |
| 771 | error | `returnReference` | ✅ 정탐 경고 (Genuine) | Reference to local variable returned. |

---

## 📌 Rule 54: 함수 parameter의 주소 값을 처리하는 handle을 return하지 말아야 한다.

* **세부 조항**: 3
* **매핑 체커 (Clang-Tidy)**: `ReturnStackAddressCheck`
* **최종 진단 판정**: **✅ 정탐 (True Positive) - 의도된 결함 정확히 검출**

### 💻 검증 소스코드
> 파일 위치: `dapa_test_suite.cpp` (L783 ~ L797)

```cpp
 783 | namespace Rule_54 {
 784 |     class A {
 785 |     public:
 786 |         void f();
 787 |     };
 788 | 
 789 |     A& foo(A a) {
 790 |         return a;  // ❌ 함수 parameter는 복사된 지역 객체 → 참조 반환은 위험
 791 |     }
 792 | 
 793 |     void bar() {
 794 |         A a;
 795 |         foo(a).f();  // ❌ 소멸된 객체 참조로 인해 미정의 동작 발생
 796 |     }
 797 | }
```

### 🔍 Cppcheck 진단 경고 로그 (Raw Output)
| 라인 (Line) | 중요도 (Severity) | 경고 ID (Warning ID) | 분류 | 진단 메시지 (Message) |
| :---: | :---: | :---: | :---: | :--- |
| 790 | error | `returnReference` | ✅ 정탐 경고 (Genuine) | Reference to local variable returned. |

---

## 📌 Rule 55: 소멸자내에서 처리할 수 없는 예외 상황을 발생시키지 말아야 한다.

* **세부 조항**: 4
* **매핑 체커 (Clang-Tidy)**: `DestructorExceptionSafetyCheck`
* **최종 진단 판정**: **✅ 정탐 (True Positive) - 의도된 결함 정확히 검출**

### 💻 검증 소스코드
> 파일 위치: `dapa_test_suite.cpp` (L803 ~ L821)

```cpp
 803 | namespace Rule_55 {
 804 |     void funcThrowsEverything();
 805 | 
 806 |     class C {
 807 |     public:
 808 |         ~C();
 809 |     private:
 810 |         bool b;
 811 |     };
 812 | 
 813 |     C::~C() {
 814 |         funcThrowsEverything(); // ❌ 예외 발생 가능
 815 |         if (b) {
 816 |             throw int();         // ❌ 예외 발생 → 소멸자에서 예외 전파
 817 |         }
 818 |     }
 819 | 
 820 |     C gc;  // 생성과 소멸 과정에서 프로그램 강제 종료 가능성
 821 | }
```

### 🔍 Cppcheck 진단 경고 로그 (Raw Output)
| 라인 (Line) | 중요도 (Severity) | 경고 ID (Warning ID) | 분류 | 진단 메시지 (Message) |
| :---: | :---: | :---: | :---: | :--- |
| 806 | style | `noConstructor` | ⚠️ 파생 경고 (Collateral) | The class 'C' does not declare a constructor although it has private member variables which likely require initialization. |
| 816 | warning | `exceptThrowInDestructor` | ✅ 정탐 경고 (Genuine) | Class C is not safe, destructor throws exception |
| 816 | error | `throwInNoexceptFunction` | ✅ 정탐 경고 (Genuine) | Unhandled exception thrown in function declared not to throw exceptions. |

---

## 📌 Rule 56: 사용되지 않는 예외 처리 문을 작성하지 말아야 한다.

* **세부 조항**: 5
* **매핑 체커 (Clang-Tidy)**: `UnknownChecker`
* **최종 진단 판정**: **✅ 정탐 (True Positive) - 의도된 결함 정확히 검출**

### 💻 검증 소스코드
> 파일 위치: `dapa_test_suite.cpp` (L827 ~ L845)

```cpp
 827 | namespace Rule_56 {
 828 |     class E {};
 829 |     void boo() throw(int, E);
 830 | 
 831 |     void foo() throw() {
 832 |         try
 833 |         {
 834 |             boo();
 835 |         }
 836 |         catch (...)
 837 |         {
 838 |             // 잡음
 839 |         }
 840 |         catch (E& e)
 841 |         {  // ❌ 위반: 도달하지 않음
 842 |             // 이 블록은 절대 실행되지 않음
 843 |         }
 844 |     }
 845 | }
```

### 🔍 Cppcheck 진단 경고 로그 (Raw Output)
| 라인 (Line) | 중요도 (Severity) | 경고 ID (Warning ID) | 분류 | 진단 메시지 (Message) |
| :---: | :---: | :---: | :---: | :--- |
| 840 | style | `constVariableReference` | ✅ 정탐 경고 (Genuine) | Variable 'e' can be declared as reference to const |

---

## 📌 Rule 57: exception specification에 기술되지 않은 모든 throw에 대하여 예외처리를 해야만 한다.

* **세부 조항**: 6
* **매핑 체커 (Clang-Tidy)**: `ExceptionSpecificationCheck`
* **최종 진단 판정**: **❌ 미탐 (Silent / False Negative) - 아무런 경고도 감지되지 않음**

### 💻 검증 소스코드
> 파일 위치: `dapa_test_suite.cpp` (L851 ~ L859)

```cpp
 851 | namespace Rule_57 {
 852 |     void foo(int a) throw(int) {
 853 |         if (a > 0) {
 854 |             throw int();     // ✅ int는 명시된 예외 → 가능
 855 |         } else {
 856 |             throw float();   // ❌ float은 명시되지 않은 예외 → 위반!
 857 |         }
 858 |     }
 859 | }
```

### 🔍 Cppcheck 진단 경고 로그 (Raw Output)
> ❌ **검출된 경고 없음 (Silent)**

---

## 📌 Rule 58: main 함수에서 처리되지 않는 throw를 작성하지 말아야 한다.

* **세부 조항**: 7
* **매핑 체커 (Clang-Tidy)**: `MainUnhandledThrowCheck`
* **최종 진단 판정**: **✅ 정탐 (True Positive) - 의도된 결함 정확히 검출**

### 💻 검증 소스코드
> 파일 위치: `dapa_test_suite.cpp` (L865 ~ L875)

```cpp
 865 | namespace Rule_58 {
 866 |     int main() {
 867 |         try {
 868 |             throw int();      // ✅ 예외 처리됨
 869 |         } catch (int) {
 870 |             // 처리 완료
 871 |         }
 872 | 
 873 |         throw int();          // ❌ 위반: 처리되지 않은 예외
 874 |     }
 875 | }
```

### 🔍 Cppcheck 진단 경고 로그 (Raw Output)
| 라인 (Line) | 중요도 (Severity) | 경고 ID (Warning ID) | 분류 | 진단 메시지 (Message) |
| :---: | :---: | :---: | :---: | :--- |
| 873 | error | `throwInEntryPoint` | ✅ 정탐 경고 (Genuine) | Unhandled exception thrown in function that is an entry point. |

---

## 📌 Rule 59: release 된 메모리 영역을 사용하지 말아야 한다.

* **세부 조항**: 8
* **매핑 체커 (Clang-Tidy)**: `UseAfterFreeCheck`
* **최종 진단 판정**: **✅ 정탐 (True Positive) - 의도된 결함 정확히 검출**

### 💻 검증 소스코드
> 파일 위치: `dapa_test_suite.cpp` (L881 ~ L891)

```cpp
 881 | namespace Rule_59 {
 882 |     void bar(int);
 883 |     void foo()
 884 |     {
 885 |         int* i = new int;
 886 |         *i = 1;
 887 |         bar(*i);
 888 |         delete i;
 889 |         bar(*i);  // ❌ 이미 해제된 메모리를 사용 → 위반
 890 |     }
 891 | }
```

### 🔍 Cppcheck 진단 경고 로그 (Raw Output)
| 라인 (Line) | 중요도 (Severity) | 경고 ID (Warning ID) | 분류 | 진단 메시지 (Message) |
| :---: | :---: | :---: | :---: | :--- |
| 889 | error | `deallocuse` | ✅ 정탐 경고 (Genuine) | Dereferencing 'i' after it is deallocated / released |

---

## 📌 Rule 60: copy operator를 통해서, 복사되지 않는 멤버 변수가 존재하지 말아야 한다.

* **세부 조항**: 9
* **매핑 체커 (Clang-Tidy)**: `PartialCopyAssignmentCheck`
* **최종 진단 판정**: **✅ 정탐 (True Positive) - 의도된 결함 정확히 검출**

### 💻 검증 소스코드
> 파일 위치: `dapa_test_suite.cpp` (L897 ~ L917)

```cpp
 897 | namespace Rule_60 {
 898 |     class Example {
 899 |     private:
 900 |         int x;
 901 |         int* y; // 동적 메모리
 902 | 
 903 |     public:
 904 |         Example(const Example& other)
 905 |             : x(other.x) {
 906 |             // ❌ y는 복사하지 않음 → 복사된 객체가 y를 공유하거나 쓰레기 값을 참조
 907 |         }
 908 | 
 909 |         Example& operator=(const Example& other) {
 910 |             if (this != &other) {
 911 |                 x = other.x;
 912 |                 // ❌ y는 복사하지 않음 → 동일한 문제 발생
 913 |             }
 914 |             return *this;
 915 |         }
 916 |     };
 917 | }
```

### 🔍 Cppcheck 진단 경고 로그 (Raw Output)
| 라인 (Line) | 중요도 (Severity) | 경고 ID (Warning ID) | 분류 | 진단 메시지 (Message) |
| :---: | :---: | :---: | :---: | :--- |
| 904 | warning | `uninitMemberVar` | ✅ 정탐 경고 (Genuine) | Member variable 'Example::y' is not initialized in the copy constructor. |
| 909 | warning | `operatorEqVarError` | ✅ 정탐 경고 (Genuine) | Member variable 'Example::y' is not assigned a value in 'Example::operator='. |
| 901 | style | `unusedStructMember` | ⚠️ 파생 경고 (Collateral) | class member 'Example::y' is never used. |

---

## 📌 Rule 61: C++에서 동적 메모리 할당은 반드시 `new` 연산자를 사용해야 한다.

* **세부 조항**: 10
* **매핑 체커 (Clang-Tidy)**: `NoMallocCheck`
* **최종 진단 판정**: **✅ 정탐 (True Positive) - 의도된 결함 정확히 검출**

### 💻 검증 소스코드
> 파일 위치: `dapa_test_suite.cpp` (L923 ~ L933)

```cpp
 923 | namespace Rule_61 {
 924 |     #include <cstdlib>
 925 | 
 926 |     class C {
 927 |     };
 928 | 
 929 |     void foo() {
 930 |         C* p = static_cast<C*>(std::malloc(sizeof(C)));  // ❌ 생성자 호출 안 됨
 931 |         delete p;  // ⚠️ new로 할당되지 않았는데 delete 사용 → 위반
 932 |     }
 933 | }
```

### 🔍 Cppcheck 진단 경고 로그 (Raw Output)
| 라인 (Line) | 중요도 (Severity) | 경고 ID (Warning ID) | 분류 | 진단 메시지 (Message) |
| :---: | :---: | :---: | :---: | :--- |
| 931 | error | `mismatchAllocDealloc` | ✅ 정탐 경고 (Genuine) | Mismatching allocation and deallocation: p |
| 931 | error | `uninitdata` | ⚠️ 파생 경고 (Collateral) | Memory is allocated but not initialized: p |
| 930 | style | `unusedAllocatedMemory` | ⚠️ 파생 경고 (Collateral) | Variable 'p' is allocated memory that is never used. |

---

## 📌 Rule 62: 순수 가상 함수는 반드시 `= 0` 으로 초기화해야 한다.

* **세부 조항**: 11
* **매핑 체커 (Clang-Tidy)**: `PureVirtualInitCheck`
* **최종 진단 판정**: **❌ 미탐 (Silent / False Negative) - 아무런 경고도 감지되지 않음**

### 💻 검증 소스코드
> 파일 위치: `dapa_test_suite.cpp` (L939 ~ L945)

```cpp
 939 | namespace Rule_62 {
 940 |     namespace {
 941 |         class A {
 942 |             virtual void f() = 1;  // ❌ 잘못된 초기화 → 쓰레기 값 가능
 943 |         };
 944 |     }
 945 | }
```

### 🔍 Cppcheck 진단 경고 로그 (Raw Output)
> ❌ **검출된 경고 없음 (Silent)**

---

## 📌 Rule 63: 순수 함수는 반드시 `virtual`로 선언되어야 한다.

* **세부 조항**: 12
* **매핑 체커 (Clang-Tidy)**: `VirtualPureCheck`
* **최종 진단 판정**: **⚠️ 미탐 (파생 경고만 발생) - 결함은 미탐지하고 부수적 경고만 발생**

### 💻 검증 소스코드
> 파일 위치: `dapa_test_suite.cpp` (L951 ~ L961)

```cpp
 951 | namespace Rule_63 {
 952 |     class Abstract {
 953 |     public:
 954 |         void foo() = 0;  // ❌ virtual 키워드 누락 → 순수 가상 함수로 인식 안 됨
 955 |     };
 956 | 
 957 |     class Derived : public Abstract {
 958 |     public:
 959 |         void foo() {}    // ⚠️ 재정의는 했지만 다형성이 깨질 수 있음
 960 |     };
 961 | }
```

### 🔍 Cppcheck 진단 경고 로그 (Raw Output)
| 라인 (Line) | 중요도 (Severity) | 경고 ID (Warning ID) | 분류 | 진단 메시지 (Message) |
| :---: | :---: | :---: | :---: | :--- |
| 959 | style | `functionStatic` | ⚠️ 파생 경고 (Collateral) | The member function 'Rule_63::Derived::foo' can be static. |
| 959 | warning | `duplInheritedMember` | ⚠️ 파생 경고 (Collateral) | The class 'Derived' defines member function with name 'foo' also defined in its parent class 'Abstract'. |

---

## 📌 Rule 64: virtual base 클래스 포인터를 derived 클래스 포인터로 변환할 때에는 반드시 `dynamic_cast`를 사용해야 한다.

* **세부 조항**: 13
* **매핑 체커 (Clang-Tidy)**: `VirtualBaseCastCheck`
* **최종 진단 판정**: **⚠️ 미탐 (파생 경고만 발생) - 결함은 미탐지하고 부수적 경고만 발생**

### 💻 검증 소스코드
> 파일 위치: `dapa_test_suite.cpp` (L967 ~ L985)

```cpp
 967 | namespace Rule_64 {
 968 |     class Base {
 969 |     public:
 970 |         virtual ~Base();
 971 |         virtual Derived* getAsDerived() = 0;
 972 |     };
 973 | 
 974 |     class Derived : virtual public Base {
 975 |     public:
 976 |         virtual Derived* getAsDerived() {
 977 |             return this;
 978 |         }
 979 |     };
 980 | 
 981 |     void foo(Base* bp, Derived* dp) {
 982 |         dp = static_cast<Derived*>(bp);       // ❌ 위반: 안전하지 않음
 983 |         dp = reinterpret_cast<Derived*>(bp);  // ❌ 위반: 위험한 캐스트
 984 |     }
 985 | }
```

### 🔍 Cppcheck 진단 경고 로그 (Raw Output)
| 라인 (Line) | 중요도 (Severity) | 경고 ID (Warning ID) | 분류 | 진단 메시지 (Message) |
| :---: | :---: | :---: | :---: | :--- |
| 983 | warning | `uselessAssignmentPtrArg` | ⚠️ 파생 경고 (Collateral) | Assignment of function parameter has no effect outside the function. Did you forget dereferencing it? |
| 976 | style | `missingOverride` | ⚠️ 파생 경고 (Collateral) | The function 'getAsDerived' overrides a function in a base class but is not marked with a 'override' specifier. |
| 983 | style | `redundantAssignment` | ⚠️ 파생 경고 (Collateral) | Variable 'dp' is reassigned a value before the old one has been used. |
| 983 | style | `unreadVariable` | ⚠️ 파생 경고 (Collateral) | Variable 'dp' is assigned a value that is never used. |

---

## 📌 Rule 65: 생성자/소멸자 내에서는 가상함수를 반드시 식별자(Qualifier)를 붙여서 호출해야 한다.

* **세부 조항**: 14
* **매핑 체커 (Clang-Tidy)**: `DynamicCastInCtorDtorCheck`
* **최종 진단 판정**: **✅ 정탐 (True Positive) - 의도된 결함 정확히 검출**

### 💻 검증 소스코드
> 파일 위치: `dapa_test_suite.cpp` (L991 ~ L1015)

```cpp
 991 | namespace Rule_65 {
 992 |     class B {
 993 |     public:
 994 |         virtual ~B() = 0;
 995 |         virtual void func();
 996 |     protected:
 997 |         B();
 998 |         B(const B&);
 999 |         B& operator=(const B&);
1000 |     };
1001 | 
1002 |     class D : public B {
1003 |     public:
1004 |         D() : B() {}
1005 |         virtual ~D();
1006 |         virtual void func();
1007 |     private:
1008 |         D(const D&);
1009 |         D& operator=(const D&);
1010 |     };
1011 | 
1012 |     B::B() {
1013 |         func();  // ❌ 위반: 식별자 없이 가상 함수 호출
1014 |     }
1015 | }
```

### 🔍 Cppcheck 진단 경고 로그 (Raw Output)
| 라인 (Line) | 중요도 (Severity) | 경고 ID (Warning ID) | 분류 | 진단 메시지 (Message) |
| :---: | :---: | :---: | :---: | :--- |
| 995 | style | `virtualCallInConstructor` | ✅ 정탐 경고 (Genuine) | Virtual function 'func' is called from constructor 'B()' at line 1013. Dynamic binding is not used. |
| 1005 | style | `missingOverride` | ⚠️ 파생 경고 (Collateral) | The destructor '~D' overrides a destructor in a base class but is not marked with a 'override' specifier. |
| 1006 | style | `missingOverride` | ⚠️ 파생 경고 (Collateral) | The function 'func' overrides a function in a base class but is not marked with a 'override' specifier. |

---

## 📌 Rule 66: 생성자 또는 소멸자에서는 `dynamic_cast`를 사용해서는 안 된다.

* **세부 조항**: 15
* **매핑 체커 (Clang-Tidy)**: `DynamicCastInCtorDtorCheck`
* **최종 진단 판정**: **✅ 정탐 (True Positive) - 의도된 결함 정확히 검출**

### 💻 검증 소스코드
> 파일 위치: `dapa_test_suite.cpp` (L1021 ~ L1033)

```cpp
1021 | namespace Rule_66 {
1022 |     class B2 {
1023 |     public:
1024 |         virtual ~B2();
1025 |         virtual void foo();
1026 | 
1027 |         B2() {
1028 |             typeid(B2);                      // ❌ 런타임 타입 정보 사용 → 위반
1029 |             foo();                           // ❌ 가상 함수 직접 호출 → 위반
1030 |             dynamic_cast<B2*>(this);         // ❌ dynamic_cast 사용 → 위반
1031 |         }
1032 |     };
1033 | }
```

### 🔍 Cppcheck 진단 경고 로그 (Raw Output)
| 라인 (Line) | 중요도 (Severity) | 경고 ID (Warning ID) | 분류 | 진단 메시지 (Message) |
| :---: | :---: | :---: | :---: | :--- |
| 1025 | style | `virtualCallInConstructor` | ✅ 정탐 경고 (Genuine) | Virtual function 'foo' is called from constructor 'B2()' at line 1029. Dynamic binding is not used. |
| 1028 | warning | `constStatement` | ⚠️ 파생 경고 (Collateral) | Redundant code: Found unused 'typeid' expression. |

---
