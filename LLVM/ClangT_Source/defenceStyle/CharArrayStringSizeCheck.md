[[다차원 배열 차원 해석 & AST 타입 계층]]
```cpp
int data[2][3][4]  // int 4개를 가진 배열을 3개 가진 배열을 2개 가진 배열
```
```scss
data[0]:        // 첫 번째 [3][4] 배열
  ├── [0]: int[4]  // int 4개
  ├── [1]: int[4]  // int 4개  
  └── [2]: int[4]  // int 4개

data[1]:        // 두 번째 [3][4] 배열
  ├── [0]: int[4]  // int 4개
  ├── [1]: int[4]  // int 4개
  └── [2]: int[4]  // int 4개

ConstantArrayTypeLoc[2]     ← 1회차: 가장 바깥쪽
└── ConstantArrayTypeLoc[3] ← 2회차: 중간
    └── ConstantArrayTypeLoc[4] ← 3회차: 안쪽
        └── BuiltinTypeLoc(int) ← 4회차: 가장 안쪽
```


```shell
Checks: '-*,defence-style-char-array-string-size'
CheckOptions:
  - key: defence-style-char-array-string-size.RequireExactSize
    value: 'false'   # 기본 false. true면 정확히 len+1 강제
  - key: defence-style-char-array-string-size.OfferFixIt
    value: 'true'    # [N]에서 N이 정수 리터럴일 때 자동 교체 제안
```

