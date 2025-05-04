- 리눅스 서버가 윈도우 네트워크 환경과 파일 및 프린터를 공유할 수 있도록 하는 서비스.
# smbclient 명령어 상세 설명

`smbclient`는 리눅스에서 SMB/CIFS 프로토콜을 사용하여 Windows 또는 SAMBA 서버에 연결하고 파일을 관리할 수 있게 해주는 명령줄 유틸리티입니다.

```
smbclient [옵션] 서버/공유이름 [암호] [-c 명령어]
```

## 주요 옵션

- `-L, --list=HOST`: 지정한 호스트에서 사용 가능한 공유 목록을 표시
- `-U, --user=USERNAME`: 접속할 사용자 이름 지정
- `-W, --workgroup=WORKGROUP`: 작업 그룹/도메인 이름 지정
- `-N, --no-pass`: 비밀번호 프롬프트 없이 접속(null 비밀번호)
- `-A, --authentication-file=FILE`: 인증 정보가 있는 파일 지정
- `-c, --command=COMMAND`: 하나의 명령을 실행하고 종료
- `-P, --machine-pass`: 시스템의 머신 계정 비밀번호 사용
- `-d, --debuglevel=DEBUGLEVEL`: 디버그 레벨 설정(0-10)
- `-p, --port=PORT`: 연결할 포트 지정(기본값: 445/139)
## 연결 흐름

1. 사용 가능한 공유 확인: `smbclient -L 서버`
2. 특정 공유에 연결: `smbclient //서버/공유`
3. 인증 정보 입력 (요청 시)
4. `smb: \>` 프롬프트에서 명령 실행
5. `exit` 또는 `quit`으로 종료

SMB(Server Message Block) 프로토콜은 Microsoft Windows 네트워크 파일 공유의 기본 프로토콜이며, SAMBA는 이를 리눅스/유닉스 시스템에서 구현한 것입니다.


`smb.conf`는 **삼바(Samba) 서버의 설정 파일**로, 리눅스와 윈도우 간 파일 공유를 관리하는 데 사용됩니다. 이 파일은 `/etc/samba/smb.conf`에 위치하며, 삼바 서버의 동작을 정의하는 다양한 옵션을 포함하고 있습니다.

### ✅ `smb.conf`의 주요 섹션

`smb.conf`는 **세 가지 주요 섹션**으로 구성됩니다:

1. **[global]**: 삼바 서비스의 전체적인 설정을 정의합니다.
    
2. **[디렉터리명]**: 특정 공유 디렉터리에 대한 설정을 저장합니다. 예: `[home]`
    
3. **[printers]**: 프린터 공유 방식을 설정합니다.
    

### ✅ 주요 설정 옵션

- **workgroup**: 삼바 서버가 속할 작업 그룹을 지정합니다.
    
- **server string**: 삼바 서버의 설명을 설정합니다.
    
- **security**: 인증 방식 (`user`, `share`, `server`, `domain`)을 지정합니다.
    
- **hosts allow**: 삼바 서버에 접근할 수 있는 IP 주소를 지정합니다.
    
- **log file**: 삼바 서버의 로그 파일 위치를 설정합니다.
    
- **max log size**: 로그 파일의 최대 크기를 지정합니다.
    
- **encrypt passwords**: 암호화된 패스워드를 사용할지 여부를 설정합니다.
    
- **[web]** → 공유 이름 (접근 시 사용하는 이름)
    
- **path = ...** → 실제 시스템 상의 디렉터리 경로
### ✅ 예제 설정

ini

```
[global]
   workgroup = SAMBA
   security = user
   passdb backend = tdbsam
   log file = /var/log/samba/log.%m
   max log size = 50

[homes]
   comment = Home Directories
   valid users = %S
   browseable = No
   read only = No
   inherit acls = Yes
```

이 설정은 삼바 서버가 **사용자 인증을 요구하며**, 홈 디렉터리를 공유하는 방식으로 동작하도록 구성합니다.