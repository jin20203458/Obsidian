**iptables**는 **패킷 필터링 방화벽**으로, 패킷이 네트워크를 통해 이동하는 과정을 제어합니다.  
이 과정은 **체인(Chain)**이라는 개념을 사용하여 패킷을 검사하고 처리합니다. 각 체인은 **정해진 규칙에 따라** 패킷을 처리합니다.


## ✅ **iptables 체인 구조**

기본적으로 **3개의 주요 체인**이 있습니다.

| 체인          | 설명                                                          |
| ----------- | ----------------------------------------------------------- |
| **INPUT**   | 로컬 시스템으로 들어오는 패킷을 처리합니다. (`hostname`이나 `localhost`로 향하는 패킷) |
| **OUTPUT**  | 로컬 시스템에서 나가는 패킷을 처리합니다. (`localhost`에서 외부로 나가는 패킷)          |
| **FORWARD** | 시스템을 거쳐가는 패킷을 처리합니다. (예: 라우터처럼 다른 네트워크로 향하는 패킷)             |
| PREROUTING  | 패킷이 라우팅되기 전에 처리 (NAT에서 사용)                                  |
| POSTROUTING | 패킷이 라우팅된 후 처리 (NAT에서 사용)                                    |

이렇게 3개의 기본 체인 외에도 **사용자가 추가로 체인**을 정의할 수 있습니다.

---

## ✅ **iptables의 기본 동작 흐름**

##### 1. **패킷이 들어오면 `INPUT` 체인으로** → 로컬 시스템으로 들어오는 패킷에 대한 처리

##### 2. **패킷이 나가면 `OUTPUT` 체인으로** → 로컬 시스템에서 나가는 패킷에 대한 처리

##### 3. **패킷이 전달되면 `FORWARD` 체인으로** → 시스템을 거쳐서 다른 네트워크로 가는 패킷에 대한 처리

---

## ✅ **iptables 규칙 처리 순서**
```
입력 패킷        →  INPUT 체인 처리
출력 패킷        →  OUTPUT 체인 처리
포워딩 패킷      →  FORWARD 체인 처리
```

**도표로 나타내면:**
```
+-------------------+
|      패킷         |
|    (패킷 발생)    |
+-------------------+
        |
        V
   +-------------+
   |  INPUT 체인 |  <-- 로컬 시스템으로 들어오는 패킷
   +-------------+
        |
        V
   +-------------+
   |   OUTPUT    |  <-- 로컬 시스템에서 나가는 패킷
   +-------------+
        |
        V
   +-------------+
   |  FORWARD    |  <-- 시스템을 거쳐가는 패킷
   +-------------+
        |
        V
    [패킷 처리 완료]
```

### **`iptables` 명령어 구조**

**iptables 명령어**는 **체인**, **옵션**, **규칙**을 정의하는 형식으로 사용됩니다.
`iptables [옵션] [체인] [규칙]`
![[Pasted image 20250414182129.png]]

![[Pasted image 20250414182356.png]]
NAT를 써 출발지 주소를 바꾸는 설정
`iptables -t nat -A POSTROUTING -o eth0 -j SNAT --to 203.247.40.100`
NAT를 써 도착지 주소를 바꾸는 설정
`iptables -t nat -A PREROUTING -d 203.247.40.100 -p tcp --dport 80 -j DNAT --to-destination 192.168.1.100
`