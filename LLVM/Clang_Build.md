## Windows에서 LLVM/Clang 전체 도구 체인 빌드 가이드 (GitHub Desktop 사용)

이 가이드는 **GitHub Desktop**과 명령줄을 함께 사용하여 Windows에서 LLVM/Clang의 전체 개발 및 분석 도구를 빌드하는 구체적인 과정입니다123.

## 사전 준비

- **GitHub Desktop**, **CMake**, **Python**, **Visual Studio 2022** ("Desktop development with C++" 워크로드 포함)가 설치되어 있어야 합니다4.
    

## 1단계: LLVM 저장소 클론 (GitHub Desktop 사용)

명령줄 대신 GitHub Desktop의 그래픽 인터페이스를 사용하여 LLVM 소스 코드를 로컬 컴퓨터에 복제합니다[5](https://docs.github.com/en/desktop/adding-and-cloning-repositories/cloning-a-repository-from-github-to-github-desktop)[6](https://heardlibrary.github.io/digital-scholarship/manage/control/github/clone/)[7](https://docs.github.com/en/desktop/adding-and-cloning-repositories/cloning-and-forking-repositories-from-github-desktop).

1. **GitHub Desktop 실행**: 프로그램을 엽니다.
    
2. **저장소 클론 메뉴 선택**: 상단 메뉴에서 **File** > **Clone Repository...**를 클릭합니다[7](https://docs.github.com/en/desktop/adding-and-cloning-repositories/cloning-and-forking-repositories-from-github-desktop).
    
3. **URL로 클론**:
    
    - 나타나는 창에서 **URL** 탭을 선택합니다.
        
    - URL 입력란에 LLVM 공식 저장소 주소를 붙여넣습니다:  
        `https://github.com/llvm/llvm-project.git`
        
    - **Local path** 입력란 옆의 **Choose...** 버튼을 눌러 소스 코드를 저장할 로컬 폴더(예: `C:\Dev\llvm-project`)를 선택합니다.
        
4. **클론 실행**: **Clone** 버튼을 클릭합니다. GitHub Desktop이 자동으로 소스 코드를 다운로드합니다[5](https://docs.github.com/en/desktop/adding-and-cloning-repositories/cloning-a-repository-from-github-to-github-desktop).
    

**성공 확인:** 잠시 후 GitHub Desktop의 메인 화면에 'llvm-project' 저장소가 나타나고, 지정한 로컬 폴더에 소스 코드 파일들이 생성됩니다.


## 2단계: CMake 빌드 구성 (Configure)

이 과정이 가장 중요합니다. 빌드에 포함할 모든 도구와 옵션을 여기서 설정합니다.

text

```PowerShell
# 1. 빌드 전용 폴더 생성 및 이동
# (이미 폴더를 만들었다면 이 부분은 건너뛰고 cd build만 실행하세요)
mkdir build
cd build

# 2. CMake로 빌드 시스템 구성 (모든 도구 포함)
# (PowerShell에서는 백틱(`)을 사용하여 여러 줄로 입력합니다)
cmake -G "Visual Studio 17 2022" -A x64 `
      -DLLVM_ENABLE_PROJECTS="clang;clang-tools-extra;compiler-rt;lld" `
      -DLLVM_TARGETS_TO_BUILD="X86" `
      -DCMAKE_BUILD_TYPE=Release `
      -DLLVM_ENABLE_ASSERTIONS=ON `
      ../llvm

```

**성공 확인:** `build` 폴더 안에 `LLVM.sln`이라는 Visual Studio 솔루션 파일이 생성되면 성공입니다. 오류 메시지 없이 "Configuring done" 및 "Generating done" 메시지가 출력됩니다.


## 3단계: 빌드 실행 (PowerShell)

이제 구성된 내용을 바탕으로 실제 컴파일을 진행합니다. 이 과정은 시스템 사양에 따라 1시간 이상 소요될 수 있습니다.


```PowerShell
# 1. 빌드 시작 (CPU 코어 8개를 사용하여 병렬 빌드) 
cmake --build . --config Release --parallel 8`
```
**팁:** `--parallel` 뒤의 숫자는 사용하는 CPU 코어 수에 맞게 조절하면 빌드 속도가 빨라집니다. (예: 12코어 CPU -> `--parallel 12`)


**성공 확인:** 마지막에 오류 메시지 없이 빌드 프로세스가 100% 완료되면 성공입니다.


## 4단계: 빌드 결과 확인 (PowerShell)

빌드가 완료되면 우리가 필요로 하는 실행 파일들이 생성되었는지 확인합니다.

```PowerShell
# 1. 빌드된 실행 파일이 있는 폴더로 이동
cd Release\bin

# 2. 주요 도구 존재 여부 확인 (ls는 Get-ChildItem의 별칭입니다) 
ls clang.exe ls clang-tidy.exe 
# 3. 도구 버전 확인으로 최종 검증 (.\는 현재 폴더의 파일을 의미합니다) 
.\clang.exe --version .\clang-tidy.exe --version
```


**성공 확인:** 각 명령어 실행 시 버전 정보가 정상적으로 출력되면 모든 과정이 성공적으로 완료된 것입니다.

## 5단계: 환경 변수 설정 (PowerSEt PATH)

빌드된 도구들을 어느 위치에서든 편하게 사용하기 위해 시스템 `PATH`에 경로를 추가합니다.

```PowerShell
# 현재 PowerShell 세션에서만 임시로 경로 설정 
$env:PATH += ";C:\Dev\llvm-project\build\bin\Release" 

# 시스템에 영구적으로 경로 추가 (관리자 권한 PowerShell 필요, 새 세션부터 적용됨) 
# 중요: 이 명령어는 현재 PATH의 끝에 새 경로를 추가합니다. 
setx PATH "$($env:PATH);C:\Dev\llvm-project\build\bin\Release" /M
```

**성공 확인:** **새로운 PowerShell 창을 열고** `clang --version`을 입력했을 때 버전 정보가 나오면 성공입니다.

이 과정을 통해 새로운 정적 분석 도구 개발과 테스트에 필요한 모든 LLVM/Clang 구성 요소를 한 번에 효율적으로 구축했습니다.