
> - **DeadStoresChecker**는 "더 이상 사용되지 않는 변수(=dead variable)에 값을 저장(=store)하는 코드"를 찾는 **정적분석기(Clang Static Analyzer)용 플로우 센서티브(Flow-sensitive) 체커**입니다.
    
- 즉, **읽히지 않을 값(Dead value)이 저장된 곳**을 자동으로 경고해줍니다.


## 예시

### (1) 읽히지 않는 변수 대입

```cpp
void f() {
    int a = 0;    // dead store!
    a = 10;       // a=0은 사용되기 전에 덮어써짐
    printf("%d\n", a);
}
```
- `a = 0`은 읽히지 않고, 곧바로 `a = 10`으로 덮어써져서 **dead store**.
### (2) 사용되지 않는 초기화

```cpp
void g() {
    int b = 1;    // dead store!
    // b는 이후에 전혀 쓰이지 않음
}
```
- `b = 1`도 읽히지 않고 소멸.
### (3) self-assignment, null assignment 등은 **예외적으로 무시**됨
```cpp
void h() {
    int* p = nullptr; // dead store 무시 (디펜시브 프로그래밍)
    p = new int(5);
}
```
- 포인터에 null 대입, self-assignment 등은 일부러 경고를 안 내기도 함.



## DeadStoresChecker 논리 흐름도
```scss
[함수/블록 진입]
      │
      ▼
[CFG(Control Flow Graph) 생성]
      │
      ▼
[LiveVariables(생존 변수) 분석]
      │
      ▼
[모든 대입(할당, 초기화) 코드 탐색]
      │
      ▼
[대입문/초기화문 발견]
      │
      ▼
 ┌───────────────────────────────┐
 │    예외 상황(무시)인가?       │
 │   (예: null 대입, self-assign)│
 └────────────┬──────────────────┘
              │
          [예]│────→ [무시/분석X]
              │
              ▼
           [아니오]
              │
              ▼
[이 변수 값이 "live"(읽힐 예정)인가?]
      │
   ┌──┴─────┐
   │        │
[아니오]   [예]
   │        │
   ▼        │
[dead store │
경고 리포트]│
   │        │
   ▼        ▼
[다음 대입문/초기화문 반복]
      │
      ▼
[함수/블록 종료]
```
