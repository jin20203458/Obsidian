NFS(Network File System)는 네트워크를 통해 파일을 공유하는 시스템으로, 서버와 클라이언트 간에 원격 파일을 마치 로컬 파일처럼 사용할 수 있도록 해줘. 🚀 다양한 NFS 관련 명령어를 정리해볼게!

✅ **NFS 서버 관련 명령어**

|명령어|설명|
|---|---|
|`exportfs -v`|NFS 서버에서 공유된 디렉터리 목록과 설정 정보 출력|
|`exportfs -a`|`/etc/exports` 파일에 정의된 모든 공유를 활성화|
|`exportfs -u <경로>`|특정 공유를 비활성화|
|`systemctl restart nfs-server`|NFS 서버 재시작|
|`showmount -e <서버 IP>`|NFS 서버가 공유한 디렉터리 목록 확인|

✅ **NFS 클라이언트 관련 명령어**

|명령어|설명|
|---|---|
|`mount -t nfs <서버 IP>:<공유 경로> <마운트 위치>`|NFS 공유를 클라이언트에 마운트|
|`umount <마운트 위치>`|NFS 공유 해제|
|`df -h`|마운트된 NFS 파일 시스템 확인|
|`nfsstat -c`|NFS 클라이언트의 상태 정보 출력|

✅ **RPC 관련 명령어 (NFS와 연관됨)**

|명령어|설명|
|---|---|
|`rpcinfo -p <서버 IP>`|RPC 서비스 및 포트 정보 확인|
|`rpcinfo -u <서버 IP> nfs`|NFS 서비스가 실행 중인지 확인|
|`rpcinfo -t <서버 IP> mountd`|NFS 마운트 서비스 확인|

✔️ **요약**

- **NFS 서버**: `exportfs`, `showmount`, `systemctl restart nfs-server`
    
- **NFS 클라이언트**: `mount`, `umount`, `df`, `nfsstat`
    
- **RPC 관련**: `rpcinfo`