**UndefinedAssignmentChecker**는  
코드에서 **정의되지 않은 값(초기화되지 않은 변수 등)**이  
**다른 변수/메모리/필드 등에 대입되는 경우**를  
실시간으로 탐지해 **경고**하는 정적 분석기용 체크커입니다.


예시
```cpp
void foo() {
    int a;
    int b = a; // ★ 경고: "Assigned value is undefined"
}
```
