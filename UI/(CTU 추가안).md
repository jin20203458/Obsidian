네, 그렇다면 상황이 명확합니다. LLVM 빌드 환경과 커스텀 체커 개발 경험이 있으시다면, **`clang-tidy.cpp`를 직접 수정하여 Static Analyzer의 CTU(Cross Translation Unit) 기능을 통합하는 것이 압도적으로 유리합니다.**

제공해주신 훌륭한 접근 방안(옵션 2)을 바탕으로, **ArqaStatic에 CTU 기능을 탑재한 커스텀 `clang-tidy`를 통합하는 체계적인 가이드**를 정리해 드립니다.

---

## 1. 최종 목표 및 전략 🎯

전략:

clang.exe --analyze를 별도로 실행하는 대신 (옵션 1), 기존 clang-tidy.exe 바이너리 자체에 Static Analyzer의 CTU(파일 간 분석) 기능을 활성화하는 코드를 추가합니다.

**기대 효과:**

- **단일 바이너리:** `clang-tidy.exe` 하나로 Tidy 체커, Static Analyzer 체커, CTU 분석까지 모두 처리합니다.
    
- **완전한 통합:** ArqaStatic UI는 기존처럼 `ClangTidyRunnerService`만 호출하면 됩니다.
    
- **강력한 분석:** `safety-*` 같은 커스텀 체커가 CTU 정보를 활용하여 함수 호출 경계를 넘어선 널 포인터 역참조 등을 탐지할 수 있습니다.
    
- **의존성 없음:** Python(CodeChecker)이나 별도 프로세스 실행이 필요 없습니다.
    

---

## 2. 구현 로드맵 🗺️

전체 작업은 크게 4단계로 나뉩니다.

1. **Phase 1: `clang-tidy` 핵심 수정 (C++)**
    
    - `ClangTidyOptions.h`: CTU 활성화(`EnableCTU`), CTU 인덱스 경로(`CTUDir`) 옵션을 추가합니다.
        
    - `ClangTidy.cpp` (`prepareCTUIndex`): CTU 분석에 필요한 AST 덤프 및 `externalDefMap.txt` 파일을 생성하는 로직을 추가합니다.
        
    - `ClangTidy.cpp` (`runClangTidy`): `EnableCTU` 옵션이 켜지면 `prepareCTUIndex`를 호출하도록 수정합니다.
        
    - `ClangTidy.cpp` (`createASTConsumer`): 컴파일러의 `AnalyzerOptions`에 CTU 관련 설정(`experimental-enable-naive-ctu-analysis`, `ctu-dir` 등)을 전달합니다.
        
2. **Phase 2: 옵션 연동 (C++)**
    
    - `ClangTidyMain.cpp`: `--enable-ctu`, `--ctu-dir` 커맨드라인 옵션을 추가합니다.
        
    - `ClangTidyOptions.cpp`: `.clang-tidy` YAML 설정 파일에서 `EnableCTU`, `CTUDir`를 읽을 수 있도록 YAML 매핑을 추가합니다.
        
3. **Phase 3: ArqaStatic 연동 (C# / XAML)**
    
    - `ClangTidyRunnerService.cs`: `EnableCTU` 옵션을 받아 `clang-tidy.exe`에 `--enable-ctu` 인자를 전달하도록 수정합니다.
        
    - `MainViewModel.cs` / `MainWindow.xaml`: UI에 "CTU 분석 활성화" 체크박스를 추가합니다.
        
4. **Phase 4: 커스텀 체커 수정 (C++)**
    
    - 개발 중이신 `safety-*` 체커가 `ASTContext`의 Cross-TU 기능을 활용하여 다른 파일의 함수 정의(Definition)를 가져와 분석하도록 수정합니다.
        

---

## 3. 단계별 상세 가이드 🛠️

제시해주신 코드 스케치를 기반으로 각 파일의 수정 사항을 체계적으로 배치했습니다.

### Phase 1: `clang-tidy` 핵심 수정

#### 1.1. `ClangTidyOptions.h`: 옵션 필드 추가

`struct ClangTidyOptions` 내부에 CTU 관련 설정을 추가합니다.

C++

```
// clang-tools-extra/clang-tidy/ClangTidyOptions.h
struct ClangTidyOptions {
    // ... 기존 필드들 ...

    // ===== CTU 지원 추가 =====
    std::optional<bool> EnableCTU;
    std::optional<std::string> CTUDir;
    std::optional<std::string> CTUIndexName; // (선택적)
};
```

_(참고: `llvm::yaml::MappingTraits` 업데이트도 필요합니다. Phase 2.2 참고)_

#### 1.2. `ClangTidy.cpp`: CTU 인덱스 생성 로직 추가 (개념적 예시)

`runClangTidy` 함수가 호출되기 전에, CTU 인덱스를 미리 생성하는 헬퍼 함수들을 추가합니다.

C++

```
// clang-tools-extra/clang-tidy/ClangTidy.cpp
namespace {

// (제시해주신 prepareCTUIndex 및 generateExternalDefMap 로직)
// 이 함수들은 컴파일 데이터베이스를 기반으로 각 파일의 AST를 덤프하고
// externalDefMap.txt를 생성하는 역할을 합니다.
bool prepareCTUIndex(
    const CompilationDatabase &Compilations,
    ArrayRef<std::string> InputFiles,
    StringRef CTUDir,
    DiagnosticsEngine &DE) 
{
    llvm::outs() << "Preparing CTU index in: " << CTUDir << "\n";
    
    // 1. CTU 디렉토리 생성
    if (std::error_code EC = llvm::sys::fs::create_directories(CTUDir)) {
       llvm::errs() << "Error creating CTU directory: " << EC.message() << "\n";
       return false;
    }

    // 2. 각 파일에 대해 AST 덤프 생성 (-emit-ast)
    // ... (Compilations에서 커맨드를 가져와 -emit-ast, -o <ctu-dir/file.ast> 추가 후 실행) ...
    // (참고: system() 대신 Clang 드라이버를 직접 호출하는 것이 더 견고합니다.)

    // 3. External Definition Map 생성
    // ... (generateExternalDefMap 호출, AST에서 External Def를 추출하여 맵 파일 생성) ...
    
    llvm::outs() << "CTU index preparation complete.\n";
    return true;
}

} // namespace
```

#### 1.3. `ClangTidy.cpp` (`runClangTidy`): CTU 인덱싱 트리거

`runClangTidy` 함수 초입부에서 CTU 옵션이 켜져 있으면 인덱싱을 수행합니다.

C++

```
// clang-tools-extra/clang-tidy/ClangTidy.cpp
std::vector<ClangTidyError> runClangTidy(
    clang::tidy::ClangTidyContext &Context,
    const CompilationDatabase &Compilations,
    ArrayRef<std::string> InputFiles,
    /* ... (기타 인자) ... */) 
{
    ClangTool Tool(Compilations, InputFiles,
                   std::make_shared<PCHContainerOperations>(), BaseFS);

    // ===== CTU 지원 추가 =====
    if (Context.getOptions().EnableCTU.value_or(false)) {
        std::string CTUDir = Context.getOptions().CTUDir.value_or("./ctu-dir");
        
        DiagnosticsEngine DE(new DiagnosticIDs(), new DiagnosticOptions());
        if (!prepareCTUIndex(Compilations, InputFiles, CTUDir, DE)) {
            // 실패 시 경고만 하고 Tidy 분석은 계속 진행 (CTU 없이)
            llvm::errs() << "Warning: CTU index preparation failed. "
                         << "Proceeding without CTU analysis.\n";
        }
    }
    // ===== CTU 지원 끝 =====

    // ... (기존 ClangTool 설정 코드) ...
}
```

#### 1.4. `ClangTidy.cpp` (`createASTConsumer`): CTU 옵션 전달

`ClangTidyASTConsumerFactory::createASTConsumer` 내부에서 `CompilerInstance`의 `AnalyzerOptions`에 CTU 설정을 주입합니다.

C++

```
// clang-tools-extra/clang-tidy/ClangTidy.cpp
std::unique_ptr<clang::ASTConsumer> 
ClangTidyASTConsumerFactory::createASTConsumer(
    clang::CompilerInstance &Compiler, StringRef File) 
{
    // ... (기존 Consumers, Profiling, Finder 등 설정) ...

    #if CLANG_TIDY_ENABLE_STATIC_ANALYZER
    // Static Analyzer 체커가 활성화된 경우
    AnalyzerOptions &AnalyzerOptions = Compiler.getAnalyzerOpts();
    // ... (기존 AnalyzerOptions 설정) ...

    if (!AnalyzerOptions.CheckersAndPackages.empty()) {
        // ... (기존 setStaticAnalyzerCheckerOpts 등) ...

        // ===== CTU 설정 추가 =====
        if (Context.getOptions().EnableCTU.value_or(false)) {
            std::string CTUDir = Context.getOptions().CTUDir.value_or("./ctu-dir");
            std::string CTUIndex = Context.getOptions().CTUIndexName.value_or("externalDefMap.txt");
            
            AnalyzerOptions.Config["experimental-enable-naive-ctu-analysis"] = "true";
            AnalyzerOptions.Config["ctu-dir"] = CTUDir;
            AnalyzerOptions.Config["ctu-index-name"] = CTUIndex;
            
            // (선택적) CTU 호출 목록 파일 지정
            // AnalyzerOptions.Config["ctu-invocation-list"] = CTUDir + "/invocations.yaml";
        }
        // ===== CTU 설정 끝 =====

        // ... (ento::CreateAnalysisConsumer 호출) ...
    }
    #endif

    // ... (ClangTidyASTConsumer 생성 및 반환) ...
}
```

---

### Phase 2: 옵션 연동

#### 2.1. `ClangTidyMain.cpp`: 커맨드라인 옵션 추가

`clang-tidy.exe`가 `--enable-ctu` 인자를 받도록 합니다.

C++

```
// clang-tools-extra/clang-tidy/tool/ClangTidyMain.cpp
static cl::OptionCategory ClangTidyCategory("clang-tidy options");

// ... (기존 cl::opt들) ...

// ===== CTU 옵션 추가 =====
static cl::opt<bool> EnableCTU(
    "enable-ctu",
    cl::desc("Enable Cross Translation Unit (CTU) analysis"),
    cl::init(false),
    cl::cat(ClangTidyCategory));

static cl::opt<std::string> CTUDir(
    "ctu-dir",
    cl::desc("Path to CTU index directory"),
    cl::init("./ctu-dir"), // 기본값
    cl::cat(ClangTidyCategory));

// ... (main 함수 내부) ...
int clangTidyMain(int argc, const char **argv) {
    // ... (옵션 파싱) ...
    
    ClangTidyOptions EffectiveOptions = OptionsProvider->getGlobalOptions();

    // ===== CTU 옵션 설정 =====
    if (EnableCTU.getNumOccurrences() > 0)
        EffectiveOptions.EnableCTU = EnableCTU;
    if (CTUDir.getNumOccurrences() > 0)
        EffectiveOptions.CTUDir = CTUDir;

    // ... (runClangTidy 호출) ...
}
```

#### 2.2. `ClangTidyOptions.cpp`: `.clang-tidy` YAML 지원

`.clang-tidy` 파일에서 CTU를 켜고 끌 수 있게 합니다.

C++

```
// clang-tools-extra/clang-tidy/ClangTidyOptions.cpp
template <>
struct MappingTraits<ClangTidyOptions> {
    static void mapping(IO &IO, ClangTidyOptions &Options) {
        // ... (기존 매핑) ...
        
        // ===== CTU 옵션 추가 =====
        IO.mapOptional("EnableCTU", Options.EnableCTU);
        IO.mapOptional("CTUDir", Options.CTUDir);
        IO.mapOptional("CTUIndexName", Options.CTUIndexName);
    }
};
```

---

### Phase 3: ArqaStatic 연동

#### 3.1. `ClangTidyRunnerService.cs`: `--enable-ctu` 인자 전달

UI의 `EnableCTU` (bool) 값을 받아 `clang-tidy.exe`의 인자로 넘겨줍니다.

C#

```
// ArqaStatic.Services.ClangTidyRunnerService.cs
private string BuildClangTidyArguments(
    // ... (기존 인자들) ...
    bool enableCTU, // ⬅️ MainViewModel에서 전달받을 새 파라미터
    string projectPath // ⬅️ CTU 인덱스 경로 설정을 위한 프로젝트 경로
) {
    var argsList = new List<string>();
    // ... (기존 파일 목록, -p 옵션 등 추가) ...

    // ===== CTU 활성화 (수정된 clang-tidy 사용) =====
    if (enableCTU)
    {
        string ctuIndexPath = Path.Combine(projectPath, ".ctu-index"); // 예시 경로
        argsList.Add("--enable-ctu");
        argsList.Add($"--ctu-dir \"{ctuIndexPath}\"");
    }

    // ... (나머지 옵션들) ...
    return string.Join(" ", argsList);
}
```

#### 3.2. `MainWindow.xaml`: CTU 체크박스 추가

`MainViewModel`에 `EnableCTU` (bool) 속성을 추가하고, UI에 체크박스를 바인딩합니다.

XML

```
<CheckBox Content="CTU 분석 활성화 (파일 간 분석, 매우 느림)"
          IsChecked="{Binding EnableCTU}"
          ToolTip="함수 호출 경계를 넘어 버그를 탐지합니다. (예: Nullptr가 다른 파일에서 반환되는 경우)" />
```

---

### Phase 4: 커스텀 체커에서 CTU 활용

CTU가 활성화되면, Static Analyzer의 `CrossTranslationUnitContext`를 활용할 수 있게 됩니다.

C++

```
// YourCustomChecks/SafetyCfgNullDereferenceCheck.cpp
void SafetyCfgNullDereferenceCheck::check(
    const MatchFinder::MatchResult &Result) 
{
    const auto *CallExpr = Result.Nodes.getNodeAs<CallExpr>("call");
    if (!CallExpr) return;

    // 호출된 함수의 선언(Declaration) 가져오기
    if (const auto *CalleeDecl = CallExpr->getDirectCallee()) {
        
        // 함수 정의(Definition) 가져오기
        // 1. 현재 파일(TU)에서 정의(body)를 찾습니다.
        const FunctionDecl *Def = CalleeDecl->getDefinition();

        // 2. 현재 파일에 정의가 없다면 (외부 함수라면)
        if (!Def || !Def->hasBody()) {
            
            // 3. ===== CTU를 통해 외부 정의를 가져옵니다. =====
            // (Static Analyzer의 CrossTranslationUnitContext를 가져와야 함)
            // 실제 구현은 Clang의 ento::CrossTranslationUnitContext 인프라를
            // 체커에 주입하거나 접근해야 합니다.
            //
            // const ento::CrossTranslationUnitContext *CTUCtx = 
            //    getAnalysisContext()->getCrossTranslationUnitContext();
            // if (CTUCtx) {
            //     Def = CTUCtx->getDefinition(CalleeDecl);
            // }
        }

        // 4. 가져온 함수 정의(Def)를 분석합니다.
        if (Def && Def->hasBody()) {
            // 예: 이 함수의 반환 값이 Null인지 등을 분석
            // analyzeFunctionBody(Def, Result.Context);
        }
    }
}
```

---

## 4. 빌드 및 테스트 전략 🧪

### 4.1. 빌드 (부분 빌드)

LLVM 전체를 다시 빌드할 필요 없이, `clang-tidy` 타겟만 재빌드하여 시간을 절약합니다.

Bash

```
# llvm-project/build 디렉토리에서
cmake --build . --target clang-tidy --config Release
```

_결과물: `build/bin/clang-tidy.exe` (모든 기능이 통합된 바이너리)_

### 4.2. 테스트

제시해주신 예제(`file1.cpp`, `file2.cpp`)로 CTU 기능을 검증합니다.

**`file1.cpp`**

C++

```
// file1.cpp
int* dangerous_function() {
    return nullptr;
}
```

**`file2.cpp`**

C++

```
// file2.cpp
extern int* dangerous_function();
int main() {
    int* p = dangerous_function();
    *p = 10;  // ⬅️ CTU 없이는 감지 불가, CTU 활성화 시 감지되어야 함
    return 0;
}
```

**테스트 커맨드:**

Bash

```
# 1. compile_commands.json 생성 (CMake 또는 기타 빌드 시스템)
# (프로젝트 루트에 build/compile_commands.json가 있다고 가정)

# 2. CTU 없이 실행 (버그 탐지 실패 예상)
./build/bin/clang-tidy file1.cpp file2.cpp -p ./build

# 3. CTU 활성화 후 실행 (버그 탐지 성공 예상)
./build/bin/clang-tidy file1.cpp file2.cpp -p ./build --enable-ctu --ctu-dir=./ctu-temp
```

---

## 5. 결론

이 접근 방식(옵션 2 수정)은 LLVM 빌드 환경이 준비된 상황에서 가장 이상적입니다. ArqaStatic은 **CTU가 완벽하게 통합된 단일 `clang-tidy.exe`**만 배포하면 되며, 커스텀 체커의 성능을 극대화할 수 있습니다. 🚀