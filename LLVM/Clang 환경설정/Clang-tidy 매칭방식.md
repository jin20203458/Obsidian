
## 기본 원칙: ALL-OR-NOTHING

```cpp
callExpr(
  hasArgument(0, stringLiteral()),     // 조건1: 첫 번째 인자가 문자열 리터럴
  argumentCountIs(2),                  // 조건2: 인자가 정확히 2개
  callee(functionDecl(hasName("printf"))) // 조건3: 함수명이 "printf"
)
```
→ **3개 조건 모두 만족**해야만 매칭됨

## 예외: 조건부/선택적 매칭

## **`unless()` - 부정 조건**

```cpp
callExpr(
  callee(functionDecl(hasName("printf"))),
  unless(isExpansionInSystemHeader())     // 시스템 헤더가 아닌 경우만
)
```


## **`anyOf()` - OR 조건**

```cpp
callExpr(
  callee(functionDecl(
    anyOf(                                // 이 중 하나라도 만족하면
      hasName("printf"),
      hasName("fprintf"), 
      hasName("sprintf")
    )
  ))
)
```


## **`allOf()` - 명시적 AND (기본값)**

```cpp
callExpr(
  callee(functionDecl(
    allOf(                                // 모두 만족해야 함 (기본 동작과 동일)
      hasName("printf"),
      isVariadic()
    )
  ))
)
```


## **`eachOf()` - 각각 매칭**

```cpp
// 여러 패턴을 각각 검사
eachOf(
  callExpr(callee(functionDecl(hasName("printf")))),
  callExpr(callee(functionDecl(hasName("scanf"))))
)
```

## 실제 활용 예시

```cpp
// 복잡한 조건 조합
F->addMatcher(
  callExpr(
    // printf 계열 함수들 중 하나
    callee(functionDecl(
      anyOf(
        hasName("printf"),
        hasName("fprintf"),
        hasName("sprintf")
      )
    )),
    // 시스템 헤더는 제외
    unless(isExpansionInSystemHeader()),
    // 최소 1개 인자 필요
    argumentCountAtLeast(1)
  ).bind("call"),
  this
);
```

**요약**:

- **기본 = ALL-OR-NOTHING** (모든 조건 만족 필요)
    
- **`unless()`, `anyOf()`, `allOf()`** 등으로 조건부 로직 구현 가능
    
- 매처가 실패하면 **전체 바인딩 실패** → `check()` 호출 안 됨
    
