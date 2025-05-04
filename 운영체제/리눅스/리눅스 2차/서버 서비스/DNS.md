
## **named.conf**
`/etc/named.conf` 파일은 **DNS 서버(BIND)의 주요 환경 설정 파일**입니다. 이 파일을 통해 **DNS 서버의 동작 방식, 존(Zone) 파일 위치, 접근 제어 정책** 등을 정의할 수 있습니다.

### ✅ 주요 역할

1. **DNS 서버의 기본 설정**을 정의
    
2. **존(Zone) 파일 및 리버스 존(Reverse Zone) 파일**을 지정
    
3. **ACL(Access Control List) 설정**을 통해 접근 제어
    
4. **캐싱 DNS 서버 설정** 가능
    

### ✅ 기본 구조 예시

bash

```
options {
    directory "/var/named";
    allow-query { any; };
    listen-on port 53 { 192.168.1.1; };
};

zone "example.com" {
    type master;
    file "example.com.zone";
};

zone "1.168.192.in-addr.arpa" {
    type master;
    file "example.com.rev";
};
```

### ✅ 주요 설정 항목

- `options`: DNS 서버의 기본 설정을 정의
    
- `zone`: 특정 도메인에 대한 존(Zone) 파일을 지정
    
- `allow-query`: 특정 IP 또는 네트워크만 DNS 질의를 허용
    
- `listen-on`: 특정 IP에서만 DNS 요청을 수신
    
`allow-transfer`**는 DNS 서버 간 영역 데이터를 동기화하는 데 사용되며, 보안 강화를 위해 특정 서버 또는 키를 사용하여 제한**

📌 **즉,** `/etc/named.conf`**는 BIND DNS 서버의 핵심 설정 파일이며, 이를 통해 DNS 서버의 동작을 제어할 수 있습니다**.