

**PARTITION BY**
집계함수
- **설명**: 데이터 집합을 여러 파티션으로 나누어 각 파티션 내에서 윈도우 함수를 적용합니다. 파티션을 기준으로 그룹화되며, 각 그룹 내에서 별도로 계산됩니다.
```
RANK() OVER (PARTITION BY department_id ORDER BY salary DESC)
```

**ORDER BY**
순위함수
- **설명**: 파티션 내의 데이터 행을 정렬하는 기준을 설정합니다. `ORDER BY` 절이 없으면 기본 정렬 순서로 처리됩니다.
```
RANK() OVER (PARTITION BY department_id ORDER BY salary DESC)
```


**RANK()**:

- **설명**: 순위를 매기되, 동순위가 있을 경우 동일한 순위를 부여하고, 순위를 건너뛰는 방식입니다.
    
- **용도**: 동점인 경우 같은 순위를 부여하고, 그 다음 순위는 동점 수만큼 건너뛰는 방식으로 순위를 매길 때 사용합니다.
```
SELECT employee_id, 
       department_id, 
       salary, 
       RANK() OVER (PARTITION BY department_id ORDER BY salary DESC) AS rank
FROM employees;
```
ex) 1,1,3,4,4,6

**DENSE_RANK()**:

- **설명**: 순위를 매기되, 동순위가 있을 경우 동일한 순위를 부여하며 순위를 건너뛰지 않습니다.
    
- **용도**: 동점인 경우에도 순위를 연속적으로 매길 때 사용합니다.
```
SELECT employee_id, 
       department_id, 
       salary, 
       DENSE_RANK() OVER (PARTITION BY department_id ORDER BY salary DESC) AS rank
FROM employees;
```
ex)1,1,2,3,3,4

**ROW_NUMBER()**:

- **설명**: 각 행에 대해 고유한 순서 번호를 부여합니다. 순위가 중복되지 않습니다.
    
- **용도**: 데이터에 순서를 매기고 특정 행을 구분할 때 사용합니다.
```
SELECT employee_id, 
       department_id, 
       salary, 
       ROW_NUMBER() OVER (PARTITION BY department_id ORDER BY salary DESC) AS rank
FROM employees;
```
ex)1,2,3,4,5,6

- **`FIRST_VALUE`**: 파티션 내에서 첫 번째 값을 반환합니다.
- **`LAST_VALUE`**: 파티션 내에서 마지막 값을 반환합니다.
- **`LAG`**: 현재 행의 이전 행의 값을 반환합니다.
- **`LEAD`**: 현재 행의 다음 행의 값을 반환합니다.

- **UNBOUNDED PRECEDING:** 현재 행까지
- **UNBOUNDED FOLLOWING:** 현재 행부터 마지막 행까지
- **활용:** 누적 합계, 이동 평균, 전체 합계 등 다양한 계산에 사용


**다른 범위 지정 키워드:**

- **CURRENT ROW:** 현재 행만 포함
- **n PRECEDING:** 현재 행에서 n개 이전 행까지 포함
- **n FOLLOWING:** 현재 행에서 n개 이후 행까지 포함

**주의:**