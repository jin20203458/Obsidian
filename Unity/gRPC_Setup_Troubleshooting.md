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
### 에러 메시지
```text
Library\PackageCache\com.cysharp.yetanotherhttphandler@3171cb204b00\YetAnotherHttpHttpContent.cs(16,75): error CS0246: The type or namespace name 'PipeReader' could not be found (are you missing a using directive or an assembly reference?)
```

### 실패 원인
- `YetAnotherHttpHandler` UPM 패키지는 내부적으로 고성능 IO 처리를 위해 .NET Core의 `System.IO.Pipelines.PipeReader` 클래스에 의존합니다.
- UPM Git 패키지 방식으로 들여온 패키지는 NuGet의 하위 종속 패키지(여기서는 `System.IO.Pipelines`)를 유니티 프로젝트에 자동으로 다운로드해주지 않습니다.
- 결국 프로젝트 내에 `System.IO.Pipelines` 라이브러리가 부재하여 컴파일 에러가 발생했고, 이로 인해 유니티가 Safe Mode에 빠져 정상 로드가 불가능했습니다.

---

## 3. 해결 방법
1. **의존성 명시적 추가**
   - `System.IO.Pipelines` 패키지를 `Packages/packages.config`에 수동으로 선언하여 `NuGetForUnity`가 강제로 다운로드하도록 유도했습니다.
   - **수정된 [packages.config](file:///C:/Users/adg01/Documents/GitHub/MundusVivens.Unity/Packages/packages.config) 파일:**
     ```xml
     <?xml version="1.0" encoding="utf-8"?>
     <packages>
       <package id="Grpc.Net.Client" version="2.67.0" />
       <package id="Google.Protobuf" version="3.29.3" />
       <package id="System.IO.Pipelines" version="8.0.0" />
     </packages>
     ```

2. **유니티 에디터 리트라이 (Retry)**
   - packages.config 수정 후 에디터를 재로드하여 `NuGetForUnity`가 `System.IO.Pipelines`를 복원하도록 하였습니다.
   - 결과적으로 의존성이 모두 해소되어 **컴파일 에러가 소거되고 빌드가 정상화**되었습니다.
