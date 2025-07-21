
### 메모리 Leak 탐지
```cpp
void f() {
  int *p = (int*)malloc(sizeof(int));
  // free(p);  // 주석시 Leak
}
```

### Double Free 탐지
```cpp
void f() {
  int *p = (int*)malloc(sizeof(int));
  free(p);
  free(p); // Double Free
}
```

### Use After Free 탐지
```cpp
void f() {
  int *p = (int*)malloc(sizeof(int));
  free(p);
  *p = 1; // Use After Free
}
```

### Ownership Propagation(ownership_transfer)
```cpp
int *wrap_malloc() { return (int*)malloc(sizeof(int)); }
void f() {
  int *p = wrap_malloc();
  free(p);
}
```

### 사용자 정의 할당 함수 대응 (Optimistic Mode)
```cpp
__attribute__((ownership_returns(malloc, 1))) // 함수가 malloc처럼 동작
void *my_alloc(size_t n);

void f() {
  void *p = my_alloc(16);
  // free(p); // 해제 안 하면 leak 경고
}
```


class RefState
 메모리 관리 관점에서의 심볼(symbol) 상태.

