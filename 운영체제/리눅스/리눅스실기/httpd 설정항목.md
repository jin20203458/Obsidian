

| 설정 항목                  | 의미 및 역할                                | 예시 설정                                          |
| ---------------------- | -------------------------------------- | ---------------------------------------------- |
| `Listen`               | HTTP 서버가 **수신 대기(listen)**할 포트 지정      | `Listen 80`, `Listen 8080`                     |
| `ServerName`           | 서버의 **도메인 이름 또는 IP주소**를 지정             | `ServerName www.ihd.or.kr`                     |
| `DocumentRoot`         | 웹 문서(HTML 등)가 위치한 **루트 디렉터리 경로** 지정    | `DocumentRoot "/var/www/html"`                 |
| `ServerAdmin`          | 서버 관리자 이메일 주소                          | `ServerAdmin admin@ihd.or.kr`                  |
| `DirectoryIndex`       | 클라이언트가 디렉터리에 접근했을 때 **기본으로 표시될 파일** 지정 | `DirectoryIndex index.html`                    |
| `<Directory>`          | 특정 디렉터리에 대해 **접근 제어나 설정**을 적용하는 블록     | `<Directory "/var/www/html"> ... </Directory>` |
| `ErrorLog`             | **에러 로그**를 기록할 파일 경로                   | `ErrorLog logs/error_log`                      |
| `CustomLog`            | **접속 로그** 포맷과 기록 파일 경로 지정              | `CustomLog logs/access_log combined`           |
| `ServerTokens`         | 응답 헤더에서 서버 정보의 **표시 수준** 설정            | `ServerTokens Prod`                            |
| `ServerSignature`      | 오류 페이지 하단에 서버 정보 표시 여부                 | `ServerSignature Off`                          |
| `Timeout`              | 클라이언트 요청에 대한 **응답 제한 시간 (초)** 설정       | `Timeout 60`                                   |
| `KeepAlive`            | **연결을 유지할지** 여부 설정 (성능 향상 목적)          | `KeepAlive On`                                 |
| `MaxKeepAliveRequests` | 유지 연결당 최대 요청 수                         | `MaxKeepAliveRequests 100`                     |
| `KeepAliveTimeout`     | 유지 연결이 끊기기 전까지의 **유지 시간(초)**           | `KeepAliveTimeout 15`                          |
