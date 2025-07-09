
참조:
[[LocationContext]]

**ProgramPoint**는 Clang Static Analyzer에서 프로그램의 “아주 세밀한 실행 위치”를 표현하는 핵심 클래스입니다.  
아래는 C++ 코드 스타일로 내부 구조와 각 멤버의 의미를 한글 주석과 함께 설명합니다.

## ProgramPoint 내부 구조 예시

```cpp
// ProgramPoint: 코드 내 특정 실행 위치와 맥락을 표현하는 클래스
class ProgramPoint {
protected:
  // 이 ProgramPoint가 속한 실행 맥락(함수 호출 스택, 블록 등)
  const LocationContext *LCtx;

  // 실제로 가리키는 코드 위치 정보(예: Stmt*, Block*, CallSite 등)
  void *Data1;
  void *Data2;

  // ProgramPoint의 종류(예: PreStmt, PostStmt, BlockEntrance 등)
  Kind PointKind;

  // 블록 내에서의 인덱스 등 추가 정보(필요에 따라 사용)
  unsigned Tag;

public:
  // 생성자, 접근자 등 다양한 메서드 제공
  const LocationContext *getLocationContext() const { return LCtx; }
  Kind getKind() const { return PointKind; }
  void *getData1() const { return Data1; }
  void *getData2() const { return Data2; }
  unsigned getTag() const { return Tag; }
  // ...
};
```
## 각 멤버의 역할

- **LocationContext *LCtx**
    
    - 이 ProgramPoint가 발생한 함수 호출 스택, 블록, 스코프 등 실행 맥락을 가리킴
        
    - 같은 코드 위치라도 실행 맥락이 다르면 별개의 ProgramPoint로 구분
        
- **void *Data1, void *Data2**
    
    - 실제로 가리키는 코드 위치(예: Stmt*는 특정 문장, Block*는 블록 등)
        
    - 다양한 종류의 ProgramPoint를 하나의 구조로 표현하기 위해 void*로 추상화
        
- **Kind PointKind**
    
    - 이 ProgramPoint의 종류를 나타내는 enum 값
        
        - 예: PreStmt(문장 실행 전), PostStmt(문장 실행 후), BlockEntrance, BlockExit, CallEnter, CallExit 등
            
- **unsigned Tag**
    
    - 블록 내 인덱스, 추가적인 식별자 등 부가 정보 저장용
        

## 주요 특징 및 동작

- **다양한 코드 위치 표현**
    
    - 한 줄의 코드 안에서도 여러 ProgramPoint가 존재할 수 있음
        
    - if문 조건식, 함수 호출 직전/후, 블록 진입/종료 등 세밀한 위치 구분
        
- **실행 맥락과 결합**
    
    - 항상 LocationContext와 결합되어,  
        “어떤 함수 호출 스택/블록/스코프에서 발생한 위치인가?”를 명확히 표현
        
- **ExplodedNode와의 관계**
    
    - 분석 그래프의 각 노드는 ProgramPoint를 포함
        
    - 이를 통해 분석 경로에서 “정확히 어떤 위치, 어떤 맥락”에서 상태가 변화했는지 추적 가능
        

## 예시

|종류|Data1/2 용례|PointKind 예시|설명|
|---|---|---|---|
|PreStmt|Stmt*|PreStmt|문장 실행 직전|
|PostStmt|Stmt*|PostStmt|문장 실행 직후|
|BlockEntrance|CFGBlock*|BlockEntrance|블록 진입 시점|
|CallEnter|CallExpr*, StackFrameContext*|CallEnter|함수 호출 진입|

## 요약

- **ProgramPoint**는 “코드 내 아주 세밀한 실행 위치”와 “실행 맥락”을 함께 표현
    
- 내부적으로 LocationContext, 코드 위치 데이터, 종류, 부가 정보를 멤버로 가짐
    
- 다양한 코드 이벤트(문장 실행, 블록 진입, 함수 호출 등)를 하나의 구조로 통합 관리
    

이 구조 덕분에 Clang Static Analyzer는 복잡한 실행 경로와 함수 호출, 블록 중첩 등도 정확하게 추적할 수 있습니다.