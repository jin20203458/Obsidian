Telnet 설정파일 예시 (`/etc/xinetd.d/telnet`)
```
service telnet
{
 disable       = no
 flags         = REUSE
 socket_type   = stream
 wait          = no
 nice          = 5
 access_times  = 08:00-20:00
 per_source    = 7
 deny          = 10.20.16.0/24
 log_type      = FILE /var/log/telnetd.log
 server        = /usr/sbin/in.telnetd
}
```
### 🧾 각 항목 설명

|항목|의미|
|---|---|
|`service telnet`|telnet 서비스 설정 블록 시작|
|`disable = no`|telnet 서비스를 **활성화**함 (`yes`면 비활성화)|
|`flags = REUSE`|포트를 재사용 가능하게 설정 (빠른 연결/재연결에 유리)|
|`socket_type = stream`|TCP 프로토콜 사용 (연결형 통신)|
|`wait = no`|**멀티스레드/멀티프로세스 처리** 허용 (동시 접속 가능)|
|`nice = 5`|우선순위 설정 (0이 가장 높은 우선순위, 값이 클수록 낮음)|
|`access_times = 08:00-20:00`|하루 중 **08:00 ~ 20:00 사이에만 접속 허용**|
|`per_source = 7`|하나의 IP 주소당 **최대 7개 동시 접속 허용**|
|`deny = 10.20.16.0/24`|해당 네트워크 대역(서브넷)의 **접속을 차단**|
|`log_type = FILE /var/log/telnetd.log`|로그를 `/var/log/telnetd.log` 파일에 기록|
|`server = /usr/sbin/in.telnetd`|telnet 서버 데몬 경로|