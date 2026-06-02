
```sql
[fopen()]  --> 상태 등록: Opened
    |
[fclose()] --> 상태 변경: Closed
    |            |
  Already      정상 처리
   Closed --> Double fclose 버그 리포트

[Symbol 죽음 감지]
    |
 상태: Opened --> Resource Leak 버그 리포트

[Pointer Escape 발생]
    |
  추적 중단 (심볼 제거)
```


```

```