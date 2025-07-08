
> **SourceManager**는 Clang에서 소스 코드의 위치 정보를 체계적으로 관리하는 클래스입니다.
> 
> **SourceManager는 단일 TU 내에서 여러 파일(소스, 헤더, 임시 버퍼 등)을 동시에 관리**합니다.

### Clang SourceManager 객체 관계도 (개념도)

```scss
      ┌──────────────┐
      │ SourceManager│
      └───────┬──────┘
              │
   ┌──────────┼────────────┐
   │          │            │
   ▼          ▼            ▼
FileID   SourceLocation  PresumedLoc
   │         ▲              ▲
   │         │              │
   │         └─────┬────────┘
   │               │
   ▼               ▼
소스 파일      AST 노드
(실제 경로)   (Decl, Stmt, Expr 등)
```



### 객체 계층 구조도

```scss
SourceManager
│
├── FileID 1 ─────┬─ SLocEntry(FileInfo/ExpansionInfo)
│                 │
│                 └─ ContentCache
│                        │
│                        └─ MemoryBuffer
│
├── FileID 2 ─────┬─ SLocEntry(FileInfo/ExpansionInfo)
│                 │
│                 └─ ContentCache
│                        │
│                        └─ MemoryBuffer
│
└── ... (여러 파일)
│
└── LineTableInfo (전체 파일의 라인/컬럼 정보)
```

## SourceManager란?

**SourceManager**는  
Clang(정적 분석기, 컴파일러 등)에서  
**"이 코드가 어느 파일, 몇 줄, 몇 컬럼에 있는지"**  
정확하게 추적해주는 **위치 정보 관리자**입니다.

## 왜 필요할까?

- 소스 코드가 여러 파일(메인 소스, 헤더, 인클루드, 매크로 등)에 흩어져 있죠.
    
- 분석 결과나 경고 메시지를  
    **"main.cpp 12번째 줄 5번째 칸에서 문제가 있다!"**  
    이렇게 정확히 알려주려면  
    모든 파일과 줄, 컬럼 정보를 한 곳에서 관리해야 해요.
    
- 이 역할을 SourceManager가 합니다.
    

## 어떻게 동작할까? (구조)

1. **여러 파일을 한꺼번에 관리**
    
    - main.cpp, a.h, b.h, 임시 버퍼, 매크로 확장 등  
        TU(컴파일 단위)에 포함된 모든 파일을 다 관리합니다.
        
2. **각 파일에 고유 번호(FileID) 부여**
    
    - 파일마다 FileID를 붙여서 구분합니다.
        
3. **위치 정보(SourceLocation) 관리**
    
    - 코드의 각 토큰, 문장, 변수 등은  
        "어느 파일(FileID), 몇 번째 위치(오프셋)"로 추적됩니다.
        
4. **줄/컬럼 정보 변환**
    
    - "이 위치가 실제로 몇 번째 줄, 몇 번째 컬럼이냐?"  
        → 내부의 LineTable에서 빠르게 변환해줍니다.
        
5. **매크로/인클루드 위치도 추적**
    
    - 매크로 확장, #include 등으로  
        실제 위치와 표시 위치가 다를 때도  
        둘 다 정확히 추적합니다[1](https://docs.hdoc.io/hdoc/llvm-project/r9EE212A2867FC293.html)[6](https://swtv.kaist.ac.kr/courses/cs453-fall14/lec5.5-building-Clang-analyzer.pdf).
        

## 예시

```cpp
#include "a.h"
int main() {
foo(); // 여기서 에러! 
}
```

- foo()가 a.h에 정의되어 있고, main.cpp에서 호출.
    
- SourceManager는
    
    - main.cpp, a.h 둘 다 관리하고
        
    - "foo() 호출이 main.cpp 3번째 줄 5번째 칸"  
        이렇게 정확히 알려줍니다.
        

## 실제로 어떻게 쓰이나?

- AST(구문 트리)에서 어떤 문장(Stmt)의 위치를 알고 싶을 때
    
```cpp
SourceLocation loc = stmt->getBeginLoc();
unsigned line = srcMgr.getExpansionLineNumber(loc);
unsigned col  = srcMgr.getExpansionColumnNumber(loc);
```
    
- 경고/오류 메시지, 분석 리포트, 코드 수정 등  
    **모든 "위치 표시"가 SourceManager를 거칩니다**[1](https://docs.hdoc.io/hdoc/llvm-project/r9EE212A2867FC293.html)[6](https://swtv.kaist.ac.kr/courses/cs458-fall2022/part1-coverage/lec8-building-Clang-analyzer-v3.pdf).
    

## 요약

- **SourceManager는 "이 코드가 어디에 있는지"를 정확하게 추적하는 관리자!**
    
- 여러 파일, 줄, 컬럼, 매크로, 인클루드까지  
    **모든 위치 정보를 한 곳에서 관리**합니다.
    
- Clang 기반 분석기/컴파일러의 **위치 정보 인프라**입니다.
    

**참고로, SourceManager는 하나의 TU(컴파일 단위) 안에서만 동작하며,  
그 TU에 포함된 모든 파일을 동시에 관리합니다.**

---

추가 질문하기

소스 확인

1. [https://clang.llvm.org/doxygen/classclang_1_1SourceManager.html](https://clang.llvm.org/doxygen/classclang_1_1SourceManager.html)
2. [https://pangyoalto.com/clang-and-optimization/](https://pangyoalto.com/clang-and-optimization/)
3. [https://clang.llvm.org/doxygen/SourceManager_8h.html](https://clang.llvm.org/doxygen/SourceManager_8h.html)
4. [https://hackyboiz.github.io/2020/11/22/idioth/av_evasion_part1/](https://hackyboiz.github.io/2020/11/22/idioth/av_evasion_part1/)
5. [https://docs.hdoc.io/hdoc/llvm-project/r9EE212A2867FC293.html](https://docs.hdoc.io/hdoc/llvm-project/r9EE212A2867FC293.html)
6. [https://swtv.kaist.ac.kr/courses/cs453-fall14/lec5.5-building-Clang-analyzer.pdf](https://swtv.kaist.ac.kr/courses/cs453-fall14/lec5.5-building-Clang-analyzer.pdf)
7. [https://swtv.kaist.ac.kr/courses/cs458-fall2022/part1-coverage/lec8-building-Clang-analyzer-v3.pdf](https://swtv.kaist.ac.kr/courses/cs458-fall2022/part1-coverage/lec8-building-Clang-analyzer-v3.pdf)
8. [https://stackoverflow.com/questions/53851704/in-clang-can-i-access-the-sourcemanager-when-writing-a-custom-astmatcher](https://stackoverflow.com/questions/53851704/in-clang-can-i-access-the-sourcemanager-when-writing-a-custom-astmatcher)
9. [https://github.com/llvm-mirror/clang/blob/master/include/clang/Basic/SourceManager.h](https://github.com/llvm-mirror/clang/blob/master/include/clang/Basic/SourceManager.h)