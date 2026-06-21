# Unity gRPC 연동 및 YetAnotherHttpHandler 컴파일 에러 해결 기록

- **작성일**: 2026-06-18
- **프로젝트**: MundusVivens.Unity
- **작업 목적**: Unity 클라이언트에서 gRPC 통신을 사용하기 위해 `Grpc.Net.Client` + `YetAnotherHttpHandler` 기술 스택 연동 및 빌드 오류 해결

---

## 1. 시도한 작업 (Setup Process)
1. **UPM 패키지 추가 (`Packages/manifest.json`)**
   - 비동기 처리를 위한 `UniTask` 추가.
   - HTTP/2 통신 핸들러인 `YetAnotherHttpHandler` 추가.
   - 외부 NuGet 패키지 관리를 위한 `NuGetForUnity` 추가.
   - *(이 과정에서 NuGetForUnity의 패키지 이름 규칙 오류(`com.github.glitchenzo.nugetforunity` -> `com.github-glitchenzo.nugetforunity`)를 수정하여 패키지 해소 문제를 1차 해결함)*

2. **NuGet 패키지 선언 (`Packages/packages.config`)**
   - `Grpc.Net.Client` (2.67.0) 및 `Google.Protobuf` (3.29.3)를 추가하여 클라이언트가 gRPC 통신 및 직렬화를 처리할 수 있도록 선언.

3. **로컬 패키지 격리 (`.gitignore`)**
   - NuGet을 통해 받아오는 라이브러리 파일이 저장소로 유출되지 않도록 `/Assets/Packages/` 및 `/Assets/Packages.meta`를 git ignore 목록에 추가.

---

## 2. 발생한 문제 및 실패 원인
### 에러 1: `PipeReader/Pipelines` 누락 (Safe Mode 교착 상태)
* **에러 메시지**:
  ```text
  Library\PackageCache\com.cysharp.yetanotherhttphandler@3171cb204b00\YetAnotherHttpHttpContent.cs(16,75): error CS0246: The type or namespace name 'PipeReader' could not be found (are you missing a using directive or an assembly reference?)
  Library\PackageCache\com.cysharp.yetanotherhttphandler@3171cb204b00\NativeHttpHandlerCore.cs(4,17): error CS0234: The type or namespace name 'Pipelines' does not exist in the namespace 'System.IO'
  ```
* **원인**: UPM 패키지는 하위 NuGet 종속성을 자동으로 복원하지 않습니다. 이로 인해 컴파일 에러가 발생하여 유니티가 Safe Mode에 갇혔고, 결과적으로 에디터 백그라운드가 멈춰 `NuGetForUnity`가 복원 기능을 수행하지 못하는 교착 상태가 발생했습니다.

### 에러 2: gRPC 프로토콜 HTTP/1.1 다운그레이드 에러
* **에러 메시지**:
  ```text
  [gRPC] Test call failed: Status(StatusCode="Internal", Detail="Bad gRPC response. Response protocol downgraded to HTTP/1.1.")
  ```
* **원인**: 로컬 개발 환경에서 TLS(HTTPS)를 사용하지 않는 비보안 HTTP (`http://localhost:5001`) 주소를 사용하여 통신할 때, `YetAnotherHttpHandler`가 기본 설정으로 인해 첫 핸드셰이크 단계에서 HTTP/2가 아닌 HTTP/1.1 프로토콜로 다운그레이드 요청을 처리하여 gRPC 통신 규격이 깨지게 됩니다.

---

## 3. 해결 방법
### 1. 의존성 로컬 강제 복원 (에러 1 해결)
임시 `.NET Standard 2.1` 프로젝트를 생성하여 필요한 NuGet 어셈블리(총 12개 DLL)를 수집 및 [Assets/Packages](file:///C:/Users/adg01/Documents/GitHub/MundusVivens.Unity/Assets/Packages) 폴더 내에 수동으로 주입하여 교착 상태를 해소했습니다.

#### 자동 복원 스크립트 (`restore_nuget.ps1`)
이 스크립트를 유니티 프로젝트 루트 폴더에 생성하고 실행하면, 임시 빌드 과정을 통해 필요한 gRPC 및 Pipelines 어셈블리를 `Assets/Packages/` 폴더에 즉시 셋업합니다.

```powershell
# Temporary NuGet restorer for Unity gRPC setup
$ErrorActionPreference = "Stop"

# 임시 빌드 디렉토리 및 유니티 대상 디렉토리 경로 지정
$TempDir = "$env:TEMP\MundusVivens_Restore_Temp"
$UnityDestDir = "$PSScriptRoot\Assets\Packages"

Write-Host "[Restore] Creating temporary .NET project at $TempDir..." -ForegroundColor Cyan

if (Test-Path $TempDir) {
    Remove-Item -Recurse -Force $TempDir
}
New-Item -ItemType Directory -Force -Path $TempDir | Out-Null

# Unity 호환용 netstandard2.1 타겟 프로젝트 생성
$CsprojContent = @"
<Project Sdk="Microsoft.NET.Sdk">
  <PropertyGroup>
    <TargetFramework>netstandard2.1</TargetFramework>
    <LangVersion>10.0</LangVersion>
    <ImplicitUsings>disable</ImplicitUsings>
    <Nullable>enable</Nullable>
  </PropertyGroup>

  <ItemGroup>
    <PackageReference Include="Grpc.Net.Client" Version="2.67.0" />
    <PackageReference Include="Google.Protobuf" Version="3.29.3" />
    <PackageReference Include="System.IO.Pipelines" Version="8.0.0" />
  </ItemGroup>
</Project>
"@

Set-Content -Path "$TempDir\RestoreProj.csproj" -Value $CsprojContent

Write-Host "[Restore] Running dotnet publish to gather all DLL dependencies..." -ForegroundColor Cyan
cmd.exe /c "cd /d `"$TempDir`" && dotnet publish -c Release"

$PublishDir = "$TempDir\bin\Release\netstandard2.1\publish"

if (-not (Test-Path $PublishDir)) {
    Write-Error "Dotnet publish output folder not found at $PublishDir"
}

Write-Host "[Restore] Copying collected DLLs to Unity project at $UnityDestDir..." -ForegroundColor Green

if (-not (Test-Path $UnityDestDir)) {
    New-Item -ItemType Directory -Force -Path $UnityDestDir | Out-Null
}

# 어셈블리 복사 (기본 시스템 라이브러리 제외)
$Files = Get-ChildItem -Path $PublishDir -Filter *.dll
foreach ($file in $Files) {
    $name = $file.Name
    if ($name -eq "System.dll" -or $name -eq "System.Core.dll" -or $name -eq "netstandard.dll" -or $name -eq "RestoreProj.dll") {
        Write-Host "[Skip] Skipping assembly: $name" -ForegroundColor Yellow
        continue
    }
    
    $destPath = "$UnityDestDir\$name"
    Write-Host "[Copy] Copying $name -> $destPath" -ForegroundColor Green
    Copy-Item -Path $file.FullName -Destination $destPath -Force
}

Write-Host "[Restore] Cleaning up temporary directory..." -ForegroundColor Gray
Remove-Item -Recurse -Force $TempDir

Write-Host "=======================================================" -ForegroundColor Green
Write-Host "Manual NuGet restore completed successfully!" -ForegroundColor Green
Write-Host "=======================================================" -ForegroundColor Green
```

#### 실행 방법
1. 유니티 프로젝트 루트 디렉토리에서 PowerShell을 열고 아래 명령어를 가동합니다:
   ```powershell
   powershell -ExecutionPolicy Bypass -File .\restore_nuget.ps1
   ```

### 2. HTTP/2 Cleartext 강제 설정 (에러 2 해결)
TLS가 미적용된 로컬 HTTP 연결 시 `YetAnotherHttpHandler`가 HTTP/2를 포기하지 않고 강제하도록 **`Http2Only = true`** 옵션을 활성화해야 합니다.

* **수정된 [GrpcTest.cs](file:///C:/Users/adg01/Documents/GitHub/MundusVivens.Unity/Assets/Scripts/GrpcTest.cs) 코드 구성**:
  ```csharp
  // YetAnotherHttpHandler 설정 시 Http2Only 속성을 true로 설정
  var handler = new YetAnotherHttpHandler()
  {
      Http2Only = true // TLS 비활성화 환경(http://)에서 HTTP/2 통신 강제
  };
  ```

이 설정을 추가함으로써 다운그레이드 없이 `http://localhost:5001`을 통한 HTTP/2 Cleartext (h2c) gRPC 연결이 완벽하게 성사됩니다.
