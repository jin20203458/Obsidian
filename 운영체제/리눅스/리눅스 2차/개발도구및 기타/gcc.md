**`gcc`** (GNU Compiler Collection)는 주로 C, C++, Fortran 등의 프로그래밍 언어로 작성된 코드를 컴파일하는 데 사용되는 툴입니다. 다양한 **옵션**을 통해 컴파일, 디버깅, 최적화 등을 제어할 수 있습니다. `gcc` 명령어와 그 옵션들에 대해 자세히 설명하겠습니다.

`gcc [옵션] [파일] [옵션]`

- **`-c`**: 소스 파일을 컴파일만 하고 링크하지 않음.
- **`-o <파일명>`**: 출력 파일의 이름을 지정.
- **`-Wall`**: 가능한 모든 경고 메시지를 출력.
- **`-g`**: 디버깅 정보를 추가하여 디버깅 가능한 실행 파일 생성.
- **`-O` / `-O1` / `-O2` / `-O3` / `-Os`**: 최적화 수준을 지정하여 성능을 개선.
- **`-std=<표준>`**: 사용하고자 하는 C 언어 표준을 지정.
- **`-I<디렉토리>`**: 추가적인 헤더 파일 경로 지정.
- **`-L<디렉토리>`**: 추가적인 라이브러리 경로 지정.
- **`-l<라이브러리>`**: 외부 라이브러리 링크.
- **`-D<매크로>`**: 컴파일 중 매크로 정의.
- **`-E`**: 전처리 단계만 수행.
- **`-S`**: 어셈블리 코드로 변환.

✅ **1. 간단한 C 프로그램 컴파일**

bash

```
gcc -o my_program my_program.c
```

- `-o my_program` → 출력 파일 이름을 `my_program`으로 설정.
    
- `my_program.c` → C 소스 코드 파일을 컴파일.
    

👉 **결과**: 실행 파일 `my_program`이 생성됩니다.

✅ **2. 여러 개의 파일을 컴파일**

bash

```
gcc -o my_program file1.c file2.c
```

- `file1.c`, `file2.c` 두 개의 소스 코드 파일을 컴파일하여 `my_program` 실행 파일을 생성.
    

👉 **결과**: 두 개의 파일이 링크되어 하나의 실행 파일로 생성됩니다.

✅ **3. 오브젝트 파일만 생성 (**`-c` **옵션 사용)**

bash

```
gcc -c file1.c
gcc -c file2.c
gcc -o my_program file1.o file2.o
```

- `gcc -c` → 개별 `.c` 파일을 **오브젝트 파일(**`.o`**)로 변환**.
    
- `gcc -o my_program file1.o file2.o` → 오브젝트 파일을 링크하여 실행 파일 생성.
    

👉 **결과**: `file1.o`, `file2.o`가 생성되고, `my_program` 실행 파일이 만들어짐.

✅ **4. 최적화 옵션 적용**

bash

```
gcc -O2 -o optimized_program my_program.c
```

- `-O2` → 최적화 옵션(코드 실행 속도를 높임).
    
- `optimized_program` → 최적화된 실행 파일 생성.
    

👉 **결과**: 일반 컴파일보다 **더 빠르게 실행되는 프로그램** 생성.

✅ **5. 디버깅 정보 포함 (**`-g` **옵션 사용)**

bash

```
gcc -g -o debug_program my_program.c
```

- `-g` → 디버깅을 위한 심볼 정보를 추가.
    
- 디버거(GDB)로 프로그램 실행 가능.
    

👉 **결과**: `debug_program` 실행 파일을 `gdb`를 사용하여 디버깅 가능.

📌 **정리**

|명령어|설명|
|---|---|
|`gcc -o 실행파일 소스파일.c`|C 프로그램 컴파일|
|`gcc -c 소스파일.c`|오브젝트 파일만 생성|
|`gcc -O2 -o 실행파일 소스파일.c`|최적화된 실행 파일 생성|
|`gcc -g -o 실행파일 소스파일.c`|디버깅 가능한 실행 파일 생성|
