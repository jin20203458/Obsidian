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