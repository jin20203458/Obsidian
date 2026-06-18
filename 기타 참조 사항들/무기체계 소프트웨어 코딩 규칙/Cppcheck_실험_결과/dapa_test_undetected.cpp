// ============================================================================
// DAPA Software Coding Guidelines - Cppcheck Undetected (False Negative) Test Suite
// Cppcheck가 미탐(검출 실패)한 25종의 규칙 위반 사례(Bad Case) 모음
// ============================================================================

#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <stdexcept>
#include <algorithm>

// ----------------------------------------------------------------------------
// Rule 02: [Common_Style] Bullet 나 - Switch 구문에서 첫번째 Label 전에 코드 구문이 존재하면 안된다.
// ----------------------------------------------------------------------------
namespace Rule_02 {
    void foo(int m) {
        int n = 0;
        switch (m) {
            n = n + 1; // ❌ 위반: 라벨 이전의 도달할 수 없는 데드 코드 (Cppcheck 미탐)
            case 1:
                --n;
                break;
            default:
                ++n;
                break;
        }
    }
}

// ----------------------------------------------------------------------------
// Rule 03: [Common_Style] Bullet 다 - 함수/변수의 선언 시 type을 명시해야 한다.
// ----------------------------------------------------------------------------
namespace Rule_03 {
    auto x = 10; // ❌ 위반: auto 사용을 통한 타입 생략 (Cppcheck 미탐)
    auto bar() { // ❌ 위반: 반환 타입 auto 생략
        return 42;
    }
}

// ----------------------------------------------------------------------------
// Rule 04: [Common_Style] Bullet 라 - 의미 없는 구문은 사용하지 말아야 한다.(side effect)
// ----------------------------------------------------------------------------
namespace Rule_04 {
    void foo(int x) {
        x + 1; // ❌ 위반: 대입이나 함수호출이 없는 무의미한 수식 (Cppcheck 미탐)
    }
}

// ----------------------------------------------------------------------------
// Rule 06: [Common_Style] Bullet 바 - 외부 함수 사용시 이를 명시하고 사용해야 한다.
// ----------------------------------------------------------------------------
namespace Rule_06 {
    // 프로토타입 선언이나 extern 선언 없이 임의의 외부 심볼 호출 유도 (Cppcheck 미탐)
}

// ----------------------------------------------------------------------------
// Rule 07: [Common_Style] Bullet 사 - 전처리에 사용되는 매크로는 사용 전에 반드시 정의되어야 한다.
// ----------------------------------------------------------------------------
namespace Rule_07 {
    #if MY_MACRO == 1 // ❌ 위반: 정의되지 않은 매크로 비교 (Cppcheck 미탐)
    void test() {}
    #endif
}

// ----------------------------------------------------------------------------
// Rule 10: [Common_Style] Bullet 차 - switch ~ case 문은 default 문이 포함되어야 한다
// ----------------------------------------------------------------------------
namespace Rule_10 {
    void no_default(int x) {
        switch (x) { // ❌ 위반: default 분기 누락 (Cppcheck 미탐)
            case 1:
                break;
        }
    }
}

// ----------------------------------------------------------------------------
// Rule 11: [Common_Style] Bullet 카 - 한 줄에 하나의 명령문을 사용한다.
// ----------------------------------------------------------------------------
namespace Rule_11 {
    void multi_statement() {
        int a = 1; int b = 2; // ❌ 위반: 한 줄에 복수 구문 배치 (Cppcheck 미탐)
    }
}

// ----------------------------------------------------------------------------
// Rule 12: [Common_Style] Bullet 타 - if - else if 문은 else 문도 반드시 포함 시킨다.
// ----------------------------------------------------------------------------
namespace Rule_12 {
    void no_else(int x) {
        if (x == 1) {
            // ...
        } else if (x == 2) { // ❌ 위반: else 누락 (Cppcheck 미탐)
            // ...
        }
    }
}

// ----------------------------------------------------------------------------
// Rule 13: [Common_Init] Bullet 가 - char 배열의 크기는 Null을 포함하는 크기로 선언되어야 한다.
// ----------------------------------------------------------------------------
namespace Rule_13 {
    void char_array_size() {
        char str[5] = "Hello"; // ❌ 위반: Null 공간 누락 (Cppcheck 미탐)
    }
}

// ----------------------------------------------------------------------------
// Rule 16: [Common_Ident] Bullet 가 - external과 internal linkage의 특성을 동시에 가질 수 없다.
// ----------------------------------------------------------------------------
namespace Rule_16 {
    static int shared_val;
    extern int shared_val; // ❌ 위반: 내부/외부 링키지 혼용 선언 (Cppcheck 미탐)
}

// ----------------------------------------------------------------------------
// Rule 17: [Common_Ident] Bullet 나 - external linkage scope에서 선언된 함수나 Object의 이름은 유일해야 한다.
// ----------------------------------------------------------------------------
namespace Rule_17 {
    // 외부 링크 전역 중복 네임 선언 유도 (Cppcheck 미탐)
}

// ----------------------------------------------------------------------------
// Rule 18: [Common_Ident] Bullet 다 - 선언 시 데이터형과 정의 시 데이터형이 동일해야 한다.
// ----------------------------------------------------------------------------
namespace Rule_18 {
    extern void my_func(int x);
    void my_func(double x) { // ❌ 위반: 선언과 파라미터 타입 불일치 (Cppcheck 미탐)
    }
}

// ----------------------------------------------------------------------------
// Rule 20: [Common_Cond] Bullet 가 - float 자료형에서 동등성 비교연산을 수행하지 말아야 한다.
// ----------------------------------------------------------------------------
namespace Rule_20 {
    void compare_float(float a, float b) {
        if (a == b) { // ❌ 위반: 부동 소수점 동등 비교 (Cppcheck 미탐)
            // ...
        }
    }
}

// ----------------------------------------------------------------------------
// Rule 22: [Common_Cond] Bullet 다 - switch의 case의 조건을 만족할 수 없는 Label을 사용하지 않는다.
// ----------------------------------------------------------------------------
namespace Rule_22 {
    void unreachable_case(unsigned char x) {
        switch (x) {
            case 256: // ❌ 위반: 타입 범위를 벗어난 도달 불가능 case (Cppcheck 미탐)
                break;
        }
    }
}

// ----------------------------------------------------------------------------
// Rule 23: [Common_Cond] Bullet 라 - Switch 구문에서 Expression을 논리적 연산으로 사용하지 말아야 한다.
// ----------------------------------------------------------------------------
namespace Rule_23 {
    void switch_bool(int x) {
        switch (x > 10) { // ❌ 위반: 조건 판단식을 switch 제어값으로 지정 (Cppcheck 미탐)
            case true:
                break;
        }
    }
}

// ----------------------------------------------------------------------------
// Rule 30: [Common_Conv] Bullet 바 - Character 문자열과 Wide character 문자열을 혼용하지 말아야 한다.
// ----------------------------------------------------------------------------
namespace Rule_30 {
    void mixed_char() {
        const char* p = "Hello" L"World"; // ❌ 위반: 일반문자 및 와이드문자 문자열 병합 (Cppcheck 미탐)
    }
}

// ----------------------------------------------------------------------------
// Rule 31: [Common_Conv] Bullet 사 - 명시적 캐스팅 시 CONST 속성을 유지해야 한다.
// ----------------------------------------------------------------------------
namespace Rule_31 {
    void const_drop() {
        const int* p = new int(10);
        int* non_const_p = (int*)p; // ❌ 위반: const 속성 무시 캐스트 (Cppcheck 미탐)
    }
}

// ----------------------------------------------------------------------------
// Rule 32: [Common_Conv] Bullet 아 - 명시적 캐스팅 시 volatile 속성을 유지해야 한다.
// ----------------------------------------------------------------------------
namespace Rule_32 {
    void volatile_drop() {
        volatile int* p = new int(10);
        int* non_volatile_p = (int*)p; // ❌ 위반: volatile 속성 무시 캐스트 (Cppcheck 미탐)
    }
}

// ----------------------------------------------------------------------------
// Rule 48: [C_Specific] Bullet 2 - #include 구문에서 표준에 맞지 않는 Character set을 사용하지 않아야 한다.
// ----------------------------------------------------------------------------
namespace Rule_48 {
    #include "invalid'char.h" // ❌ 위반: 특수문자가 섞인 헤더 인클루드 (Cppcheck 미탐)
}

// ----------------------------------------------------------------------------
// Rule 49: [C_Specific] Bullet 3 - malloc에서 사용하는 인자값은 assign될 변수 type의 정수배이어야 한다.
// ----------------------------------------------------------------------------
namespace Rule_49 {
    void bad_malloc() {
        long* ptr;
        ptr = (long*)malloc(3); // ❌ 위반: long 크기의 정수배가 아님 (Cppcheck 미탐)
    }
}

// ----------------------------------------------------------------------------
// Rule 51: [C_Specific] Bullet 5 - 구조체/배열의 초기화 시 구조에 맞게 ‘{}’를 사용하여 초기화 해야 한다.
// ----------------------------------------------------------------------------
namespace Rule_51 {
    struct Sub { int a; int b; };
    struct Parent { struct Sub s; int c; };
    void bad_struct_init() {
        struct Parent p = { 1, 2, 3 }; // ❌ 위반: 중첩 브레이스 {} 생략 (Cppcheck 미탐)
    }
}

// ----------------------------------------------------------------------------
// Rule 55: [CPP_Specific_1] Bullet 4 - 소멸자내에서 처리할 수 없는 예외 상황을 발생시키지 말아야 한다.
// ----------------------------------------------------------------------------
namespace Rule_55 {
    class BadDtor {
    public:
        ~BadDtor() {
            throw std::runtime_error("Error"); // ❌ 위반: 소멸자 내 throw 발생 (Cppcheck 미탐)
        }
    };
}

// ----------------------------------------------------------------------------
// Rule 56: [CPP_Specific_1] Bullet 5 - 사용되지 않는 예외 처리 문을 작성하지 말아야 한다.
// ----------------------------------------------------------------------------
namespace Rule_56 {
    void unused_catch() {
        try {
            int a = 10;
        } catch (const std::exception& e) { // ❌ 위반: 불필요한 예외 포획 (Cppcheck 미탐)
        }
    }
}

// ----------------------------------------------------------------------------
// Rule 57: [CPP_Specific_1] Bullet 6 - exception specification에 기술되지 않은 모든 throw에 대하여 예외처리를 해야만 한다.
// ----------------------------------------------------------------------------
namespace Rule_57 {
    void throw_spec() throw(int) {
        throw 1.5; // ❌ 위반: double 형 예외 throw 명세 위반 (Cppcheck 미탐)
    }
}

// ----------------------------------------------------------------------------
// Rule 58: [CPP_Specific_1] Bullet 7 - main 함수에서 처리되지 않는 throw를 작성하지 말아야 한다.
// ----------------------------------------------------------------------------
namespace Rule_58 {
    int main() {
        throw std::runtime_error("Unhandled"); // ❌ 위반: 처리 안되는 throw 직접 실행 (Cppcheck 미탐)
    }
}

// ----------------------------------------------------------------------------
// Rule 60: [CPP_Specific_2] Bullet 9 - copy operator를 통해서, 복사되지 않는 멤버 변수가 존재하지 말아야 한다.
// ----------------------------------------------------------------------------
namespace Rule_60 {
    class CopyBad {
    private:
        int a;
        int b;
    public:
        CopyBad& operator=(const CopyBad& other) {
            a = other.a; // ❌ 위반: 멤버 b 복사 누락 (Cppcheck 미탐)
            return *this;
        }
    };
}

// ----------------------------------------------------------------------------
// Rule 61: [CPP_Specific_2] Bullet 10 - C++에서 동적 메모리 할당은 반드시 `new` 연산자를 사용해야 한다.
// ----------------------------------------------------------------------------
namespace Rule_61 {
    void cpp_malloc() {
        int* p = (int*)malloc(sizeof(int)); // ❌ 위반: C++에서 malloc 직접 호출 (Cppcheck 미탐)
        free(p);
    }
}

// ----------------------------------------------------------------------------
// Rule 62: [CPP_Specific_2] Bullet 11 - 순수 가상 함수는 반드시 `= 0` 으로 초기화해야 한다.
// ----------------------------------------------------------------------------
namespace Rule_62 {
    class BasePure {
    public:
        virtual void func(); // 순수 가상화가 완전 누락됨 (Cppcheck 미탐)
    };
}

// ----------------------------------------------------------------------------
// Rule 63: [CPP_Specific_2] Bullet 12 - 순수 함수는 반드시 `virtual`로 선언되어야 한다.
// ----------------------------------------------------------------------------
namespace Rule_63 {
    class BaseNoVirtual {
    public:
        void func() = 0; // ❌ 위반: virtual 키워드 없이 =0 지정 (Cppcheck 미탐)
    };
}

// ----------------------------------------------------------------------------
// Rule 64: [CPP_Specific_2] Bullet 13 - 포인터 변환할 때에는 반드시 `dynamic_cast`를 사용해야 한다.
// ----------------------------------------------------------------------------
namespace Rule_64 {
    class VBase { virtual void f() {} };
    class VDerived : public VBase {};
    void test_cast(VBase* b) {
        VDerived* d = (VDerived*)b; // ❌ 위반: dynamic_cast 대신 C스타일 강제 형변환 (Cppcheck 미탐)
    }
}

// ----------------------------------------------------------------------------
// Rule 66: [CPP_Specific_2] Bullet 15 - 생성자 또는 소멸자에서는 `dynamic_cast`를 사용해서는 안 된다.
// ----------------------------------------------------------------------------
namespace Rule_66 {
    class CastBase { virtual void f() {} };
    class CastDerived : public CastBase {};
    class TestClass : public CastBase {
    public:
        TestClass() {
            CastDerived* d = dynamic_cast<CastDerived*>(this); // ❌ 위반: 생성자 내 dynamic_cast 사용 (Cppcheck 미탐)
        }
    };
}
