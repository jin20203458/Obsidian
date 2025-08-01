
**Opaque Pointer**(오페이크 포인터)란,  
포인터가 가리키는 데이터 구조체의 내부 구현 내용이 _선언 시점에 공개되지 않은_ 포인터를 말합니다.

> 링크 전까진 타 cpp가 다른cpp의 완전한 정보은닉을 실현 가능하다.
## 핵심 개념

- **내용이 보이지 않는 포인터**  
    포인터가 가리키는 구조체나 클래스의 정의가 헤더 파일 등에서 감춰져 있어,  
    사용자는 포인터를 통해 내부 멤버에 직접 접근할 수 없습니다.
    
- **캡슐화와 정보 은닉**  
    구현 세부사항을 숨겨서 인터페이스와 구현을 분리하고,  
    구현 변경 시 사용자 코드 재컴파일 없이 라이브러리만 교체할 수 있게 합니다.
    
- **주로 C/C++에서 사용**  
    특히 C++에서는 **Pimpl idiom(포인터 투 구현)이나 d-pointer** 패턴으로 널리 쓰입니다.
    

## 동작 방식

- 헤더 파일에는 포인터 타입만 선언하고, 구조체 정의는 소스 파일에 숨깁니다.

**person.h**
```cpp
#ifndef PERSON_H
#define PERSON_H
typedef struct p person;  // 내부 정의는 감춤

person* make_person(char *name, int age);
void destroy_person(person *p);
void say_hello(person *p);
int reverse(person *p);
#endif
```
- 실제 구현은 cpp 파일에 숨겨 구현


**Person.cpp**
```cpp
// person.c
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "person.h"

typedef struct p 
{
    char *name;
    int age;
    int token;
} person;

static int generate_token(void) 
{
    return rand();
}

person* make_person(char *name, int age)
{
    person *p = malloc(sizeof(person));
    p->name = strdup(name);
    p->age = age;
    p->token = generate_token();
    return p;
}

void destroy_person(person *p) 
{
    if (p) {
        free(p->name);
        free(p);
    }
}

void say_hello(person *p) 
{
    printf("Hi %s, you are %d years of age! Your token is %d.\n",
     p->name, p->age, p->token);
}

int reverse(person *p) 
{
    int len = strlen(p->name);
    for (int i = 0, j = len - 1; i < j; ++i, --j) 
    {
        char tmp = p->name[i];
        p->name[i] = p->name[j];
        p->name[j] = tmp;
    }
    return len;
}

```


**사용예시** main.c
```cpp
#include "person.h"
#include <stdlib.h>

int main(int argc, char **argv) 
{
    if (argc != 3) return 1;
    person *p = make_person(argv[1], atoi(argv[2]));
    say_hello(p);
    reverse(p);
    say_hello(p);
    destroy_person(p);
    return 0;
}
```

## 장점

- **컴파일 의존성 감소**  
    구현 변경 시 헤더가 바뀌지 않아, 의존하는 모듈 재컴파일 불필요
    
- **이진 호환성 유지**  
    라이브러리 인터페이스를 유지하면서 내부 구현 변경 가능
    
- **캡슐화 강화**  
    내부 데이터 구조를 외부에 노출하지 않음
    

## 단점

- **간접 접근 비용**  
    포인터를 통해 멤버에 접근하므로 약간의 성능 저하 가능
    
- **코드 복잡도 증가**  
    구현 분리로 인해 코드가 다소 복잡해짐
    

## 요약

|용어|설명|
|---|---|
|Opaque Pointer|내부 구조가 감춰진(불투명한) 포인터|
|Pimpl Idiom|C++에서 opaque pointer를 활용한 구현 분리 패턴|
|d-pointer|Qt, KDE 등에서 사용하는 opaque pointer 별칭|

## 참고

- LLVM, GTK+, Qt 등 대형 프로젝트에서 광범위하게 사용
    
- C++에서 `std::unique_ptr`와 함께 쓰면 메모리 관리도 편리
    

**결론:**  
**Opaque pointer는 구현 세부사항을 숨기고 인터페이스만 노출하는 강력한 캡슐화 수단**으로,  
대형 소프트웨어에서 유지보수성과 이진 호환성을 높이는 데 중요한 역할을 합니다.

1. [https://www.geeksforgeeks.org/cpp/opaque-pointer-in-cpp/](https://www.geeksforgeeks.org/cpp/opaque-pointer-in-cpp/)
2. [https://en.wikipedia.org/wiki/Opaque_pointer](https://en.wikipedia.org/wiki/Opaque_pointer)
3. [https://www.scaler.com/topics/cpp/opaque-pointer-in-cpp/](https://www.scaler.com/topics/cpp/opaque-pointer-in-cpp/)
4. [https://www.reddit.com/r/cpp/comments/1ekc00o/opaque_pointer_pattern_in_c/](https://www.reddit.com/r/cpp/comments/1ekc00o/opaque_pointer_pattern_in_c/)
5. [https://www.shiksha.com/online-courses/articles/opaque-pointer-in-cpp/](https://www.shiksha.com/online-courses/articles/opaque-pointer-in-cpp/)
6. [https://danielsieger.com/blog/2024/08/02/cpp-opaque-pointer-pattern.html](https://danielsieger.com/blog/2024/08/02/cpp-opaque-pointer-pattern.html)
7. [https://stackoverflow.com/questions/7553750/what-is-an-opaque-pointer-in-c](https://stackoverflow.com/questions/7553750/what-is-an-opaque-pointer-in-c)
8. [https://llvm.org/docs/OpaquePointers.html](https://llvm.org/docs/OpaquePointers.html)
9. [http://manu343726.github.io/2016-03-07-c++11-opaque-pointer-idiom/](http://manu343726.github.io/2016-03-07-c++11-opaque-pointer-idiom/)
10. [https://pyrasis.com/blog/entry/PracticalOpaqueType](https://pyrasis.com/blog/entry/PracticalOpaqueType)