`rsync`는 **리눅스 및 UNIX 시스템에서 파일을 복사하거나 백업할 때 사용하는 강력한 도구**입니다. 특히 **증분 복사** 기능을 지원하여 이미 복사된 파일의 변경된 부분만을 전송함으로써 **네트워크 트래픽을 줄이고 속도를 향상**시킵니다.

### ✅ 기본 사용법

bash

```
rsync [옵션] 원본 경로 대상 경로
```

예를 들어, `/home/user/data` 디렉터리를 `/backup/data`로 복사하려면:

bash

```
rsync -av /home/user/data /backup/data
```

이 명령어는 **파일 권한, 소유자, 수정 시간 등을 유지하면서 복사**합니다.

### ✅ 주요 옵션

- `-a` : 아카이브 모드 (권한, 소유자, 시간을 유지)
    
- `-r` : 하위 디렉터리까지 모두 복사
    
- `-v` : 진행 상황을 자세히 출력
    
- `-z` : 네트워크 전송 시 압축하여 속도 향상
    
- `--delete` : 원본에 없는 파일을 대상에서 삭제
    
- `-P` : 진행 상황 표시 및 부분 복사 허용
    
- `-e` : 원격 셸(SSH 등)을 사용하여 복사
    
- `--exclude '*.txt'` : 특정 파일을 제외하고 복사
    
- `--bwlimit=5000` : 네트워크 대역폭을 초당 5000KB로 제한
    

### ✅ 사용 예시

1️⃣ **로컬에서 다른 디렉터리로 복사**

bash

```
rsync -av /home/user/data /backup/data
```

2️⃣ **원격 서버로 파일 복사**

bash

```
rsync -av -e ssh /home/user/data user@remote:/backup/data
```

3️⃣ **원격 서버에서 로컬로 파일 복사**

bash

```
rsync -av user@remote:/backup/data /home/user/data
```

4️⃣ **삭제된 파일을 동기화**

bash

```
rsync -av --delete /home/user/data /backup/data
```

5️⃣ **네트워크 속도를 제한하여 복사**

bash

```
rsync -av --bwlimit=5000 /home/user/data /backup/data
```

📌 `rsync`는 **백업 및 동기화 작업을 자동화하는 데 매우 유용**하며, 특히 원격 서버와 데이터를 동기화할 때 많이 사용됩니다.