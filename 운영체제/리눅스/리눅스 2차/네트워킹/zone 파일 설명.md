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


**Start of Authority**
```
@ IN SOA ns1.example.com. admin.example.com. (  ; SOA 레코드 시작: 권한 있는 네임서버와 관리자 정보
    2025042001  ; 시리얼 번호 (Serial Number) - 존(zone) 파일 수정 시마다 증가시켜야 함
    1D          ; 리프레시 (Refresh) - 슬레이브 DNS가 마스터로부터 정보를 갱신하는 주기 (1일)
    1H          ; 리트라이 (Retry) - 갱신 실패 시 재시도 간격 (1시간)
    1W          ; 만료 (Expire) - 슬레이브 DNS가 더 이상 갱신을 시도하지 않고 데이터 사용 중단까지의 시간 (1주일)
    3H          ; 최소값 (Minimum TTL) - 캐시된 레코드가 살아있는 최소 시간 (3시간)
)

```

| 항목      | 설명                                            |
| ------- | --------------------------------------------- |
| `$TTL`  | 모든 레코드에 적용될 **기본 TTL(Time To Live)** 값 (초 단위) |
| `@`     | 현재 도메인 (예: `example.com.`) 을 의미함              |
| `IN`    | 인터넷 클래스 (거의 항상 IN)                            |
| `SOA`   | Start of Authority: 해당 도메인의 "시작 정보"를 정의       |
| `NS`    | 네임서버(Name Server) 지정                          |
| `MX`    | 메일 서버(Mail eXchange) 지정                       |
| `A`     | 호스트 이름을 IP 주소로 연결                             |
| `CNAME` | 도메인 별칭(Alias) 정의                              |
| `PTR`   | IP 주소를 도메인 이름으로 역변환 (보통 별도 zone에서 정의)         |
