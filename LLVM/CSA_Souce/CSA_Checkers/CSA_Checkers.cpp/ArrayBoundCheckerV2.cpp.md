
참조:
[[ArrayBoundChecker.cpp]]

이 체커는 아래 3가지 타입의 AST 노드가 **실행된 뒤**(PostStmt)마다 분석을 수행

- **ArraySubscriptExpr**: 배열 인덱스(예: arr[x])
- **UnaryOperator**: 포인터 deref(예: *p), (단, UO_Deref만 분석)
- **MemberExpr**: 구조체/클래스 포인터 멤버 접근(예: p->field)

```cpp

```