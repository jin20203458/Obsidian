
**Apache 웹 서버 설정**

Apache는 가장 널리 사용되는 오픈 소스 웹 서버 중 하나입니다. 웹 서버는 클라이언트(웹 브라우저 등)의 요청을 받아 웹 페이지, 이미지, 비디오 등 웹 콘텐츠를 제공하는 역할을 합니다. Apache 설정은 주로 텍스트 파일들을 수정하여 이루어지며, 서버의 동작 방식, 보안, 성능 등을 제어합니다.

**주요 설정 파일 (일반적인 경우):**

- **`/etc/httpd/conf/httpd.conf` (경로는 배포판에 따라 다를 수 있습니다.)**: Apache의 **메인 설정 파일**입니다. 서버의 전반적인 동작 방식, 포트 설정, 모듈 로드, 기본 디렉터리 설정, 가상 호스트 설정 등을 정의합니다.

- **`/etc/httpd/conf.d/` (또는 `/etc/apache2/conf-available/` 등)**: 메인 설정 파일에서 include 되는 **추가 설정 파일**들이 위치하는 디렉터리입니다. 각 모듈별 설정, 가상 호스트 설정 등을 분리하여 관리하는 데 유용합니다.

- **`/etc/httpd/conf/httpd-vhosts.conf` (또는 `/etc/apache2/sites-available/` 등)**: **가상 호스트 (Virtual Host)** 설정을 정의하는 파일입니다. 하나의 Apache 서버에서 여러 개의 도메인 이름 또는 IP 주소를 호스팅할 때 사용됩니다.

- **`.htaccess`**: 각 디렉터리별로 설정을 변경할 수 있는 **분산 설정 파일**입니다. 권한 제어, 리다이렉션, MIME 타입 설정 등을 해당 디렉터리 내에서 적용할 수 있습니다. (메인 설정에서 허용해야 활성화됨)


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
| **UserDir**        | 아파치(Apache) 웹 서버 설정에서 "**일반 사용자 계정**"의 개인 웹사이트 디렉터리를 지정           | `UserDir www`<br>                            |

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

|**상태 코드**|**설명**|**주요 원인**|**예시**|
|---|---|---|---|
|**200 OK**|요청 성공|정상적인 요청 처리|웹페이지 정상 로딩|
|**201 Created**|리소스 생성 성공|POST 요청으로 새 데이터 생성|회원가입 완료|
|**204 No Content**|요청 성공, 응답 본문 없음|요청은 성공했지만 반환할 데이터 없음|데이터 삭제 후 응답|
|**301 Moved Permanently**|URL 영구 이동|리소스가 영구적으로 다른 위치로 이동|도메인 변경|
|**302 Found**|URL 임시 이동|리소스가 임시적으로 다른 위치로 이동|로그인 후 리디렉션|
|**400 Bad Request**|잘못된 요청|요청 형식 오류, 파라미터 문제|잘못된 API 호출|
|**401 Unauthorized**|인증되지 않음|로그인 정보 없음 또는 인증 실패|로그인 없이 보호된 페이지 접근|
|**403 Forbidden**|접근 권한 없음|인증 성공했지만 접근 권한 없음|관리자 페이지 접근 제한|
|**404 Not Found**|리소스 없음|요청한 URL이 존재하지 않음|삭제된 웹페이지 요청|
|**409 Conflict**|충돌 발생|중복 데이터 등록, 리소스 충돌|동일한 사용자명 등록 시 오류|
|**500 Internal Server Error**|서버 내부 오류|서버 측 문제 발생|서버 다운 또는 코드 오류|
|**503 Service Unavailable**|서비스 이용 불가|서버 과부하 또는 점검 중|서버 유지보수 중|

