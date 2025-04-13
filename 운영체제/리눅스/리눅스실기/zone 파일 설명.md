`zone 파일`은 **DNS 서버**(특히 BIND 같은 데몬)가 도메인 이름을 IP 주소로 변환하거나 그 반대로 변환할 수 있도록 **DNS 레코드들을 저장해놓은 설정 파일**이에요.  
DNS의 핵심 데이터베이스라고 생각하면 됩니다.

```
$TTL    86400
@       IN SOA   ns1.example.com. root.example.com. (
                2025041301  ; serial
                3H          ; refresh
                15M         ; retry
                1W          ; expire
                1D )        ; minimum

        IN NS    ns1.example.com.
        IN NS    ns2.example.com.

        IN MX 10 mail.example.com.

@       IN A     192.168.10.1
www     IN A     192.168.10.2
mail    IN A     192.168.10.3
```

|항목|설명|
|---|---|
|`$TTL`|모든 레코드에 적용될 **기본 TTL(Time To Live)** 값 (초 단위)|
|`@`|현재 도메인 (예: `example.com.`) 을 의미함|
|`IN`|인터넷 클래스 (거의 항상 IN)|
|`SOA`|Start of Authority: 해당 도메인의 "시작 정보"를 정의|
|`NS`|네임서버(Name Server) 지정|
|`MX`|메일 서버(Mail eXchange) 지정|
|`A`|호스트 이름을 IP 주소로 연결|
|`CNAME`|도메인 별칭(Alias) 정의|
|`PTR`|IP 주소를 도메인 이름으로 역변환 (보통 별도 zone에서 정의)|
