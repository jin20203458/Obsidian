`tar` 명령어는 **리눅스에서 파일 및 디렉터리를 하나의 아카이브 파일로 묶거나 압축하는 명령어**입니다. `Tape ARchive`의 약자로, 주로 백업 및 파일 전송에 사용됩니다.

### ✅ 기본 사용법

bash

```
tar [옵션] [아카이브 파일명] [파일 및 디렉터리]
```

### ✅ 주요 옵션

- `-c` : 새로운 아카이브 파일 생성
    
- `-x` : 아카이브 파일 해제(압축 해제)
    
- `-f <파일명>` : 아카이브 파일 이름 지정
    
- `-v` : 실행 과정 출력 (verbose)
    
- `-z` : gzip 압축 사용
    
- `-j` : bzip2 압축 사용
    
- `-t` : 아카이브 파일 내부 목록 확인
    
- `-r` : 기존 아카이브 파일에 새로운 파일 추가
    
- `-u` : 기존 아카이브 파일에서 수정된 파일 업데이트
    
- `-C <디렉터리>` : 특정 디렉터리에서 압축 파일 생성 또는 해제
    

### ✅ 예제

1️⃣ **디렉터리 및 파일 압축**

bash

```
tar -czvf backup.tar.gz /home/user/documents
```

- `/home/user/documents` 디렉터리를 `backup.tar.gz` 파일로 압축
    

2️⃣ **압축 해제**

bash

```
tar -xzvf backup.tar.gz
```

- `backup.tar.gz` 파일을 현재 디렉터리에 해제
    

3️⃣ **압축 파일 내부 확인**

bash

```
tar -tvf backup.tar.gz
```

- `backup.tar.gz` 내부 파일 목록 출력
    

4️⃣ **압축 파일에 새로운 파일 추가**

bash

```
tar -rvf backup.tar newfile.txt
```

- `backup.tar` 파일에 `newfile.txt` 추가
    

5️⃣ **특정 날짜 이후 수정된 파일만 압축**

bash

```
tar -N 2023-01-01 -cvf archive.tar /home/user/documents
```

- 2023년 1월 1일 이후 수정된 파일만 `archive.tar`에 포함