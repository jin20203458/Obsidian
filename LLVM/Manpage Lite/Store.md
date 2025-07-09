> **store**는 Clang Static Analyzer에서 프로그램의 "가상 메모리 상태"를 표현하는 핵심 객체입니다. 


- 프로그램의 각 변수, 포인터, 배열, 구조체 멤버 등 **모든 메모리 지역(MemRegion)**에 대해  
  "현재 이 지역에 어떤 값(SVal)이 저장되어 있는가?"를 추적·관리합니다

참조:
[[StoreManager]]

## 주요 역할

- **메모리 지역(MemRegion) ↔ 값(SVal) 매핑**
    
    - 변수, 포인터, 배열, 구조체 멤버 등 모든 메모리 지역을 키로 하여,  
        그 지역의 현재 값을 SVal로 저장합니다.
        
- **분석 경로별 독립적 메모리 상태 유지**
    
    - 분기(if, switch 등)마다 store의 복사본이 생성되어  
        경로별로 독립적인 메모리 상태를 추적할 수 있습니다.
        
- **정적 분석의 기반**
    
    - 변수 추적, 포인터 역참조, 메모리 오류 탐지 등  
        모든 상태 추적의 기반이 됩니다.
        

## 클래스 구조도

아래는 store와 관련된 주요 클래스 구조입니다.

```scss
ProgramState
   └── Store (store_t)
         ├── RegionBindings (실제 매핑 구현)
         │     └── Map<MemRegion*, SVal>
         └── StoreManager (구현체: RegionStoreManager 등)
```

|클래스/구조체|역할/설명|
|---|---|
|**ProgramState**|분석 경로의 전체 상태(메모리, 심볼, 경로 등)|
|**store_t (Store)**|메모리 상태(Region ↔ SVal 매핑)의 핵심 객체|
|**RegionBindings**|실제 매핑(사전) 구현체|
|**MemRegion**|변수, 포인터, 배열, 구조체 멤버 등 "메모리 지역" 표현|
|**SVal**|해당 지역에 저장된 값(정수, 포인터, 심볼릭 등)|
|**StoreManager**|store의 생성·조회·업데이트 등 관리 로직|

## 객체 관계도

아래는 store 객체와 관련 주요 객체 간의 관계를 도식화한 예시입니다.

```scss
[ProgramState]
     |
     v
  [store_t]
     |
     v
[RegionBindings]
     |
     v
+---------------------+
| MemRegion* | SVal   |
+---------------------+
|   x의 Region | 42   |  // int x = 42;
|   p의 Region | Loc  |  // int* p = &x;
|   arr[0]    | 1    |  // int arr[0] = 1;
+---------------------+
```

- **ProgramState**는 분석 경로의 전체 상태를 관리하며,  
    그 중 store_t가 메모리 상태(Region ↔ SVal 매핑)를 담당합니다.
    
- **RegionBindings**는 실제 매핑(사전) 역할을 하며,  
    각 MemRegion(메모리 지역)마다 SVal(값)을 저장합니다.
    
- **StoreManager**는 store 객체의 생성, 복사, 업데이트 등 내부 동작을 담당합니다.
    

## 예시로 보는 동작

- `int x = 42;`
    
    - x의 MemRegion → NonLoc SVal(42)
        
- `int *p = &x;`
    
    - p의 MemRegion → Loc SVal(x의 MemRegion)
        
- `arr = 1;`
    
    - arr의 MemRegion → NonLoc SVal(1)
        

## 요약

- **store 객체는 "메모리 지역(MemRegion) ↔ 값(SVal)" 매핑을 관리하는 가상 메모리 테이블**입니다.
    
- 분석 경로별로 독립적으로 존재하며,  
    변수, 포인터, 배열, 구조체 등 모든 메모리 상태를 추적합니다.
    
- Clang Static Analyzer의 정적 분석, 버그 탐지, 경로 추적의 핵심 기반이 되는 구조입니다.