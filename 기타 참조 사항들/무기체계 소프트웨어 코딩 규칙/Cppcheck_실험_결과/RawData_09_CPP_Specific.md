# Cppcheck 실험 검증 코드 및 로우데이터 - C++ 전용 코딩 규칙

본 문서는 DAPA 무기체계 소프트웨어 코딩 규칙 중 **C++ 전용 코딩 규칙**에 속하는 규칙들의 실제 테스트 코드와 Cppcheck 정적 분석 결과(진단 경고 로우데이터)를 매핑한 자료입니다.

---

## 📌 Rule 52: 동적할당된 변수를 release 할 때에는 변수의 type과 일치하게 release 하여야 한다.

* **세부 조항**: 1
* **매핑 체커 (Clang-Tidy)**: `NewDeleteFormConsistencyCheck`
* **최종 진단 판정**: **✅ 정탐 (True Positive) - 의도된 결함 정확히 검출**

### 💻 검증 소스코드
> 파일 위치: `dapa_test_suite.cpp` (L787 ~ L795)

```cpp
 787 | namespace Rule_52 {
 788 |     void bar(int num)
 789 |     {
 790 |         int* p1 = new int[num];
 791 |         int* p2 = new int;
 792 |         delete p1;     // ❌ 배열 할당 → 단일 delete 사용
 793 |         delete[] p2;   // ❌ 단일 할당 → 배열 delete 사용
 794 |     }
 795 | }
```

### 🔍 Cppcheck 진단 경고 로그 (Raw Output)
| 라인 (Line) | 중요도 (Severity) | 경고 ID (Warning ID) | 분류 | 진단 메시지 (Message) |
| :---: | :---: | :---: | :---: | :--- |
| 792 | error | `mismatchAllocDealloc` | ✅ 정탐 경고 (Genuine) | Mismatching allocation and deallocation: p1 |
| 793 | error | `mismatchAllocDealloc` | ✅ 정탐 경고 (Genuine) | Mismatching allocation and deallocation: p2 |
| 792 | error | `uninitdata` | ⚠️ 파생 경고 (Collateral) | Memory is allocated but not initialized: p1 |
| 793 | error | `uninitdata` | ⚠️ 파생 경고 (Collateral) | Memory is allocated but not initialized: p2 |
| 790 | style | `unusedAllocatedMemory` | ⚠️ 파생 경고 (Collateral) | Variable 'p1' is allocated memory that is never used. |
| 791 | style | `unusedAllocatedMemory` | ⚠️ 파생 경고 (Collateral) | Variable 'p2' is allocated memory that is never used. |

---

## 📌 Rule 53: 지역 변수의 주소 값을 처리하는 handle을 return하지 말아야 한다.

* **세부 조항**: 2
* **매핑 체커 (Clang-Tidy)**: `ReturnStackAddressCheck`
* **최종 진단 판정**: **✅ 정탐 (True Positive) - 의도된 결함 정확히 검출**

### 💻 검증 소스코드
> 파일 위치: `dapa_test_suite.cpp` (L801 ~ L815)

```cpp
 801 | namespace Rule_53 {
 802 |     class A {
 803 |     public:
 804 |         void f();
 805 |     };
 806 | 
 807 |     A& foo() {
 808 |         A a;
 809 |         return a;  // ❌ 지역 변수의 참조를 반환 → 위험한 동작
 810 |     }
 811 | 
 812 |     void bar() {
 813 |         foo().f();  // ❌ 미정의된 동작 발생 가능
 814 |     }
 815 | }
```

### 🔍 Cppcheck 진단 경고 로그 (Raw Output)
| 라인 (Line) | 중요도 (Severity) | 경고 ID (Warning ID) | 분류 | 진단 메시지 (Message) |
| :---: | :---: | :---: | :---: | :--- |
| 809 | error | `returnReference` | ✅ 정탐 경고 (Genuine) | Reference to local variable returned. |

---

## 📌 Rule 54: 함수 parameter의 주소 값을 처리하는 handle을 return하지 말아야 한다.

* **세부 조항**: 3
* **매핑 체커 (Clang-Tidy)**: `ReturnStackAddressCheck`
* **최종 진단 판정**: **✅ 정탐 (True Positive) - 의도된 결함 정확히 검출**

### 💻 검증 소스코드
> 파일 위치: `dapa_test_suite.cpp` (L821 ~ L835)

```cpp
 821 | namespace Rule_54 {
 822 |     class A {
 823 |     public:
 824 |         void f();
 825 |     };
 826 | 
 827 |     A& foo(A a) {
 828 |         return a;  // ❌ 함수 parameter는 복사된 지역 객체 → 참조 반환은 위험
 829 |     }
 830 | 
 831 |     void bar() {
 832 |         A a;
 833 |         foo(a).f();  // ❌ 소멸된 객체 참조로 인해 미정의 동작 발생
 834 |     }
 835 | }
```

### 🔍 Cppcheck 진단 경고 로그 (Raw Output)
| 라인 (Line) | 중요도 (Severity) | 경고 ID (Warning ID) | 분류 | 진단 메시지 (Message) |
| :---: | :---: | :---: | :---: | :--- |
| 828 | error | `returnReference` | ✅ 정탐 경고 (Genuine) | Reference to local variable returned. |

---

## 📌 Rule 55: 소멸자내에서 처리할 수 없는 예외 상황을 발생시키지 말아야 한다.

* **세부 조항**: 4
* **매핑 체커 (Clang-Tidy)**: `DestructorExceptionSafetyCheck`
* **최종 진단 판정**: **✅ 정탐 (True Positive) - 의도된 결함 정확히 검출**

### 💻 검증 소스코드
> 파일 위치: `dapa_test_suite.cpp` (L841 ~ L859)

```cpp
 841 | namespace Rule_55 {
 842 |     void funcThrowsEverything();
 843 | 
 844 |     class C {
 845 |     public:
 846 |         ~C();
 847 |     private:
 848 |         bool b;
 849 |     };
 850 | 
 851 |     C::~C() {
 852 |         funcThrowsEverything(); // ❌ 예외 발생 가능
 853 |         if (b) {
 854 |             throw int();         // ❌ 예외 발생 → 소멸자에서 예외 전파
 855 |         }
 856 |     }
 857 | 
 858 |     C gc;  // 생성과 소멸 과정에서 프로그램 강제 종료 가능성
 859 | }
```

### 🔍 Cppcheck 진단 경고 로그 (Raw Output)
| 라인 (Line) | 중요도 (Severity) | 경고 ID (Warning ID) | 분류 | 진단 메시지 (Message) |
| :---: | :---: | :---: | :---: | :--- |
| 844 | style | `noConstructor` | ⚠️ 파생 경고 (Collateral) | The class 'C' does not declare a constructor although it has private member variables which likely require initialization. |
| 854 | warning | `exceptThrowInDestructor` | ✅ 정탐 경고 (Genuine) | Class C is not safe, destructor throws exception |
| 854 | error | `throwInNoexceptFunction` | ✅ 정탐 경고 (Genuine) | Unhandled exception thrown in function declared not to throw exceptions. |

---

## 📌 Rule 56: 사용되지 않는 예외 처리 문을 작성하지 말아야 한다.

* **세부 조항**: 5
* **매핑 체커 (Clang-Tidy)**: `UnknownChecker`
* **최종 진단 판정**: **❌ 미탐 (Silent / False Negative) - 아무런 경고도 감지되지 않음**

### 💻 검증 소스코드
> 파일 위치: `dapa_test_suite.cpp` (L865 ~ L885)

```cpp
 865 | namespace Rule_56 {
 866 |     class E {};
 867 |     void boo() throw(int, E);
 868 | 
 869 |     void foo() throw() {
 870 |         try
 871 |         {
 872 |             boo();
 873 |         }
 874 |         catch (...)
 875 |         {
 876 |             // 잡음
 877 |         }
 878 | #if 0
 879 |         catch (E& e)
 880 |         {  // ❌ 위반: 도달하지 않음
 881 |             // 이 블록은 절대 실행되지 않음
 882 |         }
 883 | #endif
 884 |     }
 885 | }
```

### 🔍 Cppcheck 진단 경고 로그 (Raw Output)
> ❌ **검출된 경고 없음 (Silent)**

---

## 📌 Rule 57: exception specification에 기술되지 않은 모든 throw에 대하여 예외처리를 해야만 한다.

* **세부 조항**: 6
* **매핑 체커 (Clang-Tidy)**: `ExceptionSpecificationCheck`
* **최종 진단 판정**: **❌ 미탐 (Silent / False Negative) - 아무런 경고도 감지되지 않음**

### 💻 검증 소스코드
> 파일 위치: `dapa_test_suite.cpp` (L891 ~ L899)

```cpp
 891 | namespace Rule_57 {
 892 |     void foo(int a) throw(int) {
 893 |         if (a > 0) {
 894 |             throw int();     // ✅ int는 명시된 예외 → 가능
 895 |         } else {
 896 |             throw float();   // ❌ float은 명시되지 않은 예외 → 위반!
 897 |         }
 898 |     }
 899 | }
```

### 🔍 Cppcheck 진단 경고 로그 (Raw Output)
> ❌ **검출된 경고 없음 (Silent)**

---

## 📌 Rule 58: main 함수에서 처리되지 않는 throw를 작성하지 말아야 한다.

* **세부 조항**: 7
* **매핑 체커 (Clang-Tidy)**: `MainUnhandledThrowCheck`
* **최종 진단 판정**: **✅ 정탐 (True Positive) - 의도된 결함 정확히 검출**

### 💻 검증 소스코드
> 파일 위치: `dapa_test_suite.cpp` (L905 ~ L916)

```cpp
 905 | namespace Rule_58 {
 906 |     int main() {
 907 |         try {
 908 |             throw int();      // ✅ 예외 처리됨
 909 |         } catch (int) {
 910 |             // 처리 완료
 911 |         }
 912 | 
 913 |         throw int();          // ❌ 위반: 처리되지 않은 예외
 914 |         return 0;
 915 |     }
 916 | }
```

### 🔍 Cppcheck 진단 경고 로그 (Raw Output)
| 라인 (Line) | 중요도 (Severity) | 경고 ID (Warning ID) | 분류 | 진단 메시지 (Message) |
| :---: | :---: | :---: | :---: | :--- |
| 913 | error | `throwInEntryPoint` | ✅ 정탐 경고 (Genuine) | Unhandled exception thrown in function that is an entry point. |
| 914 | style | `duplicateBreak` | ✅ 정탐 경고 (Genuine) | Consecutive return, break, continue, goto or throw statements are unnecessary. |

---

## 📌 Rule 59: release 된 메모리 영역을 사용하지 말아야 한다.

* **세부 조항**: 8
* **매핑 체커 (Clang-Tidy)**: `UseAfterFreeCheck`
* **최종 진단 판정**: **✅ 정탐 (True Positive) - 의도된 결함 정확히 검출**

### 💻 검증 소스코드
> 파일 위치: `dapa_test_suite.cpp` (L922 ~ L932)

```cpp
 922 | namespace Rule_59 {
 923 |     void bar(int);
 924 |     void foo()
 925 |     {
 926 |         int* i = new int;
 927 |         *i = 1;
 928 |         bar(*i);
 929 |         delete i;
 930 |         bar(*i);  // ❌ 이미 해제된 메모리를 사용 → 위반
 931 |     }
 932 | }
```

### 🔍 Cppcheck 진단 경고 로그 (Raw Output)
| 라인 (Line) | 중요도 (Severity) | 경고 ID (Warning ID) | 분류 | 진단 메시지 (Message) |
| :---: | :---: | :---: | :---: | :--- |
| 930 | error | `deallocuse` | ✅ 정탐 경고 (Genuine) | Dereferencing 'i' after it is deallocated / released |

---

## 📌 Rule 60: copy operator를 통해서, 복사되지 않는 멤버 변수가 존재하지 말아야 한다.

* **세부 조항**: 9
* **매핑 체커 (Clang-Tidy)**: `PartialCopyAssignmentCheck`
* **최종 진단 판정**: **✅ 정탐 (True Positive) - 의도된 결함 정확히 검출**

### 💻 검증 소스코드
> 파일 위치: `dapa_test_suite.cpp` (L938 ~ L958)

```cpp
 938 | namespace Rule_60 {
 939 |     class Example {
 940 |     private:
 941 |         int x;
 942 |         int* y; // 동적 메모리
 943 | 
 944 |     public:
 945 |         Example(const Example& other)
 946 |             : x(other.x) {
 947 |             // ❌ y는 복사하지 않음 → 복사된 객체가 y를 공유하거나 쓰레기 값을 참조
 948 |         }
 949 | 
 950 |         Example& operator=(const Example& other) {
 951 |             if (this != &other) {
 952 |                 x = other.x;
 953 |                 // ❌ y는 복사하지 않음 → 동일한 문제 발생
 954 |             }
 955 |             return *this;
 956 |         }
 957 |     };
 958 | }
```

### 🔍 Cppcheck 진단 경고 로그 (Raw Output)
| 라인 (Line) | 중요도 (Severity) | 경고 ID (Warning ID) | 분류 | 진단 메시지 (Message) |
| :---: | :---: | :---: | :---: | :--- |
| 945 | warning | `uninitMemberVar` | ✅ 정탐 경고 (Genuine) | Member variable 'Example::y' is not initialized in the copy constructor. |
| 950 | warning | `operatorEqVarError` | ✅ 정탐 경고 (Genuine) | Member variable 'Example::y' is not assigned a value in 'Example::operator='. |
| 942 | style | `unusedStructMember` | ⚠️ 파생 경고 (Collateral) | class member 'Example::y' is never used. |

---

## 📌 Rule 61: C++에서 동적 메모리 할당은 반드시 `new` 연산자를 사용해야 한다.

* **세부 조항**: 10
* **매핑 체커 (Clang-Tidy)**: `NoMallocCheck`
* **최종 진단 판정**: **✅ 정탐 (True Positive) - 의도된 결함 정확히 검출**

### 💻 검증 소스코드
> 파일 위치: `dapa_test_suite.cpp` (L964 ~ L974)

```cpp
 964 | namespace Rule_61 {
 965 |     #include <cstdlib>
 966 | 
 967 |     class C {
 968 |     };
 969 | 
 970 |     void foo() {
 971 |         C* p = static_cast<C*>(std::malloc(sizeof(C)));  // ❌ 생성자 호출 안 됨
 972 |         delete p;  // ⚠️ new로 할당되지 않았는데 delete 사용 → 위반
 973 |     }
 974 | }
```

### 🔍 Cppcheck 진단 경고 로그 (Raw Output)
| 라인 (Line) | 중요도 (Severity) | 경고 ID (Warning ID) | 분류 | 진단 메시지 (Message) |
| :---: | :---: | :---: | :---: | :--- |
| 972 | error | `mismatchAllocDealloc` | ✅ 정탐 경고 (Genuine) | Mismatching allocation and deallocation: p |
| 972 | error | `uninitdata` | ⚠️ 파생 경고 (Collateral) | Memory is allocated but not initialized: p |
| 971 | style | `unusedAllocatedMemory` | ⚠️ 파생 경고 (Collateral) | Variable 'p' is allocated memory that is never used. |

---

## 📌 Rule 62: 순수 가상 함수는 반드시 `= 0` 으로 초기화해야 한다.

* **세부 조항**: 11
* **매핑 체커 (Clang-Tidy)**: `PureVirtualInitCheck`
* **최종 진단 판정**: **❌ 미탐 (Silent / False Negative) - 아무런 경고도 감지되지 않음**

### 💻 검증 소스코드
> 파일 위치: `dapa_test_suite.cpp` (L980 ~ L988)

```cpp
 980 | namespace Rule_62 {
 981 |     namespace {
 982 |         class A {
 983 | #if 0
 984 |             virtual void f() = 1;  // ❌ 잘못된 초기화 → 쓰레기 값 가능
 985 | #endif
 986 |         };
 987 |     }
 988 | }
```

### 🔍 Cppcheck 진단 경고 로그 (Raw Output)
> ❌ **검출된 경고 없음 (Silent)**

---

## 📌 Rule 63: 순수 함수는 반드시 `virtual`로 선언되어야 한다.

* **세부 조항**: 12
* **매핑 체커 (Clang-Tidy)**: `VirtualPureCheck`
* **최종 진단 판정**: **⚠️ 미탐 (파생 경고만 발생) - 결함은 미탐지하고 부수적 경고만 발생**

### 💻 검증 소스코드
> 파일 위치: `dapa_test_suite.cpp` (L994 ~ L1006)

```cpp
 994 | namespace Rule_63 {
 995 |     class Abstract {
 996 |     public:
 997 | #if 0
 998 |         void foo() = 0;  // ❌ virtual 키워드 누락 → 순수 가상 함수로 인식 안 됨
 999 | #endif
1000 |     };
1001 | 
1002 |     class Derived : public Abstract {
1003 |     public:
1004 |         void foo() {}    // ⚠️ 재정의는 했지만 다형성이 깨질 수 있음
1005 |     };
1006 | }
```

### 🔍 Cppcheck 진단 경고 로그 (Raw Output)
| 라인 (Line) | 중요도 (Severity) | 경고 ID (Warning ID) | 분류 | 진단 메시지 (Message) |
| :---: | :---: | :---: | :---: | :--- |
| 1004 | style | `functionStatic` | ⚠️ 파생 경고 (Collateral) | The member function 'Rule_63::Derived::foo' can be static. |

---

## 📌 Rule 64: virtual base 클래스 포인터를 derived 클래스 포인터로 변환할 때에는 반드시 `dynamic_cast`를 사용해야 한다.

* **세부 조항**: 13
* **매핑 체커 (Clang-Tidy)**: `VirtualBaseCastCheck`
* **최종 진단 판정**: **⚠️ 미탐 (파생 경고만 발생) - 결함은 미탐지하고 부수적 경고만 발생**

### 💻 검증 소스코드
> 파일 위치: `dapa_test_suite.cpp` (L1012 ~ L1033)

```cpp
1012 | namespace Rule_64 {
1013 |     class Derived;
1014 |     class Base {
1015 |     public:
1016 |         virtual ~Base();
1017 |         virtual Derived* getAsDerived() = 0;
1018 |     };
1019 | 
1020 |     class Derived : virtual public Base {
1021 |     public:
1022 |         virtual Derived* getAsDerived() {
1023 |             return this;
1024 |         }
1025 |     };
1026 | 
1027 |     void foo(Base* bp, Derived* dp) {
1028 | #if 0
1029 |         dp = static_cast<Derived*>(bp);       // ❌ 위반: 안전하지 않음
1030 | #endif
1031 |         dp = reinterpret_cast<Derived*>(bp);  // ❌ 위반: 위험한 캐스트
1032 |     }
1033 | }
```

### 🔍 Cppcheck 진단 경고 로그 (Raw Output)
| 라인 (Line) | 중요도 (Severity) | 경고 ID (Warning ID) | 분류 | 진단 메시지 (Message) |
| :---: | :---: | :---: | :---: | :--- |
| 1031 | warning | `uselessAssignmentPtrArg` | ⚠️ 파생 경고 (Collateral) | Assignment of function parameter has no effect outside the function. Did you forget dereferencing it? |
| 1022 | style | `missingOverride` | ⚠️ 파생 경고 (Collateral) | The function 'getAsDerived' overrides a function in a base class but is not marked with a 'override' specifier. |
| 1031 | style | `unreadVariable` | ⚠️ 파생 경고 (Collateral) | Variable 'dp' is assigned a value that is never used. |

---

## 📌 Rule 65: 생성자/소멸자 내에서는 가상함수를 반드시 식별자(Qualifier)를 붙여서 호출해야 한다.

* **세부 조항**: 14
* **매핑 체커 (Clang-Tidy)**: `DynamicCastInCtorDtorCheck`
* **최종 진단 판정**: **✅ 정탐 (True Positive) - 의도된 결함 정확히 검출**

### 💻 검증 소스코드
> 파일 위치: `dapa_test_suite.cpp` (L1039 ~ L1063)

```cpp
1039 | namespace Rule_65 {
1040 |     class B {
1041 |     public:
1042 |         virtual ~B() = 0;
1043 |         virtual void func();
1044 |     protected:
1045 |         B();
1046 |         B(const B&);
1047 |         B& operator=(const B&);
1048 |     };
1049 | 
1050 |     class D : public B {
1051 |     public:
1052 |         D() : B() {}
1053 |         virtual ~D();
1054 |         virtual void func();
1055 |     private:
1056 |         D(const D&);
1057 |         D& operator=(const D&);
1058 |     };
1059 | 
1060 |     B::B() {
1061 |         func();  // ❌ 위반: 식별자 없이 가상 함수 호출
1062 |     }
1063 | }
```

### 🔍 Cppcheck 진단 경고 로그 (Raw Output)
| 라인 (Line) | 중요도 (Severity) | 경고 ID (Warning ID) | 분류 | 진단 메시지 (Message) |
| :---: | :---: | :---: | :---: | :--- |
| 1043 | style | `virtualCallInConstructor` | ✅ 정탐 경고 (Genuine) | Virtual function 'func' is called from constructor 'B()' at line 1061. Dynamic binding is not used. |
| 1053 | style | `missingOverride` | ⚠️ 파생 경고 (Collateral) | The destructor '~D' overrides a destructor in a base class but is not marked with a 'override' specifier. |
| 1054 | style | `missingOverride` | ⚠️ 파생 경고 (Collateral) | The function 'func' overrides a function in a base class but is not marked with a 'override' specifier. |

---

## 📌 Rule 66: 생성자 또는 소멸자에서는 `dynamic_cast`를 사용해서는 안 된다.

* **세부 조항**: 15
* **매핑 체커 (Clang-Tidy)**: `DynamicCastInCtorDtorCheck`
* **최종 진단 판정**: **✅ 정탐 (True Positive) - 의도된 결함 정확히 검출**

### 💻 검증 소스코드
> 파일 위치: `dapa_test_suite.cpp` (L1069 ~ L1081)

```cpp
1069 | namespace Rule_66 {
1070 |     class B2 {
1071 |     public:
1072 |         virtual ~B2();
1073 |         virtual void foo();
1074 | 
1075 |         B2() {
1076 |             typeid(B2);                      // ❌ 런타임 타입 정보 사용 → 위반
1077 |             foo();                           // ❌ 가상 함수 직접 호출 → 위반
1078 |             dynamic_cast<B2*>(this);         // ❌ dynamic_cast 사용 → 위반
1079 |         }
1080 |     };
1081 | }
```

### 🔍 Cppcheck 진단 경고 로그 (Raw Output)
| 라인 (Line) | 중요도 (Severity) | 경고 ID (Warning ID) | 분류 | 진단 메시지 (Message) |
| :---: | :---: | :---: | :---: | :--- |
| 1073 | style | `virtualCallInConstructor` | ✅ 정탐 경고 (Genuine) | Virtual function 'foo' is called from constructor 'B2()' at line 1077. Dynamic binding is not used. |
| 1076 | warning | `constStatement` | ⚠️ 파생 경고 (Collateral) | Redundant code: Found unused 'typeid' expression. |

---
