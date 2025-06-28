`/etc/mail/access` 파일은 **Sendmail의 액세스 제어 파일**로, 메일 서버가 수신하거나 중계(relay)할 메일을 도메인, IP 주소, 사용자 등의 기준에 따라 **허용하거나 차단**하도록 설정할 수 있는 중요한 구성 파일입니다.

---

### 🔹 주요 역할

- **스팸 차단**: 특정 도메인이나 IP에서 오는 메일을 거부하거나 무시할 수 있음.
    
- **릴레이 제어**: 외부에서 메일 서버를 중계 서버로 사용하는 것을 제어함.
    
- **특정 동작 지정**: 수신/발신 메일에 대해 REJECT, DISCARD, OK, RELAY 등 다양한 동작 지정 가능.
    

---

### 🔹 형식

plaintext

코드 복사

`key         action`

- `key`: 검사 대상 (예: IP 주소, 도메인, 이메일 주소, 접두어 포함 키워드 등)
    
- `action`: Sendmail이 해당 키에 대해 수행할 동작
    

---

### 🔹 사용 가능한 key 유형 예시

|key 형식|설명|
|---|---|
|`From:example.com`|발신자가 example.com일 경우|
|`To:example.com`|수신자가 example.com일 경우|
|`example.com`|해당 도메인과 관련된 모든 요청|
|`192.0.2.1`|특정 IP 주소|
|`Connect:192.0.2.0`|연결 시도하는 IP 대역에 대한 제어|
|`User:user@example.com`|특정 사용자 제어 (드물게 사용)|

---

### 🔹 action 종류

| action      | 설명                          |
| ----------- | --------------------------- |
| `REJECT`    | 해당 메일을 거부하고 오류 응답           |
| `DISCARD`   | 해당 메일을 조용히 버림 (발신자는 알 수 없음) |
| `OK`        | 해당 항목을 허용                   |
| `RELAY`     | 메일 릴레이를 허용 (신중히 사용)         |
| `ERROR: 설명` | 사용자 정의 오류 메시지 반환            |

```
From:spam.com         REJECT      # spam.com 도메인에서 오는 메일 거부
192.168.1             RELAY       # 192.168.1.* 대역에서 메일 릴레이 허용
good.com              OK          # good.com은 무조건 허용
Connect:badhost.com   REJECT      # 해당 호스트에서의 연결 자체 거부

```