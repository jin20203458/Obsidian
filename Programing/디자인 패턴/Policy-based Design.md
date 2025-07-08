>**정책 기반 설계**: “동작 방식만 바꾼다.”

C++의 **Policy-based Design**(정책 기반 설계)은 클래스의 동작 방식을 *정책(Policy)*이라는 독립적인 단위로 분리하고, 이를 **템플릿 인자**로 전달하여 클래스의 특정 행동을 컴파일 타임에 유연하게 결정할 수 있게 하는 설계 패턴입니다[1](https://www.devoops.kr/153)[4](https://ko.wikipedia.org/wiki/%EB%8B%A8%EC%9C%84%EC%A0%84%EB%9E%B5_%EA%B8%B0%EB%B0%98_%EB%94%94%EC%9E%90%EC%9D%B8)[6](https://www.slideshare.net/slideshow/policy-based-class-design/36671426).

## 주요 특징

- **정책 교체의 유연성**: 클래스가 사용하는 정책(예: 동기화 방식, 메모리 할당 방식 등)을 템플릿 파라미터로 받아, 객체 생성 시점에 다양한 정책을 손쉽게 교체할 수 있습니다. 예를 들어, 동기화가 필요 없는 경우 NoLock, 멀티스레드 환경에서는 MutexLock을 선택할 수 있습니다[1](https://www.devoops.kr/153)[3](https://definelife.tistory.com/46).
    
- **성능 저하 없음**: 정책 클래스는 보통 인라인 함수로 구현되어, 가상 함수 기반의 전략 패턴(Strategy Pattern)과 달리 런타임 오버헤드 없이 최적화가 가능합니다. 즉, 정책이 비어있으면 최적화 과정에서 코드가 완전히 제거됩니다[1](https://www.devoops.kr/153)[3](https://definelife.tistory.com/46)[4](https://ko.wikipedia.org/wiki/%EB%8B%A8%EC%9C%84%EC%A0%84%EB%9E%B5_%EA%B8%B0%EB%B0%98_%EB%94%94%EC%9E%90%EC%9D%B8).
    
- **컴파일 타임 결정**: 정책은 컴파일 타임에 결정되므로, 런타임에 동적으로 변경할 수는 없습니다. 대신 실행 효율성이 높아집니다[3](https://definelife.tistory.com/46)[4](https://ko.wikipedia.org/wiki/%EB%8B%A8%EC%9C%84%EC%A0%84%EB%9E%B5_%EA%B8%B0%EB%B0%98_%EB%94%94%EC%9E%90%EC%9D%B8).
    
- **템플릿 메타프로그래밍 활용**: C++의 템플릿 시스템을 적극적으로 활용하며, Andrei Alexandrescu의 『Modern C++ Design』에서 체계적으로 소개되었습니다[4](https://ko.wikipedia.org/wiki/%EB%8B%A8%EC%9C%84%EC%A0%84%EB%9E%B5_%EA%B8%B0%EB%B0%98_%EB%94%94%EC%9E%90%EC%9D%B8)[7](http://ajwmain.iptime.org/programming/book_summary/%5B04%5Dmodern_cpp_design/modern_cpp_design.html).
    

## 간단한 예시

```cpp
template<typename Policy>
class Host : public Policy { /* ... */ };
// Policy가 바뀌어도 Host의 public 인터페이스는 동일
```
이처럼 **정책 클래스**(Policy Class)는 보통 단순한 동작만을 포함하며, **호스트 클래스**(Host Class)는 정책을 조합해 복합 동작을 구현합니다[1](https://www.devoops.kr/153)[6](https://www.slideshare.net/slideshow/policy-based-class-design/36671426).

## Policy-based Design vs. Strategy Pattern

|Policy-based Design|Strategy Pattern|
|---|---|
|정책을 템플릿 인자로 전달(컴파일 타임 결정)|정책을 객체로 전달(런타임 결정)|
|인라인 치환 가능, 성능 저하 없음|가상 함수 호출로 오버헤드 발생|
|정책 교체는 컴파일 시점에만 가능|실행 중 정책 교체 가능|
|명시적 인터페이스 없이 구현|인터페이스(추상 클래스) 필요|
|문서화 필요(구현 규약이 명확하지 않음)|인터페이스가 구현 규약을 제공|

[3](https://definelife.tistory.com/46)[4](https://ko.wikipedia.org/wiki/%EB%8B%A8%EC%9C%84%EC%A0%84%EB%9E%B5_%EA%B8%B0%EB%B0%98_%EB%94%94%EC%9E%90%EC%9D%B8)

## 실제 활용

- **STL Allocator**: 메모리 할당 정책을 템플릿 인자로 전달해 다양한 할당 전략을 지원합니다[1](https://www.devoops.kr/153).
    
- **스마트 포인터, 컨테이너, 싱글턴 등**: 다양한 라이브러리와 프레임워크에서 정책 기반 설계가 활용됩니다[1](https://www.devoops.kr/153)[6](https://www.slideshare.net/slideshow/policy-based-class-design/36671426)[7](http://ajwmain.iptime.org/programming/book_summary/%5B04%5Dmodern_cpp_design/modern_cpp_design.html).
    

## 요약

- **Policy-based Design**은 C++ 템플릿을 활용해, 클래스의 특정 동작을 정책 단위로 분리·조합할 수 있는 강력한 설계 패턴입니다.
    
- 런타임 오버헤드 없이 다양한 동작을 지원하며, 복잡한 라이브러리나 프레임워크 설계에 널리 사용됩니다[1](https://www.devoops.kr/153)[4](https://ko.wikipedia.org/wiki/%EB%8B%A8%EC%9C%84%EC%A0%84%EB%9E%B5_%EA%B8%B0%EB%B0%98_%EB%94%94%EC%9E%90%EC%9D%B8)[6](https://www.slideshare.net/slideshow/policy-based-class-design/36671426).
    

1. [https://www.devoops.kr/153](https://www.devoops.kr/153)
2. [https://www.ikpil.com/1015](https://www.ikpil.com/1015)
3. [https://definelife.tistory.com/46](https://definelife.tistory.com/46)
4. [https://ko.wikipedia.org/wiki/%EB%8B%A8%EC%9C%84%EC%A0%84%EB%9E%B5_%EA%B8%B0%EB%B0%98_%EB%94%94%EC%9E%90%EC%9D%B8](https://ko.wikipedia.org/wiki/%EB%8B%A8%EC%9C%84%EC%A0%84%EB%9E%B5_%EA%B8%B0%EB%B0%98_%EB%94%94%EC%9E%90%EC%9D%B8)
5. [https://www.ikpil.com/1280](https://www.ikpil.com/1280)
6. [https://www.slideshare.net/slideshow/policy-based-class-design/36671426](https://www.slideshare.net/slideshow/policy-based-class-design/36671426)
7. [http://ajwmain.iptime.org/programming/book_summary/%5B04%5Dmodern_cpp_design/modern_cpp_design.html](http://ajwmain.iptime.org/programming/book_summary/%5B04%5Dmodern_cpp_design/modern_cpp_design.html)
8. [https://www.ecourse.co.kr/courses/cpp_designpattern/](https://www.ecourse.co.kr/courses/cpp_designpattern/)