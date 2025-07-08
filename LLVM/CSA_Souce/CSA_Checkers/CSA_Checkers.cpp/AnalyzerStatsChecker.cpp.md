
>이 코드는 Clang Static Analyzer의 **방문 통계**(Analyzer visitation statistics)를 수집하고 보고하는 체크커(Checker)인 `AnalyzerStatsChecker`의 구현입니다.  
>
 분석이 끝나는 시점(`check::EndAnalysis` 콜백)에서 각 함수(또는 블록)의 **제어 흐름 그래프(CFG) 블록**통계를 수집하고, 결과를 리포트로 출력합니다.


```cpp
```