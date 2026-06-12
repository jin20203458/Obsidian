SQL에서 집계와 그룹화를 더욱 정교하게 처리하기 위해 제공되는 기능들 중에서 `ROLLUP`, `CUBE`, `GROUPING SETS`, 그리고 `GROUPING` 함수가 있습니다. 이들 기능은 데이터 집계 및 요약을 다양한 방식으로 수행할 수 있게 도와줍니다.

### 1. ROLLUP

**정의:** `ROLLUP`은 계층적 그룹화를 수행하여 데이터를 집계합니다. 주어진 열들에 대해 모든 하위 그룹의 집계 결과와 함께 상위 그룹의 집계 결과도 반환합니다. 일반적으로 계층 구조를 가진 데이터에서 유용합니다.

**특징:**

- 지정된 열들에 대해 계층적으로 집계 결과를 생성합니다.
- 마지막 열에서부터 시작하여, 모든 상위 열들로 집계합니다.
```
SELECT Department, Team, SUM(Sales)
FROM SalesData
GROUP BY ROLLUP (Department, Team);
```
- 이 쿼리는 `Department`와 `Team`별로 매출을 집계하고, 각 `Department`별 집계와 전체 집계도 포함합니다.

| Department | Team | SUM(Sales) |     |
| ---------- | ---- | ---------- | --- |
| HR         | East | 1000       | D+T |
| HR         | West | 1200       | D+T |
| HR         | NULL | 2200       | D   |
| Sales      | East | 1500       | D+T |
| Sales      | West | 1700       | D+T |
| Sales      | NULL | 3200       | D   |
| NULL       | NULL | 5400       | 총계  |

### 2. CUBE

**정의:** `CUBE`는 다차원 집계를 생성하여 모든 가능한 조합의 그룹화 결과를 반환합니다. 각 열의 모든 조합에 대해 집계 결과를 제공합니다.

**특징:**

- 모든 가능한 그룹화 조합을 포함한 집계 결과를 생성합니다.
- 다양한 차원의 데이터 집계를 한 번에 수행할 수 있습니다.

```
SELECT Department, Team, SUM(Sales)
FROM SalesData
GROUP BY CUBE (Department, Team);
```
- 이 쿼리는 `Department`와 `Team`의 모든 조합에 대한 매출을 집계합니다.

| Department | Team | SUM(Sales) |     |
| ---------- | ---- | ---------- | --- |
| HR         | East | 1000       | D+T |
| HR         | West | 1200       | D+T |
| HR         | NULL | 2200       | D   |
| Sales      | East | 1500       | D+T |
| Sales      | West | 1700       | D+T |
| Sales      | NULL | 3200       | D   |
| NULL       | East | 2500       | T   |
| NULL       | West | 2900       | T   |
| NULL       | NULL | 5400       | 총계  |

### 3. GROUPING SETS

**정의:** `GROUPING SETS`는 사용자가 지정한 그룹화 집합에 대해 집계를 수행합니다. 다양한 집계 수준을 명시적으로 정의할 수 있습니다.

**특징:**

- 사용자가 직접 그룹화 집합을 정의하여 필요에 따라 집계를 수행할 수 있습니다.
- `ROLLUP`이나 `CUBE`보다 더 세밀한 제어가 가능합니다.
```
SELECT Department, Team, SUM(Sales)
FROM SalesData
GROUP BY GROUPING SETS (
    (Department, Team),
    (Department),
    ()
);
```
- 이 쿼리는 `Department`와 `Team`, `Department`만, 그리고 전체 집계(빈 집합)에 대해 매출을 집계합니다.
- 밑 표는  ROLLUP과 같다.

| Department | Team | SUM(Sales) |
| ---------- | ---- | ---------- |
| HR         | East | 1000       |
| HR         | West | 1200       |
| HR         | NULL | 2200       |
| Sales      | East | 1500       |
| Sales      | West | 1700       |
| Sales      | NULL | 3200       |
| NULL       | NULL | 5400       |

### 4. GROUPING

**정의:** `GROUPING` 함수는 `ROLLUP`이나 `CUBE`를 사용한 결과에서 각 열이 집계 수준에서 그룹화되었는지 여부를 식별하는 데 사용됩니다. 이 함수는 `GROUPING SETS`, `ROLLUP`, `CUBE`와 함께 사용할 수 있습니다.

**특징:**

- 집계 결과에서 특정 열이 집계 수준에서 그룹화된 여부를 확인합니다.
- `GROUPING` 함수는 해당 열이 집계된 결과에서 NULL인지 여부를 판별할 수 있습니다.
```
SELECT Department,
       Team,
       SUM(Sales),
       GROUPING(Department) AS DeptGrouping,
       GROUPING(Team) AS TeamGrouping
FROM SalesData
GROUP BY ROLLUP (Department, Team);
```
- 이 쿼리는 `Department`와 `Team`별로 집계된 매출과 함께, 각 열의 집계 여부를 나타내는 플래그를 제공합니다.

| Department | Team  | SUM(Sales) | DeptGrouping | TeamGrouping |
|------------|-------|------------|--------------|--------------|
| HR         | East  | 1000       | 0            | 0            |
| HR         | West  | 1200       | 0            | 0            |
| HR         | NULL  | 2200       | 0            | 1            |
| Sales      | East  | 1500       | 0            | 0            |
| Sales      | West  | 1700       | 0            | 0            |
| Sales      | NULL  | 3200       | 0            | 1            |
| NULL       | NULL  | 5400       | 1            | 1            |
각기 다른 차수의 값들이 합쳐지거나 요약된 결과를 보고, 특정 열이 집계 단계에서 어떻게 그룹화되었는지 확인할 수 있다.
