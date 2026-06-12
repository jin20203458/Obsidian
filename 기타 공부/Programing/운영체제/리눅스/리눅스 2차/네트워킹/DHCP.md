Dynamic Host Configuration Protocol

네트워크에 연결된 장치들이 자동으로 IP 주소와 기타 네트워크 설정을 받을 수 있도록 해주는 프로토콜입니다. 일반적으로 DHCP는 IP 주소, 서브넷 마스크, 기본 게이트웨이, DNS 서버 등 네트워크 설정을 자동으로 할당하여 사용자가 직접 설정을 입력할 필요를 없애줍니다.

```# 기본 설정
ddns-update-style interim;
ignore client-updates;

# 서브넷 정의
subnet 192.168.1.0 netmask 255.255.255.0 {
  range 192.168.1.10 192.168.1.50;  # 할당할 IP 범위
  option routers 192.168.1.1;      # 기본 게이트웨이
  option domain-name-servers 8.8.8.8, 8.8.4.4;  # DNS 서버
  option domain-name "example.com";  # 도메인 이름
}

# 고정 IP 주소 할당 예시
host static-host {
  hardware ethernet 00:1A:2B:3C:4D:5E;  # MAC 주소
  fixed-address 192.168.1.100;  # 할당할 고정 IP
}
```

EX)
```
ddns-update-style interim;
ignore client-updates;
subnet 192.168.0.0 netmask 255.255.255.0 {
  option routers 192.168.0.1;            # ①: 게이트웨이 정보
  option subnet-mask 255.255.255.0;      # ②: 서브넷 마스크 정보
  option domain-name "example.com";      # ③: 도메인 이름
  option domain-name-servers 192.168.1.1; # ④: 도메인 네임 서버 IP 주소
  option time-offset       -18000;
  option default-lease-time  21600;
}
```

`dhcpd.conf` 파일은 **DHCP 서버 설정 파일**로, 네트워크에서 **IP 주소를 자동으로 할당**하는 방법을 정의합니다. 이 파일에서는 IP 주소 풀, 고정 IP 할당, 네트워크 인터페이스 설정 등을 관리할 수 있습니다.

`dhcpd.conf` 파일의 주요 항목을 설명하고, 앞서 언급된 예시를 포함한 구성을 설명하겠습니다.

### 1. **기본 구조 및 설정**

`dhcpd.conf` 파일은 보통 다음과 같은 기본적인 구성을 가집니다:
```
# 기본 설정
option domain-name "example.com";        # 도메인 이름 설정
option domain-name-servers 192.168.1.1;  # DNS 서버 설정
default-lease-time 600;                  # 기본 임대 시간 설정 (초)
max-lease-time 7200;                     # 최대 임대 시간 설정 (초)

# 서브넷 설정
subnet 192.168.1.0 netmask 255.255.255.0 {
    range 192.168.1.10 192.168.1.50;  # 동적 IP 주소 풀 설정
    option routers 192.168.1.1;       # 기본 게이트웨이 설정
}
```

- **option domain-name**: 네트워크의 도메인 이름을 지정합니다.
    
- **option domain-name-servers**: 클라이언트가 사용할 DNS 서버 주소를 설정합니다.
    
- **default-lease-time**: 클라이언트가 IP 주소를 얼마나 오래 사용할 수 있는지를 정의하는 기본 임대 시간입니다.
    
- **max-lease-time**: 클라이언트가 IP 주소를 요청할 때, 그 주소를 사용할 수 있는 최대 시간입니다.
    
- **subnet**: 서브넷에 대한 설정을 정의합니다. 이 블록 안에 IP 주소 풀(`range`)과 기본 게이트웨이 설정(`routers`) 등을 정의할 수 있습니다.


### 2. **고정 IP 주소 할당**

`dhcpd.conf`에서 특정 클라이언트에게 고정 IP 주소를 할당하는 설정 방법은 다음과 같습니다.
```
host ihd_com {
    hardware ethernet 00:1A:2B:3C:4D:5E;  # MAC 주소로 클라이언트를 구별
    fixed-address 192.168.1.100;            # 고정 IP 주소 할당
}
```

- **host**: `host` 블록은 특정 장치에 대해 고정 IP를 할당할 때 사용됩니다.
    
- **hardware ethernet**: 클라이언트의 MAC 주소를 지정하여 해당 장치를 고유하게 식별합니다.
    
- **fixed-address**: 이 클라이언트에게 할당할 고정 IP 주소를 설정합니다.
    

### 3. **IP 주소 풀 설정**

다음은 IP 주소 범위를 지정하여 동적 IP를 할당하는 설정 예시입니다.
```
subnet 192.168.1.0 netmask 255.255.255.0 {
    range 192.168.1.100 192.168.1.200;  # IP 주소 풀 범위
    option routers 192.168.1.1;         # 기본 게이트웨이 설정
}
```
- **range**: 이 블록은 동적으로 할당할 수 있는 IP 주소 범위를 설정합니다.
    
- **option routers**: 클라이언트가 사용할 기본 게이트웨이를 지정합니다.
    

### 4. **서브넷 마스크 및 라우터 설정**

서브넷 마스크와 라우터를 지정하는 예시입니다.
```
subnet 192.168.1.0 netmask 255.255.255.0 {
    option routers 192.168.1.1;       # 기본 라우터(게이트웨이)
    option subnet-mask 255.255.255.0; # 서브넷 마스크 설정
}
```

- **option subnet-mask**: 서브넷 마스크를 설정합니다.
    
- **option routers**: 네트워크의 기본 게이트웨이를 지정합니다.
    

### 5. **IP 주소 예약 설정**

특정 클라이언트에 대해 예약된 IP 주소를 할당하는 설정 예시입니다.
```
host reserved_host {
    hardware ethernet 00:1A:2B:3C:4D:5E;  # 특정 클라이언트의 MAC 주소
    fixed-address 192.168.1.150;            # 예약된 IP 주소
}
```

- **host reserved_host**: `reserved_host`라는 이름을 가진 호스트를 정의합니다.
    
- **hardware ethernet**: 특정 MAC 주소를 가진 클라이언트를 지정합니다.
    
- **fixed-address**: 이 클라이언트에게 예약된 IP 주소를 할당합니다.
    

### 6. **다양한 옵션 설정**

추가적으로 사용할 수 있는 다양한 옵션들입니다.

- **option broadcast-address**: 브로드캐스트 주소를 설정합니다.
    
- **option domain-name-servers**: DNS 서버 주소를 설정합니다.
    
- **option ntp-servers**: NTP 서버 주소를 설정합니다.
    
- **option time-offset**: 시간대 오프셋을 설정합니다.
```
option broadcast-address 192.168.1.255;
option domain-name-servers 8.8.8.8, 8.8.4.4;
```

```
option domain-name "example.com";
option domain-name-servers 192.168.1.1, 8.8.8.8;

subnet 192.168.1.0 netmask 255.255.255.0 {
    range 192.168.1.100 192.168.1.200;
    option routers 192.168.1.1;
    option subnet-mask 255.255.255.0;
}

host ihd_com {
    hardware ethernet 00:1A:2B:3C:4D:5E;
    fixed-address 192.168.1.100;
}
```
### 요약

- `dhcpd.conf` 파일은 DHCP 서버에서 IP 주소를 어떻게 할당할지 정의하는 설정 파일입니다.
    
- **고정 IP 주소 할당**은 `host` 블록 내에서 **MAC 주소**와 함께 `fixed-address` 옵션으로 설정합니다.
    
- **동적 IP 할당**은 `subnet` 블록 내에서 `range` 옵션으로 설정하며, **서브넷 마스크**와 **라우터** 정보도 함께 설정합니다.
    
- 다양한 옵션들을 통해 DHCP 서버의 동작을 세밀하게 조정할 수 있습니다