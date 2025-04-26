


| **설정 항목**          | **설명**                                                            | **예시**                                       |
| ------------------ | ----------------------------------------------------------------- | -------------------------------------------- |
| **Listen**         | 웹서버가 수신할 포트 번호를 지정합니다.                                            | `Listen 80`                                  |
| **ServerName**     | 웹서버의 도메인 이름을 설정합니다. (도메인 또는 IP)                                   | `ServerName www.example.com`                 |
| **ServerRoot**     | 웹서버의 설치 디렉토리입니다.                                                  | `ServerRoot "/usr/local/apache"`             |
| **DocumentRoot**   | 웹서버의 기본 웹 콘텐츠가 위치하는 디렉토리입니다. (인덱스 페이지 등)                          | `DocumentRoot "/var/www/html"`               |
| **Directory**      | 특정 디렉토리의 접근 권한 및 설정을 정의합니다.                                       | `<Directory "/var/www/html">`                |
| **ErrorLog**       | 서버 오류 로그를 기록할 파일을 지정합니다.                                          | `ErrorLog "/var/log/httpd/error_log"`        |
| **LogLevel**       | 로그의 세부 수준을 지정합니다. (예: `debug`, `info`, `warn`, `error`, `crit` 등) | `LogLevel warn`                              |
| **AccessLog**      | 접근 로그를 기록할 파일을 지정합니다.                                             | `AccessLog "/var/log/httpd/access_log"`      |
| **ServerAdmin**    | 웹서버의 관리자 이메일 주소를 지정합니다.                                           | `ServerAdmin webmaster@example.com`          |
| **ErrorDocument**  | 특정 HTTP 오류 발생 시 사용자에게 보여줄 커스텀 페이지를 설정합니다.                         | `ErrorDocument 404 /404.html`                |
| **Timeout**        | 서버가 클라이언트 요청을 기다리는 최대 시간을 설정합니다. (초 단위)                           | `Timeout 300`                                |
| **KeepAlive**      | 클라이언트와의 연결을 유지할지 여부를 설정합니다.                                       | `KeepAlive On`                               |
| **MaxClients**     | 최대 동시 클라이언트 연결 수를 설정합니다. (서버의 최대 처리량 설정)                          | `MaxClients 150`                             |
| **VirtualHost**    | 가상 호스트를 설정하여 여러 도메인에 대해 하나의 서버를 운영할 수 있게 합니다.                     | `<VirtualHost *:80>`                         |
| **DocumentRoot**   | 가상 호스트의 웹 콘텐츠가 위치하는 디렉토리를 설정합니다.                                  | `DocumentRoot "/var/www/vhosts/example.com"` |
| **ServerAlias**    | 가상 호스트에 대한 추가 도메인 이름을 설정합니다.                                      | `ServerAlias www.example.net`                |
| **DirectoryIndex** | 웹서버가 기본적으로 로드할 인덱스 파일을 설정합니다.                                     | `DirectoryIndex index.html index.php`        |
| **UserDir**        |                                                                   |                                              |

### 간단한 예시 설정:
```
Listen 80
ServerName www.example.com
ServerRoot "/usr/local/apache"
DocumentRoot "/var/www/html"
ErrorLog "/var/log/httpd/error_log"
AccessLog "/var/log/httpd/access_log"
LogLevel warn
```