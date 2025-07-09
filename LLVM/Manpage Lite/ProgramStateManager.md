## ProgramStateManager란?

**ProgramStateManager**는 Clang Static Analyzer에서 프로그램 실행의 "분석 상태(ProgramState)"를 생성·복제·관리하는 핵심 클래스입니다.  
분석 과정에서 각 실행 경로(분기, 함수 호출 등)별로 독립적인 상태를 효율적으로 관리할 수 있게 해줍니다.

참조:
[[ProgramState]]
[[StoreManager]]
## 주요 역할

- **ProgramState 객체 생성 및 복제**
    
    - 분석 도중 새로운 실행 경로가 생기거나, 상태가 변경될 때 새로운 ProgramState 인스턴스를 생성합니다.
        
- **StoreManager 등 하위 관리 객체 소유**
    
    - 메모리 상태(store), 심볼 테이블, 제약 조건 등 분석에 필요한 여러 관리 객체를 직접 소유하고,  
        ProgramState가 내부적으로 이 객체들을 활용할 수 있도록 지원합니다.
        
- **상태 공유 및 메모리 효율화**
    
    - 동일한 상태를 여러 경로에서 공유할 때, 불필요한 복사를 줄이고 효율적으로 상태를 관리합니다.
        
- **상태 해시/동등성 관리**
    
    - ProgramState의 동등성 비교, 해시 계산 등 분석 엔진의 상태 캐싱과 경로 병합에 필수적인 기능을 제공합니다.
        

## 내부 구조 및 주요 멤버

|멤버/구조체|역할/설명|
|---|---|
|StoreManager*|메모리 상태(store) 관리 엔진|
|SymbolManager*|심볼릭 값 관리 엔진|
|ConstraintManager*|제약 조건(조건식, 분기 등) 관리 엔진|
|기타 매니저|다양한 분석 상태 요소(GDM, CallEventManager 등) 관리|

- **StoreManager**: 변수, 포인터, 배열 등 메모리 상태를 추적
    
- **SymbolManager**: 심볼릭 값(미지수, 추상 값) 관리
    
- **ConstraintManager**: 조건식, 분기 등 논리적 제약 조건 관리
    

## 객체 관계도 (TEXT MAP)

```scss
[ExprEngine/CoreEngine]
       ↓
[ProgramStateManager]  --owns→  [StoreManager, SymbolManager, ConstraintManager, ...]
       ↓
[ProgramState]  --- store_t(포인터) 등 다양한 상태 정보
```
## ProgramStateManager의 동작 예시

- 새로운 분기(예: if문) 발생 시:
    
    - ProgramStateManager가 기존 ProgramState를 복제하여  
        경로별로 독립적인 상태를 생성
        
- 변수 값 변경, 제약 조건 추가 등 상태 변화 시:
    
    - StoreManager, ConstraintManager 등 하위 매니저를 통해  
        ProgramState 내부 상태를 업데이트
        

## 요약

- **ProgramStateManager는 분석 상태(ProgramState) 객체의 생성·복제·관리를 총괄하는 핵심 클래스**입니다.
    
- 메모리, 심볼, 제약 조건 등 다양한 하위 매니저를 직접 소유하며,  
    분석 경로별로 독립적이고 효율적인 상태 관리를 지원합니다.
    
- 이 구조 덕분에 Clang Static Analyzer는 복잡한 실행 경로와 상태 변화를  
    정밀하게 추적·분석할 수 있습니다.