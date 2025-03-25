### **CreateIoCompletionPort**
`CreateIoCompletionPort`는 새로운 I/O 완료 포트를 생성하는 함수입니다. 이 포트는 비동기 I/O 작업의 완료 상태를 추적하는 데 사용됩니다.

```
HANDLE CreateIoCompletionPort(
  HANDLE FileHandle,              // 파일 핸들, 소켓 핸들 등
  HANDLE ExistingCompletionPort,  // 기존 I/O 완료 포트
  ULONG_PTR CompletionKey,        // 완료 키
  DWORD NumberOfConcurrentThreads // 처리할 스레드 수
);
```

생성 ex)
```
 g_hIOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
```
연결 ex)
```
(CreateIoCompletionPort((HANDLE)serverSocket, g_hIOCP, 0, 0) 
```
#### 반환 값

- _ExistingCompletionPort_ 매개 변수가 **NULL**인 경우 반환 값은 새 핸들입니다.
- _ExistingCompletionPort_ 매개 변수가 유효한 I/O 완료 포트 핸들인 경우 반환 값은 동일한 핸들입니다.
- _FileHandle_ 매개 변수가 유효한 핸들인 경우 해당 파일 핸들은 반환된 I/O 완료 포트와 연결됩니다.
함수가 실패하면 반환 값은 **NULL**입니다. 확장 오류 정보를 가져오려면 [**GetLastError**](https://learn.microsoft.com/ko-kr/windows/desktop/api/errhandlingapi/nf-errhandlingapi-getlasterror) 함수를 호출합니다.

---
### **GetQueuedCompletionStatus**

`GetQueuedCompletionStatus`는 I/O 완료 포트에서 완료된 작업을 기다리고, 완료된 작업을 가져오는 함수입니다. 이 함수는 비동기 작업의 완료를 처리하는 주요 함수입니다.
```
BOOL GetQueuedCompletionStatus(
  HANDLE  CompletionPort,     // I/O 완료 포트 핸들
  LPDWORD lpNumberOfBytes,    // 전송된 바이트 수
  PULONG_PTR lpCompletionKey, // 완료 키
  LPOVERLAPPED *lpOverlapped, // OVERLAPPED 구조체 포인터
  DWORD dwMilliseconds        // 대기 시간 (INFINITE로 무기한 대기)
);
```
- `completionKey`는 어떤 클라이언트(또는 소켓)에 대한 작업이 완료되었는지 알려줍니다.
- `lpOverlapped`는 어떤 비동기 작업이 완료되었는지, 그 작업에 대한 상세한 상태를 알 수 있게 해줍니다.

---
### **PostQueuedCompletionStatus**

`PostQueuedCompletionStatus`는 I/O 작업이 완료된 후 이를 I/O 완료 포트에 전달하는 함수입니다. 이 함수는 보통 애플리케이션이 I/O 작업을 수동으로 처리할 때 사용됩니다.
```
BOOL PostQueuedCompletionStatus(
  HANDLE  CompletionPort,    // I/O 완료 포트 핸들
  DWORD   dwNumberOfBytes,   // 전송된 바이트 수
  ULONG_PTR dwCompletionKey, // 완료 키
  LPOVERLAPPED lpOverlapped // 완료된 작업에 대한 OVERLAPPED 구조체
);
```

---
### **CloseHandle**

`CloseHandle`은 I/O 완료 포트를 닫는 데 사용됩니다. I/O 작업이 완료된 후 더 이상 해당 포트를 사용할 필요가 없을 때 호출합니다.

### **GetLastError**

`GetLastError`는 최근 함수 호출에서 발생한 오류를 반환하는 함수입니다. IOCP 관련 작업이 실패했을 때 오류 코드를 확인하는 데 사용됩니다.