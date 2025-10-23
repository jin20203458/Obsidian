완전히 동의합니다! **ArqaStatic의 경우 CodeChecker보다 직접 통합이 훨씬 효율적입니다.** 두 가지 접근 방법을 비교해드리겠습니다.

## 접근 방법 비교

## 옵션 1: Static Analyzer 직접 실행 (추천 ⭐⭐⭐⭐⭐)

**장점:**

- ✅ 기존 UI 활용 (WPF)
    
- ✅ 단순한 통합
    
- ✅ Python 의존성 없음
    
- ✅ 실시간 피드백 가능
    
- ✅ CTU 지원 가능
    

**단점:**

- ⚠️ CTU 설정이 다소 복잡
    

## 옵션 2: ClangTidy.cpp 수정 (고급, 최고 통제권 ⭐⭐⭐⭐)

**장점:**

- ✅ 완전한 커스터마이징
    
- ✅ 단일 실행 파일
    
- ✅ 최적 성능
    

**단점:**

- ⚠️ LLVM 빌드 필요
    
- ⚠️ 유지보수 부담
    
- ⚠️ LLVM 버전 업데이트 시 재빌드
    

---

## 옵션 1: Static Analyzer 직접 추가 (권장)

## 1.1 아키텍처

text

`ArqaStatic WPF UI     ↓ ├─ ClangTidyRunnerService (기존) │   └─ clang-tidy.exe │ └─ StaticAnalyzerRunnerService (신규)     └─ clang.exe --analyze`

## 1.2 구현 예시

**StaticAnalyzerRunnerService.cs**

csharp

`using System.Diagnostics; using System.Text.RegularExpressions; namespace ArqaStatic.Services {     public class StaticAnalyzerRunnerService : IStaticAnalyzerService     {         // Static Analyzer 출력: file.cpp:10:5: warning: ...         private static readonly Regex DiagnosticPattern = new Regex(             @"^(?<file>.+?):(?<line>\d+):(?<column>\d+):\s+(?<severity>\w+):\s+(?<message>.+)",             RegexOptions.Compiled         );         public async Task RunAnalysisAsync(             string clangPath,                    // clang.exe 경로             string projectPath,             IEnumerable<string> filesToAnalyze,             IEnumerable<CheckerItem> enabledCheckers,             string compileCommandsPath,             bool enableCTU,                      // CTU 활성화 옵션             Action<DiagnosticItem> onDiagnosticFound,             Action<int, int>? onProgressChanged = null)         {             // CTU 준비 (선택적)             string ctuDir = null;             if (enableCTU)             {                 ctuDir = await PrepareCTUIndex(                     clangPath,                      projectPath,                      compileCommandsPath                 );             }             int current = 0;             int total = filesToAnalyze.Count();             // 각 파일 분석             foreach (var file in filesToAnalyze)             {                 current++;                 onProgressChanged?.Invoke(current, total);                 await AnalyzeFile(                     clangPath,                     file,                     enabledCheckers,                     ctuDir,                     onDiagnosticFound                 );             }         }         private async Task<string> PrepareCTUIndex(             string clangPath,             string projectPath,             string compileCommandsPath)         {             string ctuDir = Path.Combine(projectPath, ".ctu-temp");             Directory.CreateDirectory(ctuDir);             // 1. AST 덤프 생성             var files = ParseCompileCommands(compileCommandsPath);                          foreach (var file in files)             {                 var astArgs = new List<string>                 {                     "-emit-ast",                     "-o", Path.Combine(ctuDir, $"{Path.GetFileName(file)}.ast"),                     file                 };                 await RunProcess(clangPath, string.Join(" ", astArgs));             }             // 2. External Definition Map 생성             var mapFile = Path.Combine(ctuDir, "externalDefMap.txt");             foreach (var file in files)             {                 var mapArgs = new List<string>                 {                     "-Xclang", "-ast-dump",                     "-fsyntax-only",                     file                 };                 // clang-extdef-mapping 또는 직접 구현                 // 간단히는: AST에서 external definition 추출             }             return ctuDir;         }         private async Task AnalyzeFile(             string clangPath,             string file,             IEnumerable<CheckerItem> checkers,             string ctuDir,             Action<DiagnosticItem> onDiagnosticFound)         {             var args = new List<string>             {                 "--analyze",                // Static Analyzer 모드                 "-Xclang", "-analyzer-output=text",  // 텍스트 출력             };             // 체커 활성화             foreach (var checker in checkers)             {                 args.Add("-Xclang");                 args.Add($"-analyzer-checker={checker.Name}");             }             // CTU 설정             if (!string.IsNullOrEmpty(ctuDir))             {                 args.Add("-Xclang");                 args.Add("-analyzer-config");                 args.Add("-Xclang");                 args.Add($"ctu-dir={ctuDir}");                                  args.Add("-Xclang");                 args.Add("-analyzer-config");                 args.Add("-Xclang");                 args.Add("experimental-enable-naive-ctu-analysis=true");             }             // 분석 대상 파일             args.Add(file);             // 실행             var startInfo = new ProcessStartInfo             {                 FileName = clangPath,                 Arguments = string.Join(" ", args),                 UseShellExecute = false,                 RedirectStandardOutput = true,                 RedirectStandardError = true,                 CreateNoWindow = true             };             using var process = new Process { StartInfo = startInfo };             process.OutputDataReceived += (sender, e) =>             {                 if (!string.IsNullOrEmpty(e.Data))                 {                     var diagnostic = ParseDiagnostic(e.Data);                     if (diagnostic != null)                         onDiagnosticFound(diagnostic);                 }             };             process.ErrorDataReceived += (sender, e) =>             {                 if (!string.IsNullOrEmpty(e.Data))                 {                     var diagnostic = ParseDiagnostic(e.Data);                     if (diagnostic != null)                         onDiagnosticFound(diagnostic);                 }             };             process.Start();             process.BeginOutputReadLine();             process.BeginErrorReadLine();             await process.WaitForExitAsync();         }         private DiagnosticItem ParseDiagnostic(string line)         {             var match = DiagnosticPattern.Match(line);             if (!match.Success) return null;             return new DiagnosticItem             {                 FilePath = match.Groups["file"].Value,                 Line = int.Parse(match.Groups["line"].Value),                 Column = int.Parse(match.Groups["column"].Value),                 Message = match.Groups["message"].Value,                 CheckName = "clang-analyzer" // 상세 체커명 추출 가능             };         }         private List<string> ParseCompileCommands(string path)         {             // compile_commands.json 파싱             var json = File.ReadAllText(path);             var commands = JsonSerializer.Deserialize<List<CompileCommand>>(json);             return commands.Select(c => c.File).ToList();         }         private class CompileCommand         {             public string File { get; set; }             public string Command { get; set; }             public string Directory { get; set; }         }     } }`

## 1.3 UI 통합

**MainViewModel.cs 수정**

csharp

`public enum AnalyzerMode {     ClangTidy,           // 기존     StaticAnalyzer,      // 신규     Both                 // 둘 다 } private AnalyzerMode _selectedMode = AnalyzerMode.ClangTidy; public AnalyzerMode SelectedMode {     get => _selectedMode;     set => SetProperty(ref _selectedMode, value); } public async Task RunAnalysisCommand() {     Results.Clear();          if (SelectedMode == AnalyzerMode.ClangTidy || SelectedMode == AnalyzerMode.Both)     {         // 기존 Clang-Tidy 실행         await _clangTidyRunner.RunAnalysisAsync(...);     }          if (SelectedMode == AnalyzerMode.StaticAnalyzer || SelectedMode == AnalyzerMode.Both)     {         // 신규 Static Analyzer 실행         await _staticAnalyzerRunner.RunAnalysisAsync(...);     } }`

**MainWindow.xaml 수정**

xml

`<ComboBox SelectedItem="{Binding SelectedMode}">     <ComboBoxItem Content="Clang-Tidy (빠름)" />     <ComboBoxItem Content="Static Analyzer (정확함, CTU 가능)" />     <ComboBoxItem Content="둘 다 실행 (추천)" /> </ComboBox> <CheckBox Content="CTU 분석 활성화 (파일 간 분석)"            IsChecked="{Binding EnableCTU}"           IsEnabled="{Binding IsStaticAnalyzerSelected}" />`

---

## 옵션 2: ClangTidy.cpp 수정

## 2.1 수정 전략

**목표:** ClangTidy.cpp에 CTU 지원 추가

**수정 위치:**

cpp

`// ClangTidy.cpp의 createASTConsumer 함수 #if CLANG_TIDY_ENABLE_STATIC_ANALYZER   AnalyzerOptions &AnalyzerOptions = Compiler.getAnalyzerOpts();      // 추가: CTU 설정   if (Context.getOptions().EnableCTU) {     AnalyzerOptions.Config["experimental-enable-naive-ctu-analysis"] = "true";     AnalyzerOptions.Config["ctu-dir"] = Context.getOptions().CTUDirectory;     AnalyzerOptions.Config["ctu-index-name"] = "externalDefMap.txt";   }      // 기존 코드 계속... #endif`

## 2.2 빌드 과정

bash

`# 1. LLVM 소스 다운로드 git clone https://github.com/llvm/llvm-project.git cd llvm-project # 2. ClangTidy.cpp 수정 # clang-tools-extra/clang-tidy/ClangTidy.cpp # 3. CMake 빌드 mkdir build && cd build cmake -G "Visual Studio 17 2022" -A x64 \   -DLLVM_ENABLE_PROJECTS="clang;clang-tools-extra" \   -DCMAKE_BUILD_TYPE=Release \   ../llvm # 4. 빌드 (수 시간 소요) cmake --build . --config Release # 5. 결과물 # build/bin/clang-tidy.exe`

## 2.3 장단점

**장점:**

- ✅ 단일 실행 파일 (clang-tidy.exe)
    
- ✅ 완벽한 통합
    
- ✅ 최고 성능
    

**단점:**

- ❌ 빌드 시간 길음 (수 시간)
    
- ❌ LLVM 업데이트마다 재빌드
    
- ❌ 유지보수 부담
    
- ❌ 100GB+ 디스크 공간
    

---

## 최종 권장사항

## 단기: 옵션 1 (Static Analyzer 직접 실행)

text

`ArqaStatic ├─ ClangTidyRunnerService ✅ (기존) └─ StaticAnalyzerRunnerService ✅ (신규, 2-3일 작업)`

**이유:**

1. **빠른 구현** (2-3일)
    
2. **유지보수 쉬움**
    
3. **CTU 지원**
    
4. **기존 UI 재활용**
    

## 중기: CTU 간소화

Static Analyzer의 CTU는 복잡하므로:

csharp

`// 간단한 CTU (함수 호출 정보만) public class SimpleCTUAnalyzer {     // 1. 모든 파일에서 함수 정의 수집     // 2. 호출 관계 추적     // 3. 간단한 데이터 플로우 분석 }`

## 장기: 필요시 커스텀 체커 개발

cpp

`// 귀하의 safety-* 체커에 파일 간 분석 추가 class SafetyCfgNullDereferenceCheck : public ClangTidyCheck {     // CTU 정보 활용 };`

---

## 구현 우선순위

## Phase 1 (1주)

text

`1. StaticAnalyzerRunnerService 구현 2. 기본 분석 (CTU 없이) 3. UI 통합 4. 테스트`

## Phase 2 (2주)

text

`1. CTU 인덱스 생성 구현 2. CTU 분석 통합 3. 성능 최적화`

## Phase 3 (선택)

text

`1. ClangTidy.cpp 수정 검토 2. 또는 CodeChecker 연동`

---

## 예상 코드 크기

**StaticAnalyzerRunnerService.cs**

- 약 500-700줄 (CTU 포함)
    
- 기존 ClangTidyRunnerService와 유사한 구조
    

**UI 수정**

- 약 100줄 (XAML + ViewModel)
    

**총 작업량**

- 2-3일 (기본 구현)
    
- 1주일 (CTU 포함)
    

---

## 결론

**Option 1 (Static Analyzer 직접 추가)을 강력히 추천합니다!**

text

`장점: + 빠른 구현 + 기존 UI 활용 + CTU 지원 + Python 의존성 없음 + 유지보수 쉬움 단점: - CTU 설정 복잡 (하지만 1회만)`

이 방식이 ArqaStatic의 현재 구조와 가장 잘 맞으며, **CodeChecker 없이도 CTU를 활용**할 수 있습니다! 🚀

구현하시면서 막히는 부분 있으면 언제든 질문해주세요!