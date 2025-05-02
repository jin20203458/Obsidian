### 📁 `/var/log` 주요 로그 파일 설명

| 로그 파일                           | 설명                                                   |
| ------------------------------- | ---------------------------------------------------- |
| **`/var/log/messages`**         | 시스템 전반의 일반 로그 메시지 (커널, 서비스 등). RHEL 계열에서 가장 기본적인 로그. |
| **`/var/log/syslog`**           | (Debian/Ubuntu 계열) 시스템 전반의 로그. RHEL에는 기본적으로 없음.      |
| **`/var/log/secure`**           | 인증 관련 로그. `ssh`, `su`, `sudo` 등 보안 이벤트 기록.           |
| **`/var/log/maillog`**          | 메일 시스템 관련 로그 (`postfix`, `sendmail` 등).              |
| **`/var/log/cron`**             | `cron` 데몬에서 실행한 정기 작업의 기록.                           |
| **`/var/log/dmesg`**            | 부팅 시 커널 메시지를 기록한 로그. 하드웨어 관련 정보 확인에 유용.              |
| **`/var/log/yum.log`**          | `yum` 패키지 매니저로 설치/업데이트된 내역. (RHEL 계열)                |
| **`/var/log/audit/audit.log`**  | SELinux, `auditd`와 관련된 고급 보안 감사 로그. (보안 감사 시 사용)     |
| **`/var/log/httpd/access_log`** | Apache 웹 서버 접근 기록. 누가 언제 어떤 요청을 보냈는지 확인 가능.          |
| **`/var/log/httpd/error_log`**  | Apache 웹 서버 오류 로그. 설정 오류, 요청 처리 중 문제 등 기록.           |
| **`/var/log/btmp`**             | 실패한 로그인 시도 기록 (바이너리 파일. `lastb` 명령으로 확인).            |
| **`/var/log/wtmp`**             | 로그인/로그아웃 등 사용자 세션 기록 (바이너리. `last` 명령으로 확인).         |
| **`/var/log/lastlog`**          | 각 사용자 마지막 로그인 정보 (`lastlog` 명령으로 확인).                |
| `/var/log/boot.log`             | 부팅 시 실행된 서비스들의 상태를 기록                                |

---

### 🔧 유용한 로그 확인 명령어

- `tail -f /var/log/messages` – 실시간 로그 모니터링
    
- `grep ssh /var/log/secure` – SSH 로그인 시도 기록 검색
    
- `journalctl` – systemd 기반 로그 확인 (RHEL 7 이후 권장)