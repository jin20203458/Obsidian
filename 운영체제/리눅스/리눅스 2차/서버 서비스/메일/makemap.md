
`makemap` 명령어는 **Sendmail에서 사용되는 데이터베이스 맵을 생성하는 도구**야. 주로 **키-값 형태의 데이터를 저장**하고, Sendmail이 이를 조회할 수 있도록 해줘.

✅ **기본 사용법**

bash

```
makemap -t hash /etc/mail/access < access.txt
```

➝ `access.txt` 파일을 기반으로 **해시(hash) 형식의 데이터베이스 맵**을 생성하여 `/etc/mail/access.db` 파일로 저장.

✅ **주요 옵션**

- `-t` : 데이터베이스 형식 지정 (`hash`, `btree`, `dbm` 등)
    
- `-o` : 기존 데이터베이스 파일을 덮어쓰기
    
- `-u` : 데이터베이스 내용을 텍스트로 덤프
    
- `-d` : 중복 키 허용 (B-tree 형식에서만 가능)
    
- `-f` : 키의 대문자를 소문자로 변환하지 않음
    
- `-s` : 안전 검사 무시
    

✅ **예제** 1️⃣ **Sendmail의 access.db 파일 생성**

bash

```
makemap -t hash /etc/mail/access < access.txt
```

➝ `access.txt`를 기반으로 `/etc/mail/access.db` 생성.

2️⃣ **데이터베이스 내용을 확인**

bash

```
makemap -u hash /etc/mail/access.db
```

➝ `access.db` 파일의 내용을 텍스트로 출력.

✔️ **요약**

- `makemap`은 **Sendmail에서 키-값 데이터베이스를 생성하는 명령어**.
    
- `-t` 옵션으로 **데이터베이스 형식**을 지정할 수 있음.
    
- `-u` 옵션을 사용하면 **데이터베이스 내용을 확인** 가능.