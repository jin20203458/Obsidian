## OSI 7 계층 구조

데이터는 송신 측의 7계층에서 1계층으로 내려가면서 각 계층의 정보(헤더)가 추가(캡슐화)되고, 수신 측에서는 1계층에서 7계층으로 올라가면서 정보가 제거(역캡슐화)됩니다.[](https://www.cloudflare.com/learning/ddos/glossary/open-systems-interconnection-model-osi/)​

|계층 번호|계층 이름|주요 기능|데이터 단위|주요 장비 및 프로토콜|
|---|---|---|---|---|
|**7계층**|**응용 계층 (Application Layer)**|사용자와 직접 상호작용하며, 이메일, 웹 브라우징 등 네트워크 서비스를 제공[](https://www.cloudflare.com/learning/ddos/glossary/open-systems-interconnection-model-osi/)​.|데이터 (Data)|HTTP, FTP, SMTP, DNS|
|**6계층**|**표현 계층 (Presentation Layer)**|데이터의 형식을 변환(번역), 압축, 암호화 및 복호화를 수행하여 응용 프로그램이 이해할 수 있도록 함[](https://www.cloudflare.com/learning/ddos/glossary/open-systems-interconnection-model-osi/)​.|데이터 (Data)|JPEG, MPEG, SSL, TLS|
|**5계층**|**세션 계층 (Session Layer)**|두 장치 간의 통신 세션을 생성, 관리, 동기화 및 종료하는 역할을 담당[](https://www.a1.digital/knowledge-hub/osi-model-and-its-7-layers-explained/)​.|데이터 (Data)|NetBIOS, RPC|
|**4계층**|**전송 계층 (Transport Layer)**|종단 간(End-to-End) 신뢰성 있고 정확한 데이터 전송을 보장. 데이터 분할(Segmentation) 및 재조립, 흐름 제어, 오류 제어 수행[](https://www.a1.digital/knowledge-hub/osi-model-and-its-7-layers-explained/)​.|세그먼트 (Segment)|TCP, UDP|
|**3계층**|**네트워크 계층 (Network Layer)**|여러 네트워크를 통해 데이터를 목적지까지 전달하는 최적의 경로(라우팅)를 결정. 논리적 주소(IP 주소)를 사용[](https://www.a1.digital/knowledge-hub/osi-model-and-its-7-layers-explained/)​.|패킷 (Packet)|라우터, IP, ICMP|
|**2계층**|**데이터 링크 계층 (Data Link Layer)**|물리적으로 연결된 두 노드 간의 신뢰성 있는 데이터 전송을 담당. 물리적 주소(MAC 주소)를 사용하며, 오류 검출 및 흐름 제어 수행[](https://aws.amazon.com/what-is/osi-model/)​.|프레임 (Frame)|스위치, 브리지, 이더넷|
|**1계층**|**물리 계층 (Physical Layer)**|데이터를 전기 신호, 빛, 또는 전파 등 물리적인 매체를 통해 전송. 데이터는 비트(0과 1) 스트림 형태로 표현됨[](https://www.geeksforgeeks.org/computer-networks/open-systems-interconnection-model-osi/)​.|비트 (Bit)|허브, 리피터, 케이블|