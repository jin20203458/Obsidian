

### **모든 `.cpp` 파일을 한 번에 검사하는 방법**

가장 안정적인 방법은 PowerShell이 와일드카드를 처리하여 파일 목록을 `clang++`에 명시적으로 전달하도록 하는 것입니다.

**단계별 명령어:**

1. **새 PowerShell 창을 엽니다.** (권장)
    
2. `C:\Users\adg01\Desktop\MTest\MTest` 디렉토리로 이동합니다.
    
    PowerShell
    
    ```PowerShell
    cd C:\Users\adg01\Desktop\MTest\MTest
    ```
    
3. **`PATH` 환경 변수를 설정합니다. (필수):**
    
    PowerShell
    
    ```PowerShell
    $env:PATH += ";C:\Users\adg01\Documents\GitHub\llvm-project\build\bin"
    ```
    
    **확인:** `$env:PATH`를 입력하여 경로가 제대로 추가되었는지 다시 한번 확인합니다.
    
4. **분석 결과를 받을 임시 디렉토리를 생성하고 그 안으로 이동합니다.**
    
    PowerShell
    
    ```PowerShell
    Remove-Item -Path "temp_html_output_all" -Recurse -Force -ErrorAction SilentlyContinue
    mkdir temp_html_output_all
    cd temp_html_output_all
    ```
    
    _이전과 구분을 위해 폴더 이름을 `temp_html_output_all`로 변경했습니다._
    
5. 모든 .cpp 파일 목록을 가져와서 clang++에 인자로 전달합니다.
    
    PowerShell의 Get-ChildItem (줄여서 gci 또는 ls) cmdlet을 사용하여 .cpp 파일 목록을 가져온 후, ForEach-Object (%) 또는 $() 구문을 사용하여 이를 clang++에 전달할 수 있습니다.
    
    PowerShell
    
    ```PowerShell
    # 방법 1: `Get-ChildItem`과 배열을 사용 (가장 안정적)
    $cpp_files = Get-ChildItem -Path ".." -Filter "*.cpp" | Select-Object -ExpandProperty FullName
    clang++ --analyze -Xclang -analyzer-output=html -std=c++17 @cpp_files
    ```
    
    - `$cpp_files = Get-ChildItem -Path ".." -Filter "*.cpp" | Select-Object -ExpandProperty FullName`
        
        - `Get-ChildItem -Path ".." -Filter "*.cpp"`: 현재 디렉토리의 상위 디렉토리(`..`)에서 모든 `.cpp` 파일을 찾습니다.
            
        - `Select-Object -ExpandProperty FullName`: 찾은 각 파일의 전체 경로를 문자열로 가져옵니다.
            
        - `$cpp_files` 변수에 이 전체 경로들의 배열이 저장됩니다.
            
    - `@cpp_files`: PowerShell에서 배열 앞에 `@`를 붙이면, 배열의 각 요소를 별도의 인자로 명령에 전달합니다. 즉, `clang++ File1.cpp File2.cpp MTest.cpp` 처럼 됩니다.
        
    
    **또는, 좀 더 직접적인 방법 (PowerShell이 제대로 확장해주길 기대):**
    
    PowerShell
    
    ```PowerShell
    # 방법 2: 백틱 없이 한 줄로, 명시적인 와일드카드 확장 시도
    # (PowerShell의 버전이나 설정에 따라 다르게 작동할 수 있음)
    clang++ --analyze -Xclang -analyzer-output=html -std=c++17 ..\*.cpp
    ```
    
    - `..\*.cpp`를 PowerShell이 `..\File1.cpp ..\File2.cpp ..\MTest.cpp`와 같이 확장하여 `clang++`에 전달하기를 기대하는 방식입니다. 방법 1보다 실패할 확률이 약간 더 있습니다.
        
    
    **저는 방법 1을 가장 추천합니다. PowerShell에서 파일 목록을 다룰 때 가장 견고한 방법입니다.**
    
6. **분석 완료 후, 생성된 HTML 보고서 확인:**
    
    - 명령어가 성공적으로 실행되면, 현재 디렉토리인 `C:\Users\adg01\Desktop\MTest\MTest\temp_html_output_all` 안에 `File1.html`, `File2.html`, `MTest.html` 등의 개별 보고서 파일이 생성될 것입니다.
        
    - 이 파일들을 웹 브라우저로 엽니다:
        
    
    PowerShell
    
    ```PowerShell
    Invoke-Item File1.html
    Invoke-Item File2.html
    Invoke-Item MTest.html
    # 또는 모든 HTML을 한 번에 열려면:
    Get-ChildItem -Filter "*.html" | ForEach-Object { Invoke-Item $_.FullName }
    ```
    

이 방법을 사용하면 모든 `.cpp` 파일을 효율적으로 검사하고 HTML 보고서를 얻을 수 있을 것입니다.