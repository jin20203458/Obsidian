
Buffer.BlockCopy()
```
Buffer.BlockCopy(
    Array src,  // 원본 배열
    int srcOffset,  // 원본 배열에서 복사 시작 위치 (바이트 단위)
    Array dst,  // 대상 배열
    int dstOffset,  // 대상 배열에서 복사 시작 위치 (바이트 단위)
    int count  // 복사할 바이트 크기
);
```
- `src`: 원본 배열 (바이트 배열 `byte[]` 또는 정수 배열 등)
- `srcOffset`: **원본 배열에서 몇 번째 바이트부터 복사할지** (바이트 단위)
- `dst`: 복사 대상 배열
- `dstOffset`: **대상 배열에서 몇 번째 바이트부터 복사할지** (바이트 단위)
- `count`: 복사할 **총 바이트 크기**