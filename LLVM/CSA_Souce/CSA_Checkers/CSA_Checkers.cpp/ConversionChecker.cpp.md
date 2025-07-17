> 정수 및 부동소수점 타입 간의 암시적 형변환(implicit conversion)에서 발생할 수 있는 **부호 손실(sign loss)** 및 **정밀도 손실**(precision loss)을 탐지하는 경고용 체크입니다.
> 이 체커는 **경로 감지 기반**(path-sensitive) 분석을 수행하여 아래와 같은 경우를 잡아냅니다:



- **체커 이름:** `ConversionChecker`

- **분석 시점:** `check::PreStmt<ImplicitCastExpr>`

- **체크 목적:**
    - 음수를 부호 없는 타입으로 변환 → 부호 손실
    - 정수 → 부동소수점 또는 작은 정수 타입 → 정밀도 손실

- **체크 위치:**
    
    - 할당(`=`), 복합 대입(`+=`, `*=` 등), 비교 연산(`>`, `<`, `==`, ...) 및 반환문 등에서 `ImplicitCastExpr`가 사용될 때
 
---

### 체커가 경고하는 조건 (예시 포함)

#### 1. **부호 손실 (Loss of sign)**

- 음수(signed int)가 **unsigned int**로 암시적으로 캐스팅될 때:
```cpp
int a = -1;
unsigned int b = a; // 🚨 부호 손실 경고
```

- 곱셈에서 unsigned로 암시적 변환되는 경우:
```cpp
int a = -5;
unsigned int b = 3;
unsigned int c = a * b; // 🚨 부호 손실 경고
```

#### 2. **정밀도 손실 (Loss of precision)**

- 큰 정수 → 부동소수점 형변환 시:
```cpp
unsigned long long big = 1ULL << 55;
float f = big; // 🚨 정밀도 손실 경고 (float는 24비트 정밀도)
```

- 큰 정수 → 더 작은 정수형으로 변환될 때:
```cpp
unsigned int a = 300;
unsigned char b = a; // 🚨 정밀도 손실 (255 이상 표현 불가)
```

---
### **주요 분석 흐름**

1. `ImplicitCastExpr`가 감지되면 `checkPreStmt`가 호출됨.

2. `ParentMap`을 통해 부모 노드를 확인:
    
    - `BinaryOperator` (대입, 산술 연산 등)
    - `DeclStmt`, `ReturnStmt` (초기화 또는 반환)

3. `isLossOfSign(...)`, `isLossOfPrecision(...)` 로 부호 또는 정밀도 손실 여부 판단.

4. 손실이 확인되면 `generateNonFatalErrorNode()` 후 `reportBug(...)` 호출로 경고 리포트 생성.

```scss
[Start: checkPreStmt 호출]
          |
          v
[Cast 대상 타입이 bool인가?] ---> 예 ---> [종료]
          |
          아니오
          |
          v
[매크로 내부에서 발생한 변환인가?] ---> 예 ---> [종료]
          |
          아니오
          |
          v
[부모 노드(Parent) 찾기] 
          |
          v
[Parent가 ExplicitCastExpr인가?] ---> 예 ---> [종료]
          |
          아니오
          |
          v
[Parent 종류 분기]
    ├─ BinaryOperator
    │     ├─ BO_Assign
    │     ├─ BO_AddAssign / BO_SubAssign
    │     ├─ BO_MulAssign / BO_DivAssign ...
    │     └─ 비교 연산자 (>, < 등)
    ├─ DeclStmt
    ├─ ReturnStmt
    └─ 기타
          |
          v
[isLossOfSign 호출] + [isLossOfPrecision 호출]
          |
          v
[부호 손실 또는 정밀도 손실 존재?]
          |
         예
          |
          v
[generateNonFatalErrorNode 호출]
          |
          v
[PathSensitiveBugReport 생성 및 emitReport]
          |
          v
[종료]
```