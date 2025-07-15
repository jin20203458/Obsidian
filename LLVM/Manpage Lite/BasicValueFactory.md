> Clang Static Analyzer 내부에서 사용하는 **기본 값(상수 등)의 생성 및 관리 역할을 하는 유틸리티 클래스**입니다.


## 역할과 기능

- **정적 분석 중**에 다양한 타입의 상수 값(예: 0, 1, true, false 등)을 일관성 있게 생성하고 재사용할 수 있도록 도와줍니다.
    
- 예를 들어, `BVF.getValue(0, RegTy)`와 같이 사용하면, 해당 타입(`RegTy`)에 맞는 0 값을 생성해 줍니다.
    
- 이렇게 생성된 값들은 심볼릭 실행(symbolic execution)에서 조건 분기, 값 비교, 제약 조건 생성 등에 사용됩니다.
    

## 왜 필요한가?

- C/C++ 코드의 변수 타입이 다양하고, 타입에 따라 상수 값의 표현이 달라질 수 있기 때문입니다.
    
- 동일한 의미의 값(예: 0, 1)이라도 타입에 따라 내부 표현이 다르므로, 타입에 맞는 상수를 정확히 만들어야 분석이 정확해집니다.
    
- BasicValueFactory는 이러한 상수 값을 효율적으로 생성·관리하여, 분석 엔진이 빠르고 일관성 있게 동작하도록 지원합니다.

e.g
```cpp
QualType RegTy = TR->getValueType();

BasicValueFactory &BVF = C.getSValBuilder().getBasicValueFactory();
llvm::APSInt Zero = BVF.getValue(0, RegTy); // RegTy 타입에 맞는 0 생성
llvm::APSInt One = BVF.getValue(1, RegTy);  // RegTy 타입에 맞는 1 생성
```


