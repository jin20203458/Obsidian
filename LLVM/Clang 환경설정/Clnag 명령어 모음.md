
```shell
#모든 체커를 활성화(*)했을 때 어떤 것들이 있는지 목록
clang-tidy --list-checks -checks="*"
```

```shell
# 'bugprone'과 'ARQA' 모듈에 속한 체커들만 목록으로 보여줍니다.
clang-tidy --list-checks -checks="bugprone-*,ARQA-*"
```

```shell
# 특정 파일을 기본 설정으로 분석하고 컴파일 옵션을 전달합니다.
# '--' 뒤에 컴파일러 옵션을 명시해야 합니다.
clang-tidy main.cpp -- -std=c++20 -I./include
```

```shell
# 모든 체커를 비활성화(-*)한 후, 'readability'와 'modernize' 모듈의 체커만 활성화하여 분석합니다.
clang-tidy main.cpp -checks="-*,readability-*,modernize-*" --
```

```shell
# 분석 후 자동으로 수정 가능한 항목을 코드에 바로 적용합니다.
clang-tidy main.cpp -checks="modernize-use-auto" -fix
```

```shell
# 분석 후 자동으로 수정 가능한 항목을 코드에 바로 적용합니다.
clang-tidy main.cpp -checks="modernize-use-auto" -fix
```

```shell
# compile_commands.json을 사용하여 컴파일 옵션을 자동으로 가져옵니다.
# (CMake에서 -DCMAKE_EXPORT_COMPILE_COMMANDS=ON 으로 생성)
clang-tidy -p build/ src/main.cpp
```

```shell
# 현재 분석에 적용되는 모든 설정을 YAML 형식으로 덤프하여 확인합니다.
clang-tidy main.cpp --dump-config
```

```shell
# 각 설정이 어디서(기본값, 파일, 명령줄) 비롯되었는지 출처를 확인합니다.
clang-tidy main.cpp --explain-config
```