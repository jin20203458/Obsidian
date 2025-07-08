
**참조:**
[[CSA 주요 객체들 간의 구조도 및 관계]]
[[ASTContext]]
[[CFG]]
[[AnalysisDeclContext]]

## 예시 C 코드
```c
int foo(int x) 
{
  int y = 0;
  if (x > 10)   y = x;
  else          y = -x;
  
  return y;
}
```

## CFG 구조 (텍스트 트리)
```scss
[CFGBlock #1] (Entry 블록)
  └─ CFGElement: int y = 0;

[CFGBlock #2] (if 조건 블록)
  └─ CFGElement: if (x > 10)

[CFGBlock #3] (then 블록: x > 10일 때)
  └─ CFGElement: y = x;

[CFGBlock #4] (else 블록: x <= 10일 때)
  └─ CFGElement: y = -x;

[CFGBlock #5] (공통 후속 블록)
  └─ CFGElement: return y;

```

## 시각적 흐름(방향성 포함)
```scss
[Entry]
  │
  ▼
[if 조건]
  ├─(참)─► [then] ──┐
  │                │
  └─(거짓)► [else] ─┘
         │
         ▼
      [return]
```
## 1. CFGBlock이란?

- **CFGBlock**은 함수의 실행 흐름을 나누는 **하나의 코드 블록**(노드)입니다.
    
- 예를 들어, if문, for문, 함수 본문, 조건 분기 등 **실행 경로의 한 구간**을 나타냅니다.
    
- 여러 CFGBlock이 연결되어 전체 함수의 **Control Flow Graph**(제어 흐름 그래프, CFG)를 구성합니다.
- 내부 구조는 CFGElement를 가지고 있는 동적배열

## 예시

- 함수 본문 전체가 하나의 블록
    
- if문의 then/else 각각이 별도의 블록
    
- 반복문의 본문, 조건 등도 각각 블록
    

## 2. CFGElement란?

- **CFGElement**는 **CFGBlock 안에 들어가는 개별 요소**입니다.
    
- 실제로는 한 줄의 코드(문장, 조건, 선언 등) 또는 특정 연산을 의미합니다.
    
- 즉, **CFGBlock을 구성하는 작은 단위**입니다.

```cpp
// clang/include/clang/Analysis/CFG.h

class CFGElement {
public:
    // 어떤 종류의 CFG 요소인가?
    enum Kind {
        Statement,           // 일반 문장(Expr, Stmt 등) → CFGStmt로 저장
        AutomaticObjDtor,    // 자동 소멸자 → CFGAutomaticObjDtor
        ImplicitDtor,        // 암시적 소멸자 등
        Initializer,         // 멤버 초기화 등
        ScopeBegin, ScopeEnd // 스코프 진입/종료 등
        // ... 기타 여러 종류
    };

    Kind getKind() const;

    // 타입별 안전한 다운캐스팅 (variant 느낌)
    template <typename T>
    const T *getAs() const;

    // 구체적 데이터 저장 (내부적으로 union 구조체 또는 포인터)
private:
    Kind MyKind;
    void *Data; // 실제 데이터(Statement*, Dtor*, Initializer*, ...)의 포인터
};

```