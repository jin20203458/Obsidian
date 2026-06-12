`useradd` 명령어는 새로운 사용자 계정을 생성할 때 사용하는 Linux 명령어입니다. 주요 옵션과 설명을 정리하면 다음과 같습니다:

### **기본 사용법**

bash

```
useradd username
```

- `username`이라는 새 사용자 계정을 생성
    

### **주요 옵션**

bash

```
useradd -m username
```

- `-m`: 홈 디렉터리를 자동 생성 (`/home/username`)
    

bash

```
useradd -s /bin/bash username
```

- `-s`: 로그인할 때 사용할 기본 쉘 지정 (`/bin/bash` 등)
    

bash

```
useradd -d /custom/home username
```

- `-d`: 홈 디렉터리 위치 지정 (`/home/username` 대신 `/custom/home` 사용 가능)
    

bash

```
useradd -g groupname username
```

- `-g`: 사용자 계정을 특정 그룹에 추가 (`groupname`은 기존 그룹이어야 함)
    

bash

```
useradd -G wheel username
```

- `-G`: 추가 그룹 지정 (`wheel` 그룹에 포함시켜 sudo 권한 부여 가능)
    

### **기본값 확인 및 설정 변경**

bash

```
useradd -D
```

- 기본 설정값 확인 (`/etc/default/useradd` 참조)
    

bash

```
useradd -D -b /home2
```

- `-b`: 기본 홈 디렉터리 경로 변경 (`/home2`로 설정)
    

### **추가 설정**

1️⃣ **비밀번호 설정**

bash

```
passwd username
```

- `username` 사용자의 비밀번호 설정
    

2️⃣ **계정 잠금**

bash

```
usermod -L username
```

- 계정 잠금 (`-U` 옵션으로 잠금 해제 가능)
    

3️⃣ **계정 삭제**

bash

```
userdel username
```

- 사용자 계정 삭제 (`-r` 옵션을 추가하면 홈 디렉터리도 삭제됨)