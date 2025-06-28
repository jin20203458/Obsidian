## ✅ `mdadm`이란?

**`mdadm`**은 리눅스에서 **소프트웨어 RAID (Redundant Array of Independent Disks)** 구성을 관리하기 위한 **명령어 기반 유틸리티**입니다.

> mdadm = **Multiple Device Administrator**

이 도구를 통해 RAID 디바이스를 **생성, 삭제, 조립(assemble), 검사, 복구, 모니터링**할 수 있습니다.

---

## ✅ 사용 목적

리눅스 커널의 **md (multiple device)** 드라이버를 활용해 **RAID 0, 1, 4, 5, 6, 10** 등을 **하드웨어 없이 소프트웨어로 구현**합니다.

---

## ✅ 주요 기능

|기능|설명|
|---|---|
|**RAID 생성**|RAID 0/1/5/6/10 등을 생성|
|**RAID 조립 (assemble)**|이미 구성된 RAID를 시스템 부팅 후 다시 조립|
|**RAID 확인 (examine)**|RAID 메타데이터 확인|
|**RAID 제거 (stop/remove)**|RAID 장치 정지 및 제거|
|**RAID 상태 확인**|`/proc/mdstat` 또는 `--detail`로 상태 확인|
|**장애 복구 및 재구성**|디스크 장애 시 복구 작업 가능|
|**모니터링**|RAID 상태 변화 시 이메일 알림 설정 가능|

---

## ✅ 기본 명령어 예시

### 1. RAID 0 생성

bash

`mdadm --create /dev/md0 --level=0 --raid-devices=2 /dev/sd[b-c]`

### 2. RAID 조립 (부팅 후)

bash

`mdadm --assemble /dev/md0 /dev/sdb /dev/sdc`

### 3. 상태 확인

bash

`mdadm --detail /dev/md0`

### 4. 메타데이터 보기

bash


`mdadm --examine /dev/sdb`

### 5. RAID 정지

bash


`mdadm --stop /dev/md0`

### 6. 구성 저장 (영구적 부팅)

bash


`mdadm --detail --scan >> /etc/mdadm/mdadm.conf`

### 7. 오류발생
`mdadm -f` 옵션은 **RAID 배열에서 특정 디스크를 강제로 실패(Fail) 상태로 설정**

---

## ✅ RAID 타입 요약

|타입|설명|
|---|---|
|RAID 0|스트라이핑. 속도↑, 장애 복원력 없음|
|RAID 1|미러링. 데이터 이중화|
|RAID 5|패리티 기반 분산 저장. 효율과 복원력의 균형|
|RAID 6|이중 패리티. 더 높은 복원력|
|RAID 10|RAID 1 + RAID 0 조합. 성능과 복원력 모두 확보|

---

## ✅ 관련 파일

- `/proc/mdstat`: 현재 RAID 상태 모니터링
    
- `/etc/mdadm/mdadm.conf`: RAID 설정 파일 (부팅 시 자동 조립을 위해 사용)