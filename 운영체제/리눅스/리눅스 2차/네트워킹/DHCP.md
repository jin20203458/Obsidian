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