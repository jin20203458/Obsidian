### ✅ **디스크 쿼터 설정 순서**

1️⃣ **fstab 파일 수정**

- `/etc/fstab` 파일에서 해당 파일 시스템에 `usrquota,grpquota` 옵션 추가
    

bash

```
/dev/sda2  /home  ext4  defaults,usrquota,grpquota  0  2
```

2️⃣ **파일 시스템 다시 마운트**

- 변경된 설정을 적용하기 위해 `/home`을 다시 마운트
    

bash

```
mount -o remount /home
```

3️⃣ **쿼터 파일 생성 및 체크**

- `quotacheck` 명령어를 사용해 쿼터 기록 파일을 생성
    

bash

```
quotacheck -mf /home
```

4️⃣ **쿼터 활성화**

- `quotaon` 명령어로 쿼터 기능을 활성화
    

bash

```
quotaon /home
```

5️⃣ **사용 중인 쿼터 확인**

- `repquota` 명령어로 현재 적용된 쿼터 상태 확인
    

bash

```
repquota /home
```

이제 `/home` 디렉터리에 대한 사용자 및 그룹별 디스크 사용량 제한이 적용될 거야! 혹시 특정 사용자에 대한 쿼터 설정 방법도 궁금해? 😊✨ 자세한 내용은 여기에서 확인할 수 있어!