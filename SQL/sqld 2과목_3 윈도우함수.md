윈도우 함수는 SQL 쿼리에서 각 행에 대해 집계 및 분석을 수행할 수 있는 강력한 도구입니다. 윈도우 함수는 `OVER` 절을 사용하여 정의되며, 매개변수를 통해 어떻게 결과를 계산할지 지정합니다. 주요 매개변수는 `PARTITION BY`, `ORDER BY`, 및 `ROWS`/`RANGE`/`GROUPS` 절입니다.

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

