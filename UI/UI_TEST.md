
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
using System.IO;
using System.Text.Json;

namespace SentinelStatic.Services
{
    /// <summary>
    /// JSON 형식으로 설정을 저장하고 불러오는 서비스 클래스입니다.
    /// </summary>
    public class JsonSettingsService : ISettingsService
    {

        /// settings.json 파일의 전체 경로
        private readonly string _filePath;

        public JsonSettingsService()
        {
            // 1. 사용자별 애플리케이션 데이터 폴더 경로 가져오기
            string appDataPath = Environment.GetFolderPath(Environment.SpecialFolder.ApplicationData);
            // 결과 예시 (Windows): "C:\Users\YourUsername\AppData\Roaming"

            // 2. 우리 앱의 고유 폴더 경로 생성
            string appDir = Path.Combine(appDataPath, "SentinelStatic");
            // 결과 예시: "C:\Users\YourUsername\AppData\Roaming\SentinelStatic"

            // 3. 폴더가 없으면 생성하기
            Directory.CreateDirectory(appDir);
            // 이 메서드는 폴더가 이미 존재하면 아무것도 하지 않으므로 안전합니다.

            // 4. 최종 설정 파일 경로를 멤버 변수에 저장
            _filePath = Path.Combine(appDir, "settings.json");
            // 결과 예시: "C:\Users\YourUsername\AppData\Roaming\SentinelStatic\settings.json"
        }
        public AppSettings LoadSettings()
        {
            // 1. 설정 파일이 존재하는지 확인
            if (!File.Exists(_filePath))
            {
                // 파일이 없으면 (앱을 처음 실행한 경우), 기본값이 담긴 새로운 AppSettings 객체를 반환.
                return new AppSettings();
            }
            try
            {
                // 2. 파일이 있으면, 모든 텍스트를 읽어온다.
                string json = File.ReadAllText(_filePath);

                // 3. 읽어온 JSON 문자열을 AppSettings C# 객체로 변환(역직렬화)한다.
                // 만약 파일 내용이 비어있거나 잘못되어 null이 반환되면, 안전하게 기본 객체를 반환한다.
                return JsonSerializer.Deserialize<AppSettings>(json) ?? new AppSettings();
            }
            catch // JSON 파싱 실패, 파일 읽기 권한 없음 등 모든 예외를 잡는다.
            {
                // 어떤 종류의 오류가 발생하든, 프로그램이 죽지 않고 기본 설정을 반환하여 안전하게 실행을 계속한다.
                return new AppSettings();
            }
        }
        public void SaveSettings(AppSettings settings)
        {
            // 1. JSON 직렬화 옵션을 설정한다. (사람이 읽기 좋게 들여쓰기 적용)
            var options = new JsonSerializerOptions { WriteIndented = true };

            // 2. 전달받은 AppSettings 객체를 JSON 문자열로 변환(직렬화)한다.
            string json = JsonSerializer.Serialize(settings, options);

            // 3. 변환된 JSON 문자열을 파일에 쓴다. (파일이 존재하면 덮어쓴다)
            File.WriteAllText(_filePath, json);
        }

    }
}
```