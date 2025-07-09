

> **ExplodedGraph**는 Clang Static Analyzer에서 프로그램의 모든 가능한 실행 경로와 그 경로별 상태 변화를 표현하는 **분석 그래프**입니다.  

참조:
[[ExprEngine,CoreEngine]]
[[ExplodedNode]]

이 그래프는 Symbolic Execution(심볼릭 실행) 과정에서 생성되며, 프로그램 내의 다양한 분기, 함수 호출, 조건문 등으로 인해 발생하는 모든 실행 경로와 그 시점의 상태를 정밀하게 추적합니다.

## ExplodedGraph의 주요 특징

- **모든 실행 경로와 상태 변화 추적**
    
    - 프로그램이 실행될 수 있는 모든 경로(분기, 반복, 함수 호출 등)를 그래프의 형태로 모델링합니다.
    - 각 경로에서 변수, 메모리, 조건 등의 상태 변화도 함께 기록합니다.
        
- **ExplodedNode의 집합**
    
    - ExplodedGraph는 수많은 **ExplodedNode**(실행 위치+상태 스냅샷)로 구성됩니다.
    - 각 ExplodedNode는 "특정 실행 위치(ProgramPoint)에서의 프로그램 상태(ProgramState)"를 결합한 노드입니다.
        
- **분기와 경로 분리**
    
    - if, switch, 함수 호출 등에서 경로가 분기될 때마다 그래프가 "폭발적으로" 확장됩니다.
    - 모든 분기점, 함수 진입/종료, 조건문 등에서 새로운 노드와 경로가 생성됩니다.

## ExplodedGraph의 구조

|구성 요소|설명|
|---|---|
|**ExplodedNode**|실행 위치(ProgramPoint) + 상태(ProgramState)를 가진 노드|
|**Edge**|한 노드에서 다음 노드로의 상태/경로 전이를 나타내는 연결선|
|**Root Node**|각 실행 경로(서브그래프)의 시작점(진입점)|
|**Subgraph**|여러 개의 분리된 실행 경로(예: 여러 함수, 스레드 등)|

- 그래프 내에서 **루트 노드(Entry Node)**는 선행 노드가 없는 노드로,  
    각 실행 경로의 시작점을 의미합니다.
    
- **분리된 서브그래프**가 여러 개 존재할 수 있으며,  
    각각이 독립적인 실행 경로를 나타냅니다.
    

## ExplodedGraph의 동작 예시

1. **프로그램 진입점에서 루트 노드 생성**
2. **분기문(if, switch 등)에서 경로가 분리되어 새로운 노드 생성**
3. **함수 호출, 반복문, 조건문 등에서 상태가 변화할 때마다 새로운 노드 추가**
4. **모든 실행 경로와 그 상태 변화가 그래프 형태로 저장됨**

## ExplodedGraph의 활용

- **버그 탐지 및 경로 추적**
    
    - 모든 실행 경로에서 발생할 수 있는 버그, 메모리 오류, 조건 위반 등을 정밀하게 탐지
        
- **분석 결과 시각화**
    
    - 각 경로별로 상태 변화와 버그 발생 위치를 시각적으로 확인 가능
        
- **정적 분석의 근간**
    
    - Clang Static Analyzer의 모든 경로 기반 분석의 핵심 데이터 구조

## 요약

- **ExplodedGraph는 "프로그램의 모든 실행 경로와 그 시점의 상태"를 정밀하게 추적하는 그래프**입니다.
    
- 각 노드는 실행 위치와 상태의 스냅샷을 의미하며,  
    모든 분기, 함수 호출, 조건문 등에서 경로가 확장됩니다.
    
- 이 구조 덕분에 Clang Static Analyzer는 복잡한 프로그램의 모든 경로와 상태 변화를 효과적으로 분석할 수 있습니다.

---
## 1. ExplodedGraph와 핵심 객체들
```scss
[CoreEngine/ExprEngine]
        |
        v
 [ExplodedGraph]
        |
   +----+----+
   |         |
[ExplodedNode] ... (여러 개)
   |         |
   v         v
[ProgramPoint]   [ProgramState]
```
## 관계 설명

- **CoreEngine/ExprEngine**
    
    - 분석의 전체 흐름을 관리하고, 실행 경로를 따라 ExplodedGraph를 생성·확장합니다.
    
- **ExplodedGraph**
    
    - 프로그램의 모든 실행 경로와 상태 변화를 추적하는 그래프 구조.
    - 내부적으로 여러 개의 **ExplodedNode**를 가집니다.
    
- **ExplodedNode**
    
    - "특정 실행 위치(ProgramPoint) + 그 시점의 상태(ProgramState)" 조합을 표현하는 노드.
    - 각 노드는 이전/다음 노드(Predecessor/Successor)와 연결되어 그래프를 형성합니다.
        
- **ProgramPoint**
    
    - 이 노드가 프로그램의 어느 위치(문장, 분기, 함수 호출 등)인지 나타냅니다.
        
- **ProgramState**
    
    - 해당 위치에서의 프로그램 상태(변수 값, 메모리, 심볼, 제약 조건 등)를 담고 있습니다.

## 2. ExplodedGraph와 외부 객체의 상호작용

```scss
[Checker(체커)]
      ^
      |
[ExprEngine] <----> [ExplodedGraph]
      |
      v
[ProgramStateManager]
```
- **ExprEngine**은 ExplodedGraph를 사용해 경로별 상태 변화를 추적하고, 체커(Checker)와 상호작용합니다.
- **Checker**는 ExplodedNode, ProgramState, ProgramPoint 등의 정보를 활용해 버그 탐지 및 리포트를 생성합니다.
- **ProgramStateManager**는 각 노드의 상태(ProgramState) 생성·복제·관리를 담당합니다.


## 3. ExplodedGraph의 내부 연결 구조

```scss
[ExplodedGraph]
   |
   +-- [ExplodedNode #1] <--> [ExplodedNode #2] <--> [ExplodedNode #3] ...
           |                       |
   [ProgramPoint]           [ProgramPoint]
   [ProgramState]           [ProgramState]
```

- 각 **ExplodedNode**는 여러 개의 predecessor(이전 노드)와 successor(다음 노드)를 가질 수 있습니다.
- 분기, 반복, 함수 호출 등으로 경로가 분리·합쳐지며, 그래프가 "폭발적으로" 확장됩니다.