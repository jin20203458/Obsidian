
---
### 자료형

```cpp
class SymbolReaper

//e.g
SymReaper.isDead(Sym)          // 심볼이 죽었는지 판단. 
SymReaper.isLive(Sym)          // 심볼이 살았는지 판단.
SymReaper.isLiveRegion(Region) // 특정 메모리 영역이 아직 살았는지 판단.
```

>심볼이 "죽었는지"를 판단하는 가장 표준적인 도구
- **SymbolReaper**는 Clang Static Analyzer에서 심볼(변수, 포인터 등)이 "살아있는지(live)" 또는 "죽었는지(dead)"를 추적/관리하는 유틸리티 클래스입니다.
- 주로 checkDeadSymbols 콜백에서, 리소스 추적이나 누수 탐지 등에서 사용됩니다.



```cpp
template <typename T> 
class FoldingSet;

//주요 메서드 :
T* GetOrInsertNode(T* N);        // 중복 체크 후 객체 반환
bool RemoveNode(T* N);           // 객체 삭제
void reserve(unsigned EltCount); // 용량 예약
```
>**중복 없는 객체 저장소**
- `FoldingSet<T>`는 **중복을 제거하면서 객체를 저장**하는 고성능 해시 기반 컨테이너입니다.
- `T`는 반드시 `FoldingSetNode`를 상속하고, `Profile()` 함수를 구현해야 합니다.



```cpp
class FoldingSetNodeID

// 예시
void StreamState::Profile(FoldingSetNodeID &ID) const {
  ID.AddInteger(K); // 상태값(K)을 ID에 추가
}

//주요 메서드 :
void AddPointer(const void* Ptr); // 포인터 추가 
void AddInteger(unsigned I);      // 정수값 추가 
void AddString(StringRef Str);    // 문자열 추가 
unsigned ComputeHash() const;     // 최종 해시 계산
```
>**객체의 고유 ID 생성기**
- `FoldingSetNodeID`는 객체의 **고유한 특성들을 모아 해시 ID를 생성**하는 클래스입니다.
- `Profile()` 함수 안에서 이 객체를 사용해 **객체의 정체성을 정의**합니다.



```cpp
class CallEvent

// 예시
if (Call.isGlobalCFunction("fopen")) 
{ SymbolRef FileDesc = Call.getReturnValue().getAsSymbol(); }

// 기본 정보
unsigned getNumArgs() const; // 인자 개수 
SVal getArgSVal(unsigned Idx) const; // 인자의 심볼릭 값 
QualType getResultType() const; // 반환 타입 

// 호출 분석 
bool isGlobalCFunction() const; // C 표준 함수 판별 
RuntimeDefinition getRuntimeDefinition() const;// 실행시 정의 

// 소스 코드 추적 
SourceRange getSourceRange() const; // 호출 위치 범위
```
>**함수 호출에 대한 정보 묶음**
- `CallEvent`는 분석기가 심볼릭 실행을 하다가 마주친 **함수 호출 이벤트에 대한 모든 정보를 담고 있는 객체**입니다.
- 단순히 AST(추상 구문 트리)의 함수 호출 노드(`CallExpr`)를 넘어, 특정 분석 경로상의 문맥을 포함하는 더 풍부한 정보를 제공합니다.
- 분석 대상 코드의 `fopen()`이나 `fclose()` 같은 호출을 추적할 때 이 객체로 다룹니다.



```cpp
class CheckerContext

// 예시
ProgramStateRef State = C.getState(); 
ExplodedNode* N = C.addTransition(State);

// 상태 접근
ProgramStateRef getState() const;
const LocationContext* getLocationContext() const;

// 경로 제어
ExplodedNode* addTransition(ProgramStateRef State);
ExplodedNode* generateSink();  // 경로 종료

// 오류 보고
void emitReport(BugReport* R);
```
>**현재 분석 상황**
- `CheckerContext`는 체커가 현재 어떤 **상태**(ProgramState)에서, 어떤 **위치**(ProgramPoint)에서 동작 중인지 알려주는 **컨텍스트 객체**입니다.
- Clang Static Analyzer에서 **현재 분석 중인 경로의 상태(**`ProgramState`**)에 접근할 수 있게 해주는 관문**
- 체커가 **경로**(graph)를 확장하거나 상태를 바꿀 때 사용하는 도구입니다.



```cpp
BugReport *R = new BugReport( 
*DoubleCloseBugType,                       // 버그 유형 객체
"Closing a previously closed file stream", // 설명 메시지 
ErrNode                                    // 오류 발생 노드 
);

// 정보 추가
void addRange(SourceRange R);       // 소스 범위 하이라이트
void markInteresting(SymbolRef Sym); // 관련 심볼 강조

// 속성 설정
bool markInvalidatable();  // 리소스 무효화 가능 표시
void markDead();           // 죽은 경로 표시
```
>**CSA가 탐지한 버그(오류) 하나하나를 구체적으로 기록·표현하는 핵심 클래스**
- `BugReport`는 **특정 버그 인스턴스(오류 발생 위치, 원인, 경로 등)를 캡슐화**하는 객체
- 버그의 종류(타입), 설명, 소스 코드 위치, 관련 변수/심볼, 경로 등 **버그 리포트에 필요한 모든 정보**를 저장합니다.
- 분석기가 리포트를 생성할 때 이 객체를 기반으로 텍스트/HTML/웹 리포트를 만듭니다



```cpp
class ConstraintManager

// 예시
ConstraintManager &CMgr = State->getConstraintManager(); 
ConditionTruthVal OpenFailed = CMgr.isNull(State, Sym);
```
>**변수의 값이 어떤 "조건" 또는 "제약(Constraint)"을 가지는지를 추적하는 컴포넌트**

|항목|설명|
|---|---|
|역할|심볼(SymbolRef)이 가질 수 있는 **값의 조건 (Constraints)**을 추적|
|사용처|`isNull`, `assumeSymEQ`, `assumeSymNE`, `assumeSymLT`, …|
|예|어떤 경로에서는 `F != NULL`, 다른 경로에서는 `F == NULL`일 수도 있음 → 각각 조건 추론|

```cpp
class ConditionTruthVal

//예시
ConstraintManager &CMgr = State->getConstraintManager(); 
ConditionTruthVal OpenFailed = CMgr.isNull(State, Sym);
```
>CSA에서 **조건(Condition)이 참인지, 거짓인지, 또는 확실하지 않은지를 표현하기 위한 특수한 타입**

|상태|의미|관련 함수|
|---|---|---|
|`isConstrainedTrue()`|**항상 참**인 조건 (예: `ptr == NULL` 확정)|조건은 반드시 성립함|
|`isConstrainedFalse()`|**항상 거짓**인 조건 (예: `ptr != NULL` 확정)|조건은 절대 성립하지 않음|
|`isUnderconstrained()` 또는 두 함수 모두 false|**확실하지 않음**|조건이 참일 수도, 거짓일 수도 있음|


---
### 함수

```cpp
T *FindNodeOrInsertPos(const FoldingSetNodeID &ID,                           
					          void *&InsertPos);
```
- ID :객체의 고유 프로파일 정보. 객체에 대해 `Profile()`을 호출해 만들어진 ID입니다.
- InsertPos : 삽입 위치를 담아오는 참조 포인터입니다. 이미 존재하면 무시되고, 존재하지 않으면 이 위치에 삽입하게 됩니다.

 반환 타입 : T*

- `T`는 `FoldingSet<T>`에 저장된 객체 타입입니다.
- 이 함수는 **동일한 Profile 정보를 가진 기존 객체가 있다면 그 객체의 포인터를 반환**합니다.
- 없다면 `nullptr`을 반환하고, 삽입을 위한 `InsertPos` 위치만 채워줘요.


```cpp
checkerContext.generateSink();
```
- **오류 발생 지점에서 현재 경로를 종료**시키는 노드를 생성합니다.
- 이 노드는 `ExplodedGraph` 상에서 해당 경로의 끝점 역할을 하며, 이후 이 경로에 대한 추가 분석은 중단됩니다.
- 같은 오류 상태가 이미 존재하면 중복 생성을 방지하기 위해 `nullptr`을 반환합니다.


```cpp
checkerContext.addTransition(State);
```
- **새로운 상태(State)로 일반적인 분석 경로 노드(transition)를 생성**합니다.
- 이 노드는 `ExplodedGraph` 상에서 "정상적인 경로 확장" 역할만 하며, 특별한 의미(에러/경고)는 없습니다.
- 해당 경로의 추가 분석이 **계속 진행**됩니다.
- 완전히 동일한 상태(프로그램 위치, 상태, 태그 등)가 이미 존재하면 **중복 생성하지 않고 기존 노드를 재활용**합니다.
- **주로 단순 상태 갱신, 리포트 없는 전이**에 사용합니다.



---

### 매크로

```cpp
REGISTER_MAP_WITH_PROGRAMSTATE(StreamMap, SymbolRef, StreamState)
```
> **Clang Static Analyzer**에서 사용되는 매크로로, 체커(Checkers)들이 프로그램 상태(`ProgramState`)에 **키-값 쌍 형태의 정보를 저장할 수 있도록 맵 타입을 정의**

위의 매크로는 다음과 같은 형태의 맵을 정의합니다:

> `StreamMap : ProgramState의 일부로써 SymbolRef → StreamState로 매핑하는 저장 공간`

즉, 각 파일 핸들(`SymbolRef`)에 대해 현재 상태(`StreamState` — 예: Opened, Closed 등)를 추적하는 용도로 쓰이죠.
- **불변(immutable)한 키-값 맵 자료구조**



