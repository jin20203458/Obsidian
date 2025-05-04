**id=8e**

### ✅ LVM 생성 과정

1️⃣ **pvcreate**: 물리 볼륨(Physical Volume, PV) 생성

bash

```
pvcreate /dev/sdb1
```

- `/dev/sdb1`을 LVM에서 사용할 수 있도록 물리 볼륨으로 초기화합니다.
    

2️⃣ **vgcreate**: 볼륨 그룹(Volume Group, VG) 생성

bash

```
vgcreate myVG /dev/sdb1
```

- `myVG`라는 볼륨 그룹을 생성하고, `/dev/sdb1`을 포함시킵니다.
    

3️⃣ **lvcreate**: 논리 볼륨(Logical Volume, LV) 생성

bash

```
lvcreate -L 10G -n myLV myVG
```

- `myVG`에서 `myLV`라는 이름의 논리 볼륨을 10GB 크기로 생성합니다.
    

### ✅ 이후 과정

4️⃣ **파일 시스템 생성**

bash

```
mkfs.ext4 /dev/myVG/myLV
```

- 논리 볼륨을 파일 시스템으로 포맷합니다.
    

5️⃣ **마운트 및 사용**

bash

```
mount /dev/myVG/myLV /mnt
```

- `/mnt` 디렉터리에 마운트하여 사용할 수 있도록 설정합니다.