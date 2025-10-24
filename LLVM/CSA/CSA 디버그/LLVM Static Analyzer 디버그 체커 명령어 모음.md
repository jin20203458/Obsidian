

---

## 🎯 핵심 시각화 체커들

- 명령어는 프론트엔드

| 체커 이름                            | 설명              | 명령어                                                                             |
| -------------------------------- | --------------- | ------------------------------------------------------------------------------- |
| **Exploded Graph 시각화** ⭐⭐⭐       | 심볼릭 실행 경로 시각화   | `clang -cc1 -analyze -analyzer-checker="debug.ViewExplodedGraph" your_file.cpp` |
| **Control Flow Graph (CFG)** ⭐⭐⭐ | 함수 내부 제어 흐름 시각화 | `clang -cc1 -analyze -analyzer-checker="debug.ViewCFG" your_file.cpp`           |
| **Call Graph** ⭐⭐                | 함수 간 호출 관계 시각화  | `clang -cc1 -analyze -analyzer-checker="debug.ViewCallGraph" your_file.cpp`     |
드라이버 : 
```cpp
clang++ --analyze -Xanalyzer -analyzer-checker="debug.ViewExplodedGraph" ErrorTest.cpp
```

---

## 📊 텍스트 출력 분석 체커들

```powershell
clang -cc1 -analyze -analyzer-checker="debug.DumpCFG" your_file.cpp           # CFG 텍스트 출력
clang -cc1 -analyze -analyzer-checker="debug.DumpCallGraph" your_file.cpp     # Call Graph 텍스트 출력
clang -cc1 -analyze -analyzer-checker="debug.Stats" your_file.cpp             # 분석 통계
clang -cc1 -analyze -analyzer-checker="debug.AnalysisOrder" your_file.cpp     # 분석 순서 추적
```

---

## 🔍 고급 분석 체커들

```powershell
clang -cc1 -analyze -analyzer-checker="debug.DumpLiveVars" your_file.cpp          # Live Variables
clang -cc1 -analyze -analyzer-checker="debug.DumpLiveExprs" your_file.cpp         # Live Expressions
clang -cc1 -analyze -analyzer-checker="debug.DumpDominators" your_file.cpp        # Dominance Tree
clang -cc1 -analyze -analyzer-checker="debug.DumpPostDominators" your_file.cpp    # Post-Dominance Tree
clang -cc1 -analyze -analyzer-checker="debug.DumpControlDependencies" your_file.cpp # Control Dependencies
```

---

## 🛠️ 디버깅 및 테스트 체커들

```powershell
clang -cc1 -analyze -analyzer-checker="debug.ReportStmts" your_file.cpp       # 모든 문장 리포트
clang -cc1 -analyze -analyzer-checker="debug.DumpTraversal" your_file.cpp     # 분기 조건 추적
clang -cc1 -analyze -analyzer-checker="debug.DumpCalls" your_file.cpp         # 함수 호출 추적
clang -cc1 -analyze -analyzer-checker="debug.ExprInspection" your_file.cpp    # Expression Inspector
clang -cc1 -analyze -analyzer-checker="debug.TaintTest" your_file.cpp         # Taint 테스트
```

---

## 🔧 설정 및 문서화 체커들

```powershell
clang -cc1 -analyze -analyzer-checker="debug.ConfigDumper" your_file.cpp              # 설정 정보 출력
clang -cc1 -analyze -analyzer-checker="debug.CheckerDocumentation" your_file.cpp      # 체커 문서화
clang -cc1 -analyze -analyzer-checker="debug.StdCLibraryFunctionsTester" your_file.cpp # 표준 라이브러리 함수 테스터
clang -cc1 -analyze -analyzer-checker="debug.StreamTester" your_file.cpp              # Stream 테스터
```

---

## 📝 실제 사용 예시

```powershell
# 테스트 파일 위치로 이동
cd C:\Users\user\Desktop\ErrorTest\ErrorTest

# Exploded Graph 시각화
clang -cc1 -analyze -analyzer-checker="debug.ViewExplodedGraph" ErrorTest.cpp

# CFG 시각화
clang -cc1 -analyze -analyzer-checker="debug.ViewCFG" ErrorTest.cpp

# 분석 통계 출력
clang -cc1 -analyze -analyzer-checker="debug.Stats" ErrorTest.cpp

# 디버그 체커 목록 확인
clang -cc1 -analyzer-checker-help | findstr debug
```

---

## 🚀 빠른 테스트 스크립트

```powershell
clang -cc1 -analyze -analyzer-checker="debug.ViewExplodedGraph,debug.Stats,debug.ViewCFG" your_file.cpp
```

---

## ✅ 요약

- `clang -cc1` 명령어로 직접 실행 가능
- 디버그 체커들을 조합하여 다양한 분석 가능
- `.dot` 파일 기반 시각화로 심층 분석 지원
- EvalAssume 기반 체커 개발에 필요한 모든 기반 확보

---

필요하다면 이 내용을 `.md` 파일로 포맷팅하거나, 자동 실행 스크립트로 변환해드릴 수도 있어요. 원하시나요? 😄