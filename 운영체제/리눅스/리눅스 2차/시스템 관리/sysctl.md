# sysctl 명령어 설명

`sysctl` 명령어는 리눅스 커널 파라미터를 실시간으로 조회하고 변경할 수 있는 강력한 시스템 관리 도구입니다. 아래 도표를 통해 `sysctl` 명령어의 주요 기능과 옵션을 설명해 드리겠습니다.

## sysctl 명령어 주요 옵션 도표

| 옵션                       | 설명                                      | 사용 예시                                    |
| ------------------------ | --------------------------------------- | ---------------------------------------- |
| `-a`, `--all`            | 모든 커널 파라미터 값을 출력                        | `sysctl -a`                              |
| `-w`, `--write`          | 커널 파라미터 값을 설정                           | `sysctl -w net.ipv4.ip_forward=1`        |
| `-p [파일]`, `--load[=파일]` | 지정된 파일에서 설정을 로드 (기본값: /etc/sysctl.conf) | `sysctl -p /etc/sysctl.d/99-custom.conf` |
| `-n`, `--values`         | 파라미터 이름 없이 값만 표시                        | `sysctl -n kernel.hostname`              |
| `-e`, `--ignore`         | 알 수 없는 키 오류 무시                          | `sysctl -e -p`                           |
| `-q`, `--quiet`          | 설정 값 변경 시 출력 없음                         | `sysctl -q -w net.ipv4.ip_forward=1`     |

## 주요 커널 파라미터 카테고리

| 카테고리     | 설명              | 예시 파라미터                                                |
| -------- | --------------- | ------------------------------------------------------ |
| net.ipv4 | IPv4 네트워크 관련 설정 | `net.ipv4.ip_forward`, `net.ipv4.icmp_echo_ignore_all` |
| net.ipv6 | IPv6 네트워크 관련 설정 | `net.ipv6.conf.all.disable_ipv6`                       |
| kernel   | 커널 코어 설정        | `kernel.hostname`, `kernel.shmmax`                     |
| vm       | 가상 메모리 관련 설정    | `vm.swappiness`, `vm.overcommit_memory`                |
| fs       | 파일 시스템 관련 설정    | `fs.file-max`, `fs.inotify.max_user_watches`           |
| dev      | 장치 관련 설정        | `dev.cdrom.autoclose`                                  |

## 설정 파일 위치

| 파일/디렉토리            | 설명                         |
| ------------------ | -------------------------- |
| `/etc/sysctl.conf` | 기본 sysctl 설정 파일            |
| `/etc/sysctl.d/`   | 추가 sysctl 설정 파일 디렉토리       |
| `/proc/sys/`       | 커널 파라미터가 파일 시스템으로 노출된 디렉토리 |

## 사용 예시
```
# ping 응답 비활성화
sysctl -w net.ipv4.icmp_echo_ignore_all=1

# IP 포워딩 활성화
sysctl -w net.ipv4.ip_forward=1

# 설정 파일에서 값 로드
sysctl -p /etc/sysctl.conf

# 특정 매개변수 값 확인
sysctl kernel.hostname
```
`sysctl`은 시스템 보안 강화, 네트워크 설정 조정, 시스템 성능 최적화 등 다양한 목적으로 사용됩니다. 변경된 설정을 영구적으로 유지하려면 `/etc/sysctl.conf` 파일이나 `/etc/sysctl.d/` 디렉토리 내 파일에 설정을 저장해야 합니다.




