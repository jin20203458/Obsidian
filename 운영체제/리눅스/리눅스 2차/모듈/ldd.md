`ldd` 명령어는 **리눅스에서 실행 파일이 사용하는 공유 라이브러리(Shared Libraries)** 를 출력해주는 도구입니다.

---

### 📌 `ldd`란?

**`ldd` = "List Dynamic Dependencies"**

즉, 어떤 실행 파일이 실행될 때 **어떤 공유 라이브러리를 참조하는지 보여주는 명령어**입니다.

사용 예시
```
ldd /bin/ls
```

출력 예시:
```
linux-vdso.so.1 =>  (0x00007ffe3b1d2000)
libselinux.so.1 => /lib64/libselinux.so.1 (0x00007f4b7dbb4000)
libc.so.6 => /lib64/libc.so.6 (0x00007f4b7d7f6000)
/lib64/ld-linux-x86-64.so.2 (0x00007f4b7de05000)
```

### 🔹 관련 명령어

|명령어|설명|
|---|---|
|`ldd 실행파일`|실행 파일이 사용하는 공유 라이브러리 확인|
|`echo "/opt/lib" >> /etc/ld.so.conf`|`/opt/lib` 경로를 라이브러리 검색 경로에 추가|
|`sudo ldconfig`|변경된 라이브러리 경로를 적용|
