참조:
[[ProgramPoint]]
[[AnalysisDeclContext]]

**LocationContext**는 Clang Static Analyzer에서 프로그램의 실행 위치(컨텍스트)를 표현하는 핵심 클래스입니다.  
이 컨텍스트는 단순히 소스코드의 한 지점만을 나타내는 것이 아니라, **함수 호출, 블록 진입, 임시 객체 생성 등 다양한 실행 스택의 계층 구조**까지 포함합니다.

## 주요 멤버 변수 및 구조


| 멤버/구조                           | 설명                                                   |
| ------------------------------- | ---------------------------------------------------- |
| `const LocationContext *Parent` | 상위(부모) LocationContext를 가리키는 포인터. 계층 구조의 핵심.         |
| `AnalysisDeclContext *ADC`      | 이 컨텍스트에 해당하는 함수/블록의 AST 및 분석 정보.                     |
| `Kind`                          | 컨텍스트 종류(enum): StackFrame, BlockInvocation, Scope 등. |
| `unsigned Id`                   | 각 LocationContext의 고유 식별자.                           |

## 계층 구조 예시

- **StackFrameContext**: 함수 호출 시 생성, Parent는 호출한 함수의 LocationContex
## 계층 구조 예시

```scss
// 예시: main() → foo() → 람다 → 임시 객체 생성
StackFrameContext(main)         // Parent: nullptr
  └─ StackFrameContext(foo)     // Parent: main의 Context
      └─ BlockInvocationContext // Parent: foo의 Context
          └─ ScopeContext       // Parent: 람다의 Context
```


## 주요 목적

- **프로그램 실행 흐름의 컨텍스트 추적**  
    함수 호출, 블록 진입, 임시 객체 생성 등 실행 경로의 "위치"를 계층적으로 추적합니다.
    
- **분석의 정확성 향상**  
    동일한 코드라도 호출 스택이나 컨텍스트에 따라 상태가 달라질 수 있으므로, 각 컨텍스트별로 별도의 분석 상태를 유지할 수 있습니다.
    

## 구조 및 종류

LocationContext는 추상 클래스이며, 실제로는 다음과 같은 하위 클래스로 구체화됩니다.

|하위 클래스|설명|
|---|---|
|StackFrameContext|함수 호출 시 생성. 호출 스택 프레임 하나를 나타냄.|
|BlockInvocationContext|블록(예: 람다, Objective-C 블록) 호출 시 생성.|
|ScopeContext|임시 객체, 지역 변수 등 특정 스코프에 진입할 때 생성.|

각 LocationContext 객체는 **부모 LocationContext**를 가리키는 포인터를 갖고 있어,  
현재 실행 위치에서 상위 호출 스택까지 계층적으로 추적할 수 있습니다.

## 사용 예시

- **함수 호출 분석**  
    함수 A에서 함수 B를 호출하면, 새로운 StackFrameContext가 생성되어  
    "A의 호출 컨텍스트 안에서 B가 실행 중"임을 명확하게 추적할 수 있습니다.
    
- **임시 객체/스코프 관리**  
    임시 객체 생성, 지역 변수의 생명주기 등도 ScopeContext로 구분하여  
    각 컨텍스트별로 메모리 상태, 변수 상태를 독립적으로 관리합니다.
    

## 분석에서의 의미

- **ExplodedGraph의 노드는 항상 LocationContext와 연결**  
    즉, 같은 소스 코드라도 서로 다른 LocationContext(예: 서로 다른 함수 호출 스택)라면  
    별개의 노드와 상태로 관리됩니다.
    
- **버그 리포트의 경로 추적**  
    버그 경로를 출력할 때, LocationContext를 따라가면  
    "어떤 함수 호출 스택에서 문제가 발생했는지"를 정확히 알 수 있습니다.
    

## 요약

- **LocationContext**는 "현재 분석 중인 실행 위치와 호출 스택"을 계층적으로 추적하는 클래스입니다.
    
- 함수 호출, 블록 진입, 임시 객체 생성 등 다양한 실행 컨텍스트를 구분하여  
    분석의 정확성을 높이고, 상태를 세밀하게 관리할 수 있게 해줍니다.
    

궁금한 부분(예: StackFrameContext의 내부 구조, 실제 사용 예 등)이 더 있다면 추가로 설명해드릴 수 있습니다.

1. [https://velog.io/@thdalwh3867/LLVM-%EC%BB%B4%ED%8C%8C%EC%9D%BC%EB%9F%AC%EC%9D%98-%ED%99%9C%EC%9A%A9](https://velog.io/@thdalwh3867/LLVM-%EC%BB%B4%ED%8C%8C%EC%9D%BC%EB%9F%AC%EC%9D%98-%ED%99%9C%EC%9A%A9)