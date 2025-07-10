
참조:
[[BugReport]]

## Clang Static Analyzer 주요 버그 타입(BugType) 정리

Clang Static Analyzer는 다양한 **버그 타입**(BugType)을 정의해 놓고, 각 타입별로 버그를 탐지합니다.  
아래는 실제로 많이 쓰이는 대표적인 버그 타입과 간단한 설명입니다.

## 1. 메모리 관련 버그

| 버그 타입명               | 설명                   |
| -------------------- | -------------------- |
| **Memory Leak**      | 할당된 메모리를 해제하지 않고 유실  |
| **Use-after-free**   | 이미 해제된 메모리 영역을 다시 사용 |
| **Double Free**      | 같은 메모리 영역을 두 번 해제    |
| **Dangling Pointer** | 유효하지 않은(해제된) 포인터 사용  |

## 2. 버퍼 및 경계 관련 버그

|버그 타입명|설명|
|---|---|
|**Out-of-bound access**|배열/버퍼 경계 밖으로 접근|
|**Buffer Overrun**|버퍼 크기를 초과하여 쓰기/읽기|
|**Buffer Underrun**|버퍼 시작점 이전 메모리 접근|

## 3. 널 포인터 및 값 관련 버그

|버그 타입명|설명|
|---|---|
|**Null Dereference**|널 포인터를 역참조|
|**Uninitialized Value**|초기화되지 않은 값 사용|
|**Garbage Value**|쓰레기 값(정의되지 않은 값) 사용|

## 4. 논리/연산 관련 버그

|버그 타입명|설명|
|---|---|
|**Divide by Zero**|0으로 나누기 시도|
|**Logic Error**|비정상 논리 연산(항상 참/거짓 등)|
|**Dead Store**|사용되지 않는 값에 저장만 하고 미사용|

## 5. Taint(오염) 관련 버그

|버그 타입명|설명|
|---|---|
|**Tainted Data**|신뢰할 수 없는(오염된) 데이터가 위험하게 사용될 때[1](https://clang.llvm.org/docs/analyzer/user-docs/TaintAnalysisConfiguration.html)[2](https://blog.trailofbits.com/2014/04/27/using-static-analysis-and-clang-to-find-heartbleed/)|
|**Injection Vulnerability**|외부 입력이 검증 없이 사용되어 취약점 발생 가능성|

- 예시:  
    `BugType TaintBT{this, "Out-of-bound access", categories::TaintedData};`  
    → **오염된 데이터(Tainted Data)**가 경계 밖 접근(Out-of-bound access)에 사용될 때 발생
    

## 6. 기타 특수 버그

|버그 타입명|설명|
|---|---|
|**Format String Error**|printf 등에서 포맷 문자열과 인자 불일치|
|**Incorrect Length Parameter**|잘못된 길이 인자 사용|
|**Unused Code**|사용되지 않는 코드(Dead Code)|

## 참고: 버그 타입과 체커의 관계

- 각 **체커(Checker)**는 자신만의 **BugType**을 정의하고,  
    해당 타입의 버그가 탐지되면 리포트로 출력합니다[3](https://github.com/haoNoQ/clang-analyzer-guide/blob/master/clang-analyzer-guide.tex).
    
- 예) `core.NullDereference`, `core.DivideZero`, `security.insecureAPI`,  
    `alpha.security.taint`, `unix.Malloc`, `deadcode.DeadStores` 등
    

## 요약

- Clang Static Analyzer는 메모리, 경계, 널 포인터, 논리, 오염(Taint) 등  
    다양한 버그 타입을 내장하고 있습니다.
    
- **TaintedData** 등은 외부 입력(사용자, 네트워크 등)에서 유입된 값이  
    검증 없이 위험하게 사용될 때 탐지하는 "오염 데이터" 관련 버그 타입입니다.
    
- 각 버그 타입은 체커별로 다를 수 있으며,  
    공식 문서나 각 체크커의 소스에서 추가 타입을 확인할 수 있습니다[1](https://clang.llvm.org/docs/analyzer/user-docs/TaintAnalysisConfiguration.html)[3](https://github.com/haoNoQ/clang-analyzer-guide/blob/master/clang-analyzer-guide.tex)[2](https://blog.trailofbits.com/2014/04/27/using-static-analysis-and-clang-to-find-heartbleed/).
    

궁금한 버그 타입이나 체커가 있으면 언제든 질문해 주세요!

1. [https://clang.llvm.org/docs/analyzer/user-docs/TaintAnalysisConfiguration.html](https://clang.llvm.org/docs/analyzer/user-docs/TaintAnalysisConfiguration.html)
2. [https://blog.trailofbits.com/2014/04/27/using-static-analysis-and-clang-to-find-heartbleed/](https://blog.trailofbits.com/2014/04/27/using-static-analysis-and-clang-to-find-heartbleed/)
3. [https://github.com/haoNoQ/clang-analyzer-guide/blob/master/clang-analyzer-guide.tex](https://github.com/haoNoQ/clang-analyzer-guide/blob/master/clang-analyzer-guide.tex)
4. [https://llvm.org/devmtg/2008-08/Kremenek_StaticAnalyzer.pdf](https://llvm.org/devmtg/2008-08/Kremenek_StaticAnalyzer.pdf)
5. [https://stackoverflow.com/questions/42696759/clang-static-analyzer-doesnt-find-the-most-basic-problems](https://stackoverflow.com/questions/42696759/clang-static-analyzer-doesnt-find-the-most-basic-problems)
6. [https://llvm.org/devmtg/2020-09/slides/Using_the_clang_static_ananalyzer_to_find_bugs.pdf](https://llvm.org/devmtg/2020-09/slides/Using_the_clang_static_ananalyzer_to_find_bugs.pdf)
7. [https://chromium.googlesource.com/chromium/src.git/+/65.0.3283.0/docs/clang_static_analyzer.md](https://chromium.googlesource.com/chromium/src.git/+/65.0.3283.0/docs/clang_static_analyzer.md)
8. [https://randomascii.wordpress.com/2013/06/24/two-years-and-thousands-of-bugs-of-static-analysis/](https://randomascii.wordpress.com/2013/06/24/two-years-and-thousands-of-bugs-of-static-analysis/)
9. [https://discourse.llvm.org/t/how-to-change-the-clang-static-analyzer-checking-way/53269](https://discourse.llvm.org/t/how-to-change-the-clang-static-analyzer-checking-way/53269)
10. [https://github.com/llvm/llvm-project/issues/57697](https://github.com/llvm/llvm-project/issues/57697)
11. [https://sergioprado.blog/bug-hunting-with-static-analysis-tools/](https://sergioprado.blog/bug-hunting-with-static-analysis-tools/)
12. [https://clang.llvm.org/docs/ClangStaticAnalyzer.html](https://clang.llvm.org/docs/ClangStaticAnalyzer.html)
13. [https://arxiv.org/html/2503.09002v2](https://arxiv.org/html/2503.09002v2)
14. [https://www.reddit.com/r/C_Programming/comments/9jl3w1/bug_in_clang_static_analyzer/](https://www.reddit.com/r/C_Programming/comments/9jl3w1/bug_in_clang_static_analyzer/)
15. [https://stackoverflow.com/questions/51278439/how-to-taint-the-command-line-arguments-in-clang-static-analyzer](https://stackoverflow.com/questions/51278439/how-to-taint-the-command-line-arguments-in-clang-static-analyzer)
16. [https://reviews.llvm.org/D98741](https://reviews.llvm.org/D98741)
17. [https://www.usenix.org/system/files/soups2021-ploger.pdf](https://www.usenix.org/system/files/soups2021-ploger.pdf)
18. [https://stackoverflow.com/questions/57899547](https://stackoverflow.com/questions/57899547)
19. [https://www.youtube.com/watch?v=nTslG8HtKeA](https://www.youtube.com/watch?v=nTslG8HtKeA)
20. [https://github.com/llvm/llvm-project/issues/63681](https://github.com/llvm/llvm-project/issues/63681)