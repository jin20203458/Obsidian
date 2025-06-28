### ✅ `getenforce` 명령어

- SELinux의 현재 모드를 확인하는 명령어입니다.
    
- 실행하면 **Enforcing**, **Permissive**, **Disabled** 중 하나를 출력합니다.
    

#### 🔹 사용 예시

bash

```
getenforce
```

출력 예시:

```
Enforcing
```

- **Enforcing**: SELinux 정책이 강제 적용됨.
    
- **Permissive**: SELinux 정책이 적용되지 않지만, 로그는 기록됨.
    
- **Disabled**: SELinux가 완전히 비활성화됨.
    

### ✅ `setenforce` 명령어

- SELinux의 모드를 **Enforcing ↔ Permissive**로 변경하는 명령어입니다.
    
- **일시적으로** SELinux 모드를 변경하며, 시스템을 재부팅하면 원래 설정으로 돌아갑니다.
    

#### 🔹 사용 예시

bash

```
setenforce 0  # Permissive 모드로 변경
setenforce 1  # Enforcing 모드로 변경
```

bash

```
getenforce
```

출력 예시:

```
Permissive
```

- `setenforce 0` → SELinux를 **Permissive** 모드로 변경 (정책 적용 X, 로그 기록 O)
    
- `setenforce 1` → SELinux를 **Enforcing** 모드로 변경 (정책 적용 O)
    

### ✅ SELinux 모드 영구 변경 방법

- `/etc/selinux/config` 파일을 수정하여 **영구적으로** SELinux 모드를 변경할 수 있습니다.
    

bash

```
vi /etc/selinux/config
```

설정 예시:

```
SELINUX=enforcing   # 강제 적용
SELINUX=permissive  # 허용하지만 정책 적용 X
SELINUX=disabled    # 완전히 비활성화
```

파일을 수정한 후 시스템을 재부팅하면 변경된 설정이 적용됩니다.