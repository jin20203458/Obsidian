**id=8e**


LVM(Logical Volume Manager)에서 사용하는 `pvcreate`, `vgcreate`, `lvcreate` 명령어는 각각 **물리 볼륨(PV), 볼륨 그룹(VG), 논리 볼륨(LV)**을 생성하는 역할을 합니다.

### ✅ `pvcreate` (Physical Volume 생성)

- **역할**: 디스크의 특정 파티션을 LVM에서 사용할 수 있도록 **물리 볼륨(PV)**으로 초기화합니다.
    
- **사용법**:
    
    bash
    
    ```
    pvcreate /dev/sdb1
    ```
    
    - `/dev/sdb1`을 LVM에서 사용할 수 있도록 설정합니다.
        

### ✅ `vgcreate` (Volume Group 생성)

- **역할**: 여러 개의 물리 볼륨(PV)을 하나의 **볼륨 그룹(VG)**으로 묶어 관리합니다.
    
- **사용법**:
    
    bash
    
    ```
    vgcreate myVG /dev/sdb1 /dev/sdc1
    ```
    
    - `myVG`라는 볼륨 그룹을 생성하고, `/dev/sdb1`과 `/dev/sdc1`을 포함시킵니다.
        

### ✅ `lvcreate` (Logical Volume 생성)

- **역할**: 볼륨 그룹(VG)에서 특정 크기의 **논리 볼륨(LV)**을 생성합니다.
    
- **사용법**:
    
    bash


    ```
    lvcreate -L 10G -n myLV myVG
    // lvcreate 옵션 위치인자
    ```
    
    - `myVG`에서 `myLV`라는 이름의 논리 볼륨을 10GB 크기로 생성합니다.
        

### ✅ 전체 과정 요약

1️⃣ **물리 볼륨 생성** → `pvcreate /dev/sdb1` 2️⃣ **볼륨 그룹 생성** → `vgcreate myVG /dev/sdb1` 3️⃣ **논리 볼륨 생성** → `lvcreate -L 10G -n myLV myVG` 4️⃣ **파일 시스템 생성** → `mkfs.ext4 /dev/myVG/myLV` 5️⃣ **마운트 및 사용** → `mount /dev/myVG/myLV /mnt`



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