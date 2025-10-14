
Tidy Path
```shell
C:\Users\user\Documents\llvm-project\build\RelWithDebInfo\bin\clang-tidy.exe
```

Project Path
```shell
C:\Users\user\Desktop\ErrorTest\ErrorTest
```

```cpp
using SentinelStatic.Models;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;
using System.Threading.Tasks;

namespace SentinelStatic.Services
{
    public class ClangTidyRunnerService : IClangTidyRunnerService
    {
        private readonly ISystemIncludePathDetector _includePathDetector;

        public ClangTidyRunnerService(ISystemIncludePathDetector includePathDetector)
        {
            _includePathDetector = includePathDetector;
        }

        // clang-tidy 출력 형식: 파일경로:라인:컬럼: warning: 메시지 [체커이름]
        private static readonly Regex DiagnosticPattern = new Regex(
            @"^(?<file>.+?):(?<line>\d+):(?<column>\d+):\s+(?<severity>\w+):\s+(?<message>.+?)\s+\[(?<check>[\w\-\.]+)\]",
            RegexOptions.Compiled
        );

        // 진행률 파싱: [1/2] Processing file ...
        private static readonly Regex ProgressPattern = new Regex(
            @"^\[(?<current>\d+)/(?<total>\d+)\]\s+Processing\s+file",
            RegexOptions.Compiled
        );

        // "warnings generated" 메시지 필터링
        private static readonly Regex WarningsGeneratedPattern = new Regex(
            @"^\d+\s+warnings?\s+generated",
            RegexOptions.Compiled | RegexOptions.IgnoreCase
        );

        // "Suppressed" 메시지 필터링
        private static readonly Regex SuppressedPattern = new Regex(
            @"^Suppressed\s+\d+\s+warnings?",
            RegexOptions.Compiled | RegexOptions.IgnoreCase
        );

        // 코드 스니펫 라인 (숫자 | 코드 형식)
        private static readonly Regex CodeSnippetPattern = new Regex(
            @"^\s+\d+\s+\|",
            RegexOptions.Compiled
        );

        // 캐럿 표시 라인 (      ^ 형식)
        private static readonly Regex CaretPattern = new Regex(
            @"^\s+\|?\s*\^",
            RegexOptions.Compiled
        );

        // [추가] header-filter 정보성 메시지 필터링
        private static readonly Regex HeaderFilterInfoPattern = new Regex(
            @"^Use -header-filter=",
            RegexOptions.Compiled | RegexOptions.IgnoreCase
        );

        // [추가] 파일 경로만 있는 라인 필터링 (단독 파일명)
        private static readonly Regex FilePathOnlyPattern = new Regex(
            @"^[A-Za-z]:\\.*\.(cpp|c|h|hpp)$",
            RegexOptions.Compiled
        );

        public async Task RunAnalysisAsync(string clangTidyPath, string projectPath,
            IEnumerable<string> filesToAnalyze,
            IEnumerable<CheckerItem> enabledCheckers,
            Action<DiagnosticItem> onDiagnosticFound,
            Action<int, int> onProgressChanged = null)
        {
            // 1. .clang-tidy 파일 내용 생성 (YAML 형식)
            var sb = new StringBuilder();
            sb.AppendLine($"Checks: '-*,{string.Join(",", enabledCheckers.Select(c => c.Name))}'");
            if (enabledCheckers.Any(c => c.Options.Any()))
            {
                sb.AppendLine("CheckOptions:");
                foreach (var checker in enabledCheckers)
                {
                    foreach (var option in checker.Options)
                    {
                        string value = option.GetValueAsString();
                        sb.AppendLine($"  {checker.Name}.{option.Name}: {value}");
                    }
                }
            }

            // 2. .clang-tidy 파일 쓰기
            string clangTidyFilePath = Path.Combine(projectPath, ".clang-tidy");
            await File.WriteAllTextAsync(clangTidyFilePath, sb.ToString());

            // 시스템 Include 경로 자동 감지
            var systemIncludes = _includePathDetector.DetectSystemIncludePaths();
            string includeArgs = string.Join(" ", systemIncludes.Select(p => $"-I\"{p}\""));

            // 3. 인자 조합 및 프로세스 실행
            string filesArg = string.Join(" ", filesToAnalyze.Select(f => $"\"{f}\""));
            string arguments = $"{filesArg} -- {includeArgs}";

            var startInfo = new ProcessStartInfo
            {
                FileName = clangTidyPath,
                Arguments = arguments,
                WorkingDirectory = projectPath,
                UseShellExecute = false,
                RedirectStandardOutput = true,
                RedirectStandardError = true,
                StandardOutputEncoding = Encoding.UTF8,
                StandardErrorEncoding = Encoding.UTF8,
                CreateNoWindow = true,
            };

            using var process = new Process { StartInfo = startInfo };

            // stdout: 실제 경고 메시지
            process.OutputDataReceived += (sender, e) =>
            {
                if (!string.IsNullOrEmpty(e.Data))
                {
                    var diagnostic = ParseDiagnosticLine(e.Data);
                    if (diagnostic != null)
                    {
                        onDiagnosticFound(diagnostic);
                    }
                }
            };

            // stderr: 진행 메시지 처리
            process.ErrorDataReceived += (sender, e) =>
            {
                if (!string.IsNullOrEmpty(e.Data))
                {
                    // 진행률 파싱 시도
                    var progressMatch = ProgressPattern.Match(e.Data);
                    if (progressMatch.Success)
                    {
                        int current = int.Parse(progressMatch.Groups["current"].Value);
                        int total = int.Parse(progressMatch.Groups["total"].Value);
                        onProgressChanged?.Invoke(current, total);
                        return; // [수정] 진행률 처리 후 종료
                    }

                    // [수정] 정보성 메시지들 필터링
                    if (ShouldFilterStderrMessage(e.Data))
                    {
                        return; // 필터링된 메시지는 무시
                    }

                    // 기타 중요한 stderr 메시지만 전달
                    onDiagnosticFound(new DiagnosticItem { Message = $"[INFO] {e.Data}" });
                }
            };

            process.Start();
            process.BeginOutputReadLine();
            process.BeginErrorReadLine();
            await process.WaitForExitAsync();

            // 완료 시 진행률 100%로 설정
            onProgressChanged?.Invoke(100, 100);
        }

        // [추가] stderr 메시지 필터링 로직
        private static bool ShouldFilterStderrMessage(string message)
        {
            // warnings generated 메시지
            if (WarningsGeneratedPattern.IsMatch(message))
                return true;

            // Suppressed warnings 메시지
            if (SuppressedPattern.IsMatch(message))
                return true;

            // header-filter 안내 메시지
            if (HeaderFilterInfoPattern.IsMatch(message))
                return true;

            // 단독 파일 경로 (진행 상태 표시)
            if (FilePathOnlyPattern.IsMatch(message))
                return true;

            return false;
        }

        private DiagnosticItem ParseDiagnosticLine(string line)
        {
            var match = DiagnosticPattern.Match(line);

            if (match.Success)
            {
                return new DiagnosticItem
                {
                    FilePath = match.Groups["file"].Value.Trim(),
                    Line = int.TryParse(match.Groups["line"].Value, out var lineNum) ? lineNum : 0,
                    Column = int.TryParse(match.Groups["column"].Value, out var colNum) ? colNum : 0,
                    Message = match.Groups["message"].Value.Trim(),
                    CheckName = match.Groups["check"].Value.Trim()
                };
            }
            else if (CodeSnippetPattern.IsMatch(line) || CaretPattern.IsMatch(line))
            {
                return null;
            }
            else if (!string.IsNullOrWhiteSpace(line))
            {
                return new DiagnosticItem { Message = line.Trim() };
            }

            return null;
        }
    }
}

```