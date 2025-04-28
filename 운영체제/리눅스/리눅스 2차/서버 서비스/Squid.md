- 웹 요청을 중개하여 캐싱, 접근 제어, 보안 기능을 제공하는 프록시 서버 소프트웨어
`squid`의 주요 설정을 도표 형태로 정리한 내용을 아래에 제공합니다:

| **설정 항목**                | **설명**                          | **예시**                                                                         |
| ------------------------ | ------------------------------- | ------------------------------------------------------------------------------ |
| **http_port**            | 프록시 서버가 사용할 포트 설정               | `http_port 8080`                                                               |
| **acl**                  | 접근 제어 목록 설정, 특정 네트워크나 IP 허용/차단  | `acl allowed_ips src 192.168.1.0/24`                                           |
| **http_access**          | 특정 acl에 대해 요청 허용 또는 차단 설정       | `http_access allow allowed_ips`                                                |
| **cache_dir**            | 캐시 저장소 설정, 디렉토리 경로와 크기 설정       | `cache_dir ufs /var/spool/squid 100 16 256`                                    |
| **visible_hostname**     | 외부에서 표시될 프록시 서버 호스트 이름 설정       | `visible_hostname proxyserver`                                                 |
| **cache_mem**            | `squid`가 사용할 메모리 캐시 크기 설정       | `cache_mem 64 MB`                                                              |
| **logfile_rotate**       | 로그 파일의 회전 주기 설정                 | `logfile_rotate 10`                                                            |
| **tcp_outgoing_address** | 외부 네트워크에 요청할 때 사용할 IP 주소 설정     | `tcp_outgoing_address 192.168.1.1`                                             |
| **dns_nameservers**      | `squid`에서 사용할 DNS 서버 설정         | `dns_nameservers 8.8.8.8 8.8.4.4`                                              |
| **auth_param**           | 사용자 인증 설정 (사용자 인증을 통한 접근 제어 설정) | `auth_param basic program /usr/lib/squid/basic_ncsa_auth /etc/squid/passwords` |

