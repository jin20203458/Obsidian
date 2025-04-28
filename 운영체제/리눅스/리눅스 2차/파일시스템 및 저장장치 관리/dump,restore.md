## dump 명령어

`dump`는 유닉스/리눅스 시스템에서 파일시스템의 백업을 생성하는 전통적인 유틸리티입니다.

### 주요 옵션

- `-0` ~ `-9`: 덤프 레벨을 지정합니다
    - `0`: 전체 백업(모든 파일)
    - `1-9`: 증분 백업(마지막 낮은 레벨 백업 이후 변경된 파일만)
- `-f 파일명`: 백업 파일을 지정합니다
- `-u`: 백업 후 /etc/dumpdates 파일을 업데이트합니다
- `-a`: 자동 크기 계산을 비활성화합니다
- `-B 바이트`: 덤프 레코드의 크기를 지정합니다
- `-b 블록`: 볼륨 크기를 블록 단위로 지정합니다
- `-h 레벨`: 해당 레벨보다 중요한 파일만 백업합니다
- `-j`: 압축하여 백업합니다(gzip 형식)

```
# 전체 백업 후 dumpdates 업데이트
dump -0uf /backup/fullbackup.dump /dev/sda1

# 마지막 레벨 0 백업 이후 변경된 파일만 백업
dump -1uf /backup/incremental.dump /dev/sda1

# 테이프 장치에 백업
dump -0f /dev/st0 /home
```

---
## restore 명령어

`restore`는 `dump` 명령으로 생성된 백업에서 파일을 복원하는 유틸리티입니다.

### 주요 옵션

- `-r`: 전체 파일시스템 복원(파일시스템을 처음부터 새로 만듦)
- `-i`: 대화형 모드로 특정 파일만 선택적으로 복원
- `-t`: 백업 내용만 표시(실제 복원하지 않음)
- `-x`: 지정한 파일/디렉토리만 추출
- `-f 파일명`: 입력 백업 파일을 지정
- `-C`: 파일 내용을 비교하는 모드
- `-y`: 프롬프트 없이 진행(yes로 응답)

```
# 전체 파일시스템 복원
cd /mnt/newdisk  # 새 파일시스템으로 이동
restore -rf /backup/fullbackup.dump

# 대화형 모드로 특정 파일만 복원
restore -if /backup/fullbackup.dump

# 백업 내용 확인만
restore -tf /backup/fullbackup.dump

# 특정 디렉토리만 복원
restore -xf /backup/fullbackup.dump ./home/user
```