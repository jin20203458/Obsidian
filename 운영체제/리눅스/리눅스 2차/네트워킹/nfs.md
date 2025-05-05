✅ **NFS(Network File System) 개요** NFS는 네트워크를 통해 파일 시스템을 공유하는 프로토콜로, 서버가 디렉토리를 공유하면 클라이언트는 이를 **자신의 로컬 디렉토리처럼 마운트하여 사용**할 수 있습니다.

- **리눅스 및 유닉스 계열에서 널리 사용**
    
- **기본 포트:** `2049`
    
- **동작 방식:** 서버는 `export`, 클라이언트는 `mount`
    

### **🧱 핵심 파일**

|경로|설명|
|---|---|
|`/etc/exports`|NFS 서버가 클라이언트에게 공유할 디렉토리와 권한을 설정하는 파일|
|`/var/lib/nfs/`|상태 파일 저장소 등 NFS 관련 메타데이터 보관|

### **📄 /etc/exports 예시**

bash

```
# 기본 공유 설정
/shared 192.168.5.0/24(rw,no_root_squash,sync)
```

🔹 **옵션 설명:**

- `rw` → 읽기/쓰기 허용
    
- `no_root_squash` → 클라이언트 **root**가 서버에서도 **root 권한 유지** (보안상 위험)
    
- `sync` → 모든 데이터를 디스크에 동기화 후 응답 (**데이터 무결성↑**)
    

✅ **네트워크 표기법:**

- `192.168.5.0/24` 또는 `192.168.5.0/255.255.255.0` 같은 형식 사용 가능
    

### **🔐 접근 제어 옵션 설명**

|옵션|의미|
|---|---|
|`rw` / `ro`|읽기/쓰기 허용 또는 읽기 전용|
|`no_root_squash`|클라이언트의 **root 권한 유지** (보안상 위험)|
|`all_squash`|모든 클라이언트 사용자를 서버의 `nobody`로 맵핑 (보안 강화)|
|`anonuid, anongid`|`nobody` 사용자 대신 특정 **UID/GID**로 매핑할 때 사용|

### **🛠️ 서버 설정 예제**

bash

```
# /etc/exports에 공유 디렉토리 추가
echo "/shared 192.168.5.0/24(rw,no_root_squash)" >> /etc/exports

# 서비스 시작
systemctl start nfs-server
systemctl enable nfs-server

# export 설정 적용
exportfs -a
```

### **💻 클라이언트 마운트 예제**

bash

```
# 서버의 공유 디렉토리를 마운트
mount -t nfs 192.168.5.10:/shared /mnt/shared

# 자동 마운트를 위해 /etc/fstab에 추가 가능
192.168.5.10:/shared  /mnt/shared  nfs  defaults  0  0
```

### **📌 보안 권장**

❗ `no_root_squash` 사용 시 주의 → 클라이언트의 root 계정이 서버에서 root 권한을 갖게 되어 **보안에 취약** ✅ **추천 설정:** `all_squash`를 기본으로 사용하고, 익명 계정 권한을 **최소화**하는 것이 바람직함