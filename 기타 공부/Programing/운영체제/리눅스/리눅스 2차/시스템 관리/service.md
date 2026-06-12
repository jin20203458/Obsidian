`service` 명령어는 **Linux에서 시스템 서비스(데몬)**를 **시작, 중지, 재시작, 상태 확인** 등 관리하는 데 사용하는 명령어입니다.

`service <서비스이름> <명령>`

```
service sshd start      # ssh 서비스 시작
service httpd restart   # Apache 웹서버 재시작
service ypserv status   # NIS 데몬 상태 확인
```
