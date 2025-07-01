

Clang Static Analyzer가 경로를 분할하는 대표적인 사례를 직관적인 예제와 함께 설명합니다. 경로 분할은 주로 **조건문, 반복문, 함수 호출**에서 발생하며, 각 경로는 독립적인 상태를 가집니다.

## 1. 조건문(if)에 의한 분할

```c
void process_file(int mode) {
  FILE* fp = fopen("data.txt", "r");
  
  if (mode == 1) 
  {  // 분기점 1: 두 경로로 분할
    // 경로 A: mode == 1
    fclose(fp);
  }
   else 
  {
    // 경로 B: mode != 1
    // fp가 닫히지 않음
  }
  
  // 분기 종료 후: 두 경로 독립적 유지
  printf("Complete"); 
}
```

- **분석 과정**:
    
```text
graph LR 
A[시작] --> B[fopen] 
B --> C{mode == 1?}
C -->|Yes| D[fclose] --> E[printf]
C -->|No| F[printf]
```
    
- **결과**:
    
    - 경로 A: 정상 종료
    - 경로 B: **리소스 누수** 감지 (파일 미닫힘)

## 2. 반복문(while)에 의한 분할

```c
void read_lines() {
  FILE* fp = fopen("log.txt", "r");
  char buf[256];
  
  while (fgets(buf, sizeof(buf), fp)) 
  {  // 매 루프마다 새로운 경로
    if (buf[0] == 'E') 
    {  // 분기점 2
      // 경로 X: 에러 로그 처리
    } 
    else 
    {
      // 경로 Y: 일반 로그 처리
    }
  }
  
  fclose(fp);
}
```

- **분석 특징**:
    
    - 각 루프 실행은 독립적 경로
    - 100번 루프 → 100개의 독립 경로 생성
    - 조건문 내부에서 추가 분기 가능

## 3. 널 포인터 검사 분할

```c
void process_data(int* data) 
{
  if (data == NULL) 
  {  // 분기점 3
    // 경로 P: 널 포인터 경로
    log_error("Null input");
    return;
  }
  
  // 경로 Q: 유효 포인터 경로
  *data = process(*data);  // 널이 아님 보장
}
```
- **분석 동작**:
    
    - `data == NULL`인 경우와 아닌 경우 완전 분리
    - 경로 Q에서는 `data`가 널 아님을 보장
        

## 4. 함수 호출에 의한 분할

```c
void helper(int x) 
{
  int* mem = malloc(sizeof(int) * x);
  // 함수 종료 시: 메모리 해제 없음
}

void main() {
  helper(10);  // 경로 M
  helper(20);  // 경로 N (독립적)
}
```
- **분석 결과**:
    
    - `helper(10)` 호출 경로: **메모리 누수** 감지
    - `helper(20)` 호출 경로: 별도로 **메모리 누수** 감지
    - 두 경로 완전 분리 → 각각 별도 리포트


## 5. 심볼릭 값에 의한 분할

```c
void symbolic_example(int x) 
{  // x: 심볼릭 값
  FILE* fp = fopen("file.txt", "r");
  
  if (x > 0) 
  {  // 분기점 4
    fclose(fp);  // 경로 X
  } 
  else 
  {
    // 경로 Y: fp 닫지 않음
  }
  
  if (x < 100) 
  {  // 분기점 5
    // 경로 X,Y에서 다시 분할
  }
}
```

- **분석 복잡도**:
```text
graph TD
  A[시작] --> B[x>0?]
  B -->|Yes| C[fclose] --> D[x<100?]
  B -->|No| E[x<100?]
  D --> F[경로 X1]
  D --> G[경로 X2]
  E --> H[경로 Y1]
  E --> I[경로 Y2]
```
    
- **결과**: 4개의 독립 경로 생성


## 6. 오류 복구 경로 분할

```c
void error_handling() 
{
  FILE* fp = fopen("critical.dat", "rw");
  
  if (verify_data(fp) != SUCCESS) 
  {  // 분기점 6
    // 경로 E: 오류 처리 경로
    recovery_procedure(); 
    return;  // 여기서 경로 종료
  }
  
  // 경로 N: 정상 처리 경로
  process_data(fp);
  fclose(fp);
}
```

- **분석 특징**:
    
    - 오류 경로(E)와 정상 경로(N) 완전 분리
    - 경로 E에서는 `fp`가 닫히지 않음 → **리소스 누수 감지**
    - 경로 N: 정상 처리

---
## 경로 분할 원리 요약

1. **분기 발생 지점**:
    
    - 조건문(`if`, `switch`)
    - 반복문(`for`, `while`)
    - 함수 호출
    - 널 포인터/에러 체크
        
2. **분할 조건**:
    
```text
graph LR
  A[단일 경로] --> B{분기점 발생?}
  B -->|Yes| C[상태 복제]
  C --> D[경로 A]
  C --> E[경로 B]
  B -->|No| F[계속 단일 경로]
```
    
3. **분석기 동작 특성**:
    
    - 각 경로는 고유한 `ProgramState` 보유
    - 분기점 이후 경로는 완전 독립적
    - 경로는 중첩 가능(트리 구조)
    - 최대 경로 수는 분석기 설정에 따라 제한됨


> 이러한 경로 분할 메커니즘은 Clang의 `ExprEngine::processBranch()` 함수에서 구현되며,  
> 정적 분석의 핵심 강점인 **경로-민감 분석(path-sensitive analysis)**의 기반이 됩니다.