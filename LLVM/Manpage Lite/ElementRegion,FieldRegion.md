
참조:
[[MemRegion]]
## Clang Static Analyzer의 ElementRegion

**ElementRegion**은 Clang Static Analyzer의 메모리 모델에서 **배열의 특정 인덱스나 포인터 산술 연산의 결과**에 해당하는 메모리 영역을 추상적으로 나타내는 객체입니다.

주의) **배열 선언만으로 ElementRegion이 배열 요소 개수만큼 바로 생성되지 않고, 요소 접근 시점에           필요에 따라 생성됩니다.**

- 예를 들어,
 ```  cpp
   int arr[5]; arr[2] = 10; 
```

- 위 코드에서 `arr[1]`에 접근할 때, 분석기는 `arr`라는 메모리 블록 내에서 2번째 요소를 나타내는 **ElementRegion**을 생성해 해당 메모리 접근을 추적합니다.
 
- **ElementRegion**은 "상위 Region(예: 배열 전체, 포인터가 가리키는 영역)"과 "인덱스(배열의 n번째, 포인터 산술 결과)" 정보를 조합하여, 해당 요소의 메모리 위치를 추상적으로 표현합니다.

## 구조체의 특정 필드와 FieldRegion

- **구조체의 특정 필드(예: `s.x`)는 ElementRegion이 아니라 FieldRegion이라는 별도의 객체로 표현**됩니다.
    
- 예시:
```cpp
struct S { int x, y; };
S s;
s.x; // → FieldRegion
```
    
- 이때 `s.x`에 대한 메모리 영역은 **FieldRegion**이 담당합니다.
  
- 만약 구조체 내부에 배열이 있고, 그 배열의 특정 요소에 접근하는 경우에는 FieldRegion과 ElementRegion이 계층적으로 결합됩니다.  
    예: `s.arr[2]`은 FieldRegion(s.arr) 아래에 ElementRegion(3)이 붙는 구조입니다.  

## 요약

- **ElementRegion**: 배열의 특정 요소, 포인터 산술 결과 등 **복합 데이터 타입의 "인덱스 기반 부분"**에 해당하는 메모리 영역을 표현합니다.
    
- **FieldRegion**: **구조체의 특정 필드**에 해당하는 메모리 영역을 표현합니다.
    
- 구조체의 필드 접근은 FieldRegion, 배열의 요소 접근은 ElementRegion이 담당합니다.
