
## 배열 타입 계층구조

```scss
Type (기본 클래스)
├── ArrayType (배열 기본 클래스)
│   ├── ConstantArrayType      // [N] 크기 명시 배열
│   ├── IncompleteArrayType    // [] 크기 생략 배열
│   └── VariableArrayType      // VLA (Variable Length Array)
└── BuiltinType               // int, char 등 기본 타입
```

## 실제 저장 구조


**예시 1:** `int arr`

```cpp
int arr[10];
```

**AST 저장 방식:**
```scss
VarDecl "arr"
└── QualType: ConstantArrayType
    ├── 크기 정보: 10
    └── 원소 타입: BuiltinType(int)
```


---

**예시 2:** `char buffer`

```cpp
char buffer[50] = "Hello";
```

**AST 저장 방식:**

```scss
VarDecl "buffer"  
└── QualType: ConstantArrayType
    ├── 크기 정보: 50
    └── 원소 타입: BuiltinType(char)
```

---

## 왜 이런 구조인가?

## 1. **배열은 복합 타입**

- **단순 타입**: `int`, `char` → `BuiltinType`
    
- **복합 타입**: `int[]`, `char*` → `ArrayType`, `PointerType`
    

## 2. **추가 정보 필요**

```cpp
// 단순히 int로는 표현 불가능한 정보들
int arr[10];    // 크기: 10
int arr[];      // 크기: 미지정  
int arr[SIZE];  // 크기: 변수
```

## 3. **타입 안전성**

```cpp
int arr1[10];
int arr2[20];
// arr1과 arr2는 다른 타입! (크기가 다름)
```


## CharArrayStringSizeCheck에서의 활용

## 타입 추출 과정

```cpp
// 1. 전체 타입 정보 가져오기
QualType QT = VD->getType();              // ConstantArrayType[50]

// 2. 배열 타입으로 캐스팅
const auto *CAT = dyn_cast<ConstantArrayType>(QT.getTypePtr());

// 3. 배열 정보 추출
uint64_t size = CAT->getSize().getZExtValue();     // 50
QualType elemType = CAT->getElementType();         // char
```

## 타입 계층 접근

```cpp
char buffer[100];
```
```scss
 QT.getTypePtr() → ConstantArrayType 객체
 ├── getSize() → 100 
 └── getElementType() → char
```

## 다차원 배열의 경우
[[다차원 배열 차원 해석 & AST 타입 계층]]
**예시:** `int matrix`

```cpp
int matrix[5][10];
```

**AST 구조:**

```scss
VarDecl "matrix"
└── QualType: ConstantArrayType[5]
    └── 원소 타입: ConstantArrayType[10]  
        └── 원소 타입: BuiltinType(int)
```

**각 레벨별 타입:**

- 최상위: `ConstantArrayType` (크기=5, 원소=`ConstantArrayType`)
    
- 중간: `ConstantArrayType` (크기=10, 원소=`int`)
    
- 최하위: `BuiltinType(int)`
    

## 핵심 정리

|선언|저장되는 타입|원소 타입|
|---|---|---|
|`int x`|`BuiltinType(int)`|-|
|`int arr`|`ConstantArrayType`|`int`|
|`int arr[]`|`IncompleteArrayType`|`int`|
|`int* ptr`|`PointerType`|`int`|

**배열은 원소 타입을 포함하면서 크기 정보를 추가로 가진 `ConstantArrayType` 등의 복합 타입으로 저장됩니다. 이를 통해 타입 시스템에서 배열의 크기까지 타입의 일부로 취급할 수 있게 됩니다.**[clang.llvm](https://clang.llvm.org/doxygen/classclang_1_1ConstantArrayType.html)
