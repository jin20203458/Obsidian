`modprobe` 명령어는 **리눅스에서 커널 모듈을 로드하거나 제거하는 명령어**입니다. `insmod`와 `rmmod` 명령어와 유사하지만, **의존성이 있는 모듈을 자동으로 처리**할 수 있다는 점에서 더 강력합니다.

### ✅ `modprobe`의 주요 기능

- **커널 모듈 로드**: 특정 모듈을 커널에 추가 (`modprobe 모듈명`)
    
- **모듈 제거**: 커널에서 특정 모듈을 제거 (`modprobe -r 모듈명`)
    
- **의존성 자동 해결**: 모듈이 다른 모듈을 필요로 하면 자동으로 함께 로드
    
- **설정 파일 활용**: `/etc/modprobe.conf` 또는 `/etc/modprobe.d/`에서 설정을 읽어 적용
    

### ✅ 기본 사용법

bash

```
modprobe [옵션] 모듈명
```

### ✅ 주요 옵션

- `-r` : 모듈 제거 (`modprobe -r 모듈명`)
    
- `-v` : 실행 과정 출력 (`modprobe -v 모듈명`)
    
- `-c` : 현재 설정 확인 (`modprobe -c`)

- `-a` : 여러 개의 모듈을 한 번에 로드 (`modprobe -a 모듈1 모듈2`)
    
- `--showconfig` : 현재 설정 파일 내용 출력 (`modprobe --showconfig`)
    

### ✅ 예제

1️⃣ **USB 관련 모듈 로드**

bash

```
modprobe usb-storage
```

- USB 저장 장치 관련 모듈을 로드하여 USB 드라이브를 인식할 수 있도록 함
    

2️⃣ **네트워크 드라이버 로드**

bash

```
modprobe e1000
```

- `e1000` 네트워크 드라이버를 로드하여 인텔 이더넷 카드 활성화
    

3️⃣ **모듈 제거**

bash

```
modprobe -r usb-storage
```

- `usb-storage` 모듈을 커널에서 제거
    

4️⃣ **모듈 정보 확인**

bash

```
modinfo e1000
```

- `e1000` 모듈의 상세 정보(버전, 의존성, 설명 등)를 출력
    

### ✅ 추가 정보

- `modprobe`는 `/lib/modules/$(uname -r)/modules.dep` 파일을 참고하여 모듈 간 의존성을 자동으로 해결합니다.
    
- `depmod` 명령어를 사용하여 의존성 정보를 갱신할 수 있습니다.