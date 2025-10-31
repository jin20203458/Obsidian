
## 💡 개념적 모델 (Conceptual Model): 순차적 단계

이 모델은 컴파일 과정을 이해하기 쉽게 단계별로 나눈 교육용 모델입니다. 각 단계는 이전 단계의 출력을 입력으로 받아 자신의 역할을 수행합니다.

## 1단계: 전처리 (Preprocessing)

- **목표**: 모든 `#`으로 시작하는 지시문을 처리하여 순수한 C++ 코드를 만듭니다.
    
- **작업**:
    
    - `#include`를 만나면 해당 헤더 파일의 내용을 소스 코드에 그대로 붙여넣습니다.
        
    - `#define` 매크로를 정의된 내용으로 모두 치환합니다.
        
    - `#ifdef` 등 조건부 컴파일 지시문에 따라 특정 코드 블록을 포함하거나 제외합니다.
        
    - 주석을 제거합니다.
        
- **출력**: 모든 전처리가 완료된, 거대하고 단일한 소스 파일 (보통 `.i` 또는 `.ii` 확장자로 저장 가능).[velog+1](https://velog.io/@hbcho/%EC%BB%B4%ED%8C%8C%EC%9D%BC%EB%9F%AC%EC%9D%98-4%EB%8B%A8%EA%B3%84-%EC%9E%91%EB%8F%99-%EC%9B%90%EB%A6%AC-%EC%A0%84%EC%B2%98%EB%A6%AC%EB%B6%80%ED%84%B0-%EB%A7%81%ED%81%AC%EA%B9%8C%EC%A7%80-%EC%99%84%EB%B2%BD-%EC%9D%B4%ED%95%B4)​
    

## 2단계: 어휘 분석 (Lexical Analysis / Lexing)

- **목표**: 전처리된 순수 C++ 코드를 의미 있는 최소 단위인 **토큰(Token)**으로 분해합니다.
    
- **입력**: 전처리 단계에서 만들어진 단일 소스 파일.
    
- **작업**:
    
    - `int main() { return 0; }` 라는 텍스트를 `int`, `main`, `(`, `)`, `{`, `return`, `0`, `;`, `}` 라는 토큰의 스트림으로 변환합니다.
        
- **출력**: 토큰의 연속적인 스트림.
    

**요약**: 개념적으로는, 전처리기가 모든 `#` 지시문을 먼저 처리하여 하나의 큰 소스 파일을 만들고, 그 다음에 Lexer가 이 파일을 읽어 토큰으로 분해하는 **명확한 순차적 관계**입니다.

---

## 🚀 실제 Clang 구현 (Practical Implementation): 통합된 파이프라인

Clang의 실제 구현은 성능과 메모리 효율을 극대화하기 위해 이 단계들을 유기적으로 통합합니다. "요청 기반(On-demand)" 방식으로 동작합니다.

## 통합된 동작 방식

1. **시작점은 Parser**: 모든 것은 `Parser`가 "다음 토큰 줘"라고 요청하는 데서 시작합니다.
    
2. **Lexer의 역할**: `Parser`의 요청을 받으면, `Lexer`는 소스 코드에서 다음 토큰을 만들기 시작합니다.
    
3. **Preprocessor의 실시간 개입**:
    
    - `Lexer`가 코드를 읽다가 `#` 지시문이나 매크로 이름을 만나면, 자신의 작업을 멈추고 즉시 `Preprocessor`에게 제어권을 넘깁니다.
        
    - **`#include` 처리**: `Preprocessor`는 `Lexer`에게 "이제부터 이 헤더 파일을 읽어라"라고 알려줍니다. `Lexer`는 즉시 파일 읽기 대상을 바꾸고, 헤더 파일의 끝에 도달하면 이전 위치로 복귀합니다.
        
    - **매크로 확장**: `Lexer`가 매크로 이름을 만나면, `Preprocessor`는 해당 매크로에 정의된 토큰 시퀀스를 `Lexer`에게 돌려줍니다. `Lexer`는 이 토큰들을 마치 원래 소스 코드에 있었던 것처럼 `Parser`에게 하나씩 전달합니다.
        
4. **최종 토큰 전달**: 이 모든 내부적인 상호작용 끝에, `Lexer`는 최종적으로 만들어진 토큰 하나를 `Parser`에게 전달하고 대기합니다.
    

**요약**: 실제 구현에서는 전처리가 별도의 단계로 먼저 실행되는 것이 아닙니다. `Lexer`가 토큰을 생성하는 과정 중에 **`Preprocessor`가 실시간으로 개입**하여 소스의 흐름을 바꾸거나 토큰을 바꿔치기하는 **매우 긴밀하게 얽힌(intertwined) 관계**입니다. 이 방식은 모든 헤더를 미리 메모리에 펼쳐놓지 않아도 되므로 훨씬 효율적입니다.

---

|구분|개념적 모델|실제 Clang 구현|
|---|---|---|
|**순서**|전처리 → Lexing (명확한 순차)|Lexing 도중 Preprocessor가 실시간 개입 (통합/동시적)|
|**실행 방식**|전체 파일 전처리 후, 전체 파일 토큰화|토큰 하나를 요청받을 때마다 필요한 만큼만 처리|
|**목적**|이해의 용이성|성능 및 메모리 효율 최적화|

1. [https://velog.io/@hbcho/%EC%BB%B4%ED%8C%8C%EC%9D%BC%EB%9F%AC%EC%9D%98-4%EB%8B%A8%EA%B3%84-%EC%9E%91%EB%8F%99-%EC%9B%90%EB%A6%AC-%EC%A0%84%EC%B2%98%EB%A6%AC%EB%B6%80%ED%84%B0-%EB%A7%81%ED%81%AC%EA%B9%8C%EC%A7%80-%EC%99%84%EB%B2%BD-%EC%9D%B4%ED%95%B4](https://velog.io/@hbcho/%EC%BB%B4%ED%8C%8C%EC%9D%BC%EB%9F%AC%EC%9D%98-4%EB%8B%A8%EA%B3%84-%EC%9E%91%EB%8F%99-%EC%9B%90%EB%A6%AC-%EC%A0%84%EC%B2%98%EB%A6%AC%EB%B6%80%ED%84%B0-%EB%A7%81%ED%81%AC%EA%B9%8C%EC%A7%80-%EC%99%84%EB%B2%BD-%EC%9D%B4%ED%95%B4)
2. [https://modoocode.com/319](https://modoocode.com/319)
3. [https://f-lab.kr/insight/understanding-cpp-compilation-process](https://f-lab.kr/insight/understanding-cpp-compilation-process)
4. [https://lake0989.tistory.com/159](https://lake0989.tistory.com/159)
5. [https://mainmethod0126.github.io/cpp/The-Compilation-Process-of-Cpp](https://mainmethod0126.github.io/cpp/The-Compilation-Process-of-Cpp)
6. [https://sanghoon23.tistory.com/74](https://sanghoon23.tistory.com/74)
7. [https://velog.io/@seoltang/compile](https://velog.io/@seoltang/compile)
8. [https://blog.naver.com/kangmin7840/222614461690](https://blog.naver.com/kangmin7840/222614461690)
9. [https://blog.naver.com/kdr0923/50007132101](https://blog.naver.com/kdr0923/50007132101)
10. [https://hyun-soon.github.io/cpp/compile/compiler/Compile/](https://hyun-soon.github.io/cpp/compile/compiler/Compile/)