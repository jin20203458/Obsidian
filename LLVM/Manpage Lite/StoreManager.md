
> **StoreManager**는 Clang Static Analyzer에서 "store"(가상 메모리 맵)의 **구현과 관리**를 담당하는 핵심 추상 클래스입니다.
> 

참조:
[[Store]]
[[ProgramStateManager]]

즉, **프로그램의 메모리 상태(Region ↔ SVal 매핑)를 실제로 어떻게 저장·조회·업데이트할지**를 결정하는 엔진 역할을 합니다.
##  예시 다이어그램 (상세)
```scss
[ExprEngine/CoreEngine]
       |
       v
[ProgramStateManager]
       |
       +--[StoreManager] (RegionStoreManager 등)
       |
       v
[ProgramState]
       |
       +-- store_t (포인터)
               |
               +-- [RegionBindings]
                       |
                       +-- MemRegion* ↔ SVal (매핑)

```

- **StoreManager**는 store(가상 메모리 맵)의 생성·조작·관리를 담당하는 엔진 역할
- **ProgramStateManager**가 StoreManager를 직접 소유
- **ProgramState**가 각 분석 경로별로 store_t(포인터)를 직접 소유
- **store_t*_는 RegionBindings를 통해 MemRegion_ ↔ SVal 매핑을 내부적으로 관리
    

이 구조는 Clang Static Analyzer의 공식 소스 코드와 문서에서 확인할 수 있는 정확한 내부 구조입니다.