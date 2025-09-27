
### 1️⃣ 선언부 예시

```cpp
int data[2][3][4];  
// int 4개를 가진 배열을 3개 가진 배열을 2개 가진 배열
```

---

### 2️⃣ 메모리 구조 해석

|차원 인덱스|배열 크기|의미|
|---|---|---|
|data[0] ~ data[1]|[3][4]|각 요소가 [3][4]짜리 2차원 배열|
|data[i][0] ~ data[i][2]|[4]|각 요소가 int 4개를 가진 1차원 배열|
|data[i][j][0] ~ data[i][j][3]|int|실제 값 저장 공간|

**시각화**

```scss
data
├── data[0] ─┬─ [0] → int[4]
│            ├─ [1] → int[4]
│            └─ [2] → int[4]
└── data[1] ─┬─ [0] → int[4]
             ├─ [1] → int[4]
             └─ [2] → int[4]
```

---

### 3️⃣ AST 타입 계층 (Clang 예시)

|레벨|타입 노드|설명|
|---|---|---|
|1|`ConstantArrayTypeLoc[2]`|바깥쪽 차원 (크기 2)|
|2|`ConstantArrayTypeLoc[3]`|중간 차원 (크기 3)|
|3|`ConstantArrayTypeLoc[4]`|안쪽 차원 (크기 4)|
|4|`BuiltinTypeLoc(int)`|실제 기본 타입(int)|

**계층 시각화**

```scss
ConstantArrayTypeLoc[2]     // 1회차: 가장 바깥
└── ConstantArrayTypeLoc[3] // 2회차: 중간
    └── ConstantArrayTypeLoc[4] // 3회차: 안쪽
        └── BuiltinTypeLoc(int) // 4회차: 실제 타입
```


---

### 4️⃣ 핵심 정리

- 다차원 배열은 **가장 안쪽 차원부터 기본 타입을 감싸면서** 바깥으로 확장됨.
- AST 분석 시 `ConstantArrayTypeLoc`이 **차원 수만큼 중첩**되고, 마지막에 `BuiltinTypeLoc`이 위치.
- 메모리 상에서는 **연속된 1차원 블록**으로 저장되지만, 소스 코드와 AST는 **논리적 차원 구조**를 보존.
