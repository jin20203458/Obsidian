/etc/mail/access

`/etc/mail/access` 파일은 **메일 서버에서 특정 호스트, 도메인, IP 주소에 대한 접근을 제어하는 설정 파일**입니다. 주로 **Sendmail**에서 사용되며, 스팸 차단 및 특정 도메인에 대한 허용/거부 정책을 설정하는 데 활용됩니다.

### ✅ 주요 기능

- 특정 **도메인** 또는 **IP 주소**에서 오는 메일을 **허용(RELAY)** 또는 **거부(REJECT)** 가능
    
- 특정 **발신자**의 메일을 **폐기(DISCARD)** 가능
    
- **스팸 필터링**을 위한 정책 설정 가능
    

### ✅ 설정 형식

파일 내부에서는 **도메인, IP 주소, 이메일 주소** 등을 지정하고, 해당 항목에 대한 동작을 설정할 수 있습니다.

plaintext

```
# 특정 도메인 허용
example.com    RELAY

# 특정 IP 주소 허용
192.168.1.10   RELAY

# 특정 도메인에서 오는 메일 거부
spamdomain.com REJECT

# 특정 이메일 주소 폐기 (발신자는 정상적으로 발송된 것으로 보이지만, 실제로는 폐기됨)
spammer@example.com DISCARD
```

### ✅ 설정 적용 방법

파일을 수정한 후, 변경 사항을 적용하려면 **access.db** 파일을 생성해야 합니다.

bash

```
makemap hash /etc/mail/access.db < /etc/mail/access
```

이 명령어를 실행하면 `access.db` 파일이 생성되며, Sendmail이 새로운 설정을 반영할 수 있습니다.

📌 **즉,** `/etc/mail/access` **파일은 메일 서버에서 특정 도메인, IP, 이메일 주소에 대한 접근을 제어하는 역할을 하며, 설정 변경 후 반드시** `makemap` **명령어를 실행해야 적용됩니다**2.

### ✅ 주요 옵션 (접두사) 설명

|키워드 (접두사)|설명|예시|
|---|---|---|
|`From:`|**메일 헤더의 From 필드**를 검사합니다. 주로 발신자 도메인 차단용입니다.|`From:spam.com DISCARD`|
|`Connect:`|**SMTP 연결을 시도하는 호스트의 IP나 도메인**을 기준으로 제어합니다.|`Connect:192.168.1.100 REJECT`|
|`To:`|**메일 수신자의 주소**를 기준으로 제어합니다. 보통 드물게 사용됩니다.|`To:user@example.com REJECT`|
|`Client:`|**HELO/EHLO 명령에서 사용된 호스트 이름**을 기준으로 제어합니다.|`Client:spambot.com REJECT`|
|`Relay:`|**릴레이 허용 여부를 결정**할 때 사용합니다. 내부 SMTP 릴레이 허용 등에 사용.|`Relay:192.168.0 OK`|
|(없음)|기본적으로 **Connect 기준**으로 동작합니다. 접두사 생략 시, 연결하는 호스트의 IP/도메인 기준입니다.|`spammer.com REJECT`|

---

### ✅ 주요 동작(Action) 예시

|동작|설명|
|---|---|
|`REJECT`|연결을 즉시 거부함.|
|`DISCARD`|수신은 하지만 **전달하지 않고 버림** (발신자는 성공한 줄 앎).|
|`OK`|명시적으로 허용함.|
|`RELAY`|릴레이 허용 (자신을 중계 서버로 허용).|

---

### ✅ 예시 정리

bash

코드 복사

`Connect:192.168.5.10      REJECT      # 특정 IP의 접속 차단 From:spamdomain.com       DISCARD     # 특정 도메인에서 온 메일 폐기 To:ceo@company.com        REJECT      # 특정 사용자에게 오는 메일 거부 Client:fakehost.net       REJECT      # HELO/EHLO로 위장한 호스트 차단 192.168.1.0               OK          # 기본 접속 허용 (접두사 없음, Connect 기준)`