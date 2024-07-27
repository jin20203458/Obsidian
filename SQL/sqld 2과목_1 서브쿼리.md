
- 다른 쿼리 내에서 실행되는 쿼리
- 서브쿼리는 주 쿼리의 결과를 기반으로 결과를 반환하거나 조건을 평가하는 데 사용
- 일반적으로 `SELECT`, `INSERT`, `UPDATE`, `DELETE` 쿼리와 함께 사용

### 1. 스칼라 서브쿼리 (Scalar Subquery)

**정의:** 스칼라 서브쿼리는 단일 값(스칼라 값)을 반환하는 서브쿼리입니다. 반환되는 값이 단 하나의 값이므로 `SELECT`, `WHERE`, `HAVING` 절에서 유용하게 사용됩니다.

**주요 특징:**

- 반환값이 1행 1열로 제한됩니다.
- 예외적으로 여러 값이 반환될 경우 오류가 발생합니다.
```
SELECT EmployeeID, Name,
       (SELECT MAX(Salary) FROM Employees) AS HighestSalary
FROM Employees;
```

### 2. 인라인 뷰 (Inline View)

**정의:** 인라인 뷰는 `FROM` 절에서 서브쿼리를 사용하여 가상의 테이블을 생성합니다. 이 서브쿼리는 `SELECT` 쿼리와 함께 사용되며, 그 결과는 일시적으로 가상의 테이블로 취급됩니다.

**주요 특징:**

- 서브쿼리의 결과가 가상의 테이블로 사용됩니다.
- 주 쿼리에서 이 가상의 테이블을 참조하여 추가적인 필터링이나 조작을 수행할 수 있습니다.
```
SELECT DepartmentID, AvgSalary
FROM (SELECT DepartmentID, AVG(Salary) AS AvgSalary
      FROM Employees
      GROUP BY DepartmentID) AS DeptAvg
WHERE AvgSalary > 50000;

```

### 3. 중첩 서브쿼리 (Nested Subquery)

**정의:** 중첩 서브쿼리는 다른 서브쿼리 내부에 서브쿼리가 있는 형태입니다. 일반적으로 `WHERE`, `HAVING` 절에 사용됩니다.

**주요 특징:**

- 서브쿼리 내에 또 다른 서브쿼리를 포함할 수 있습니다.
- 여러 레벨로 중첩될 수 있습니다.
```
SELECT Name
FROM Employees
WHERE DepartmentID IN (SELECT DepartmentID
                       FROM Departments
                       WHERE LocationID = (SELECT LocationID
                                            FROM Locations
                                            WHERE City = 'New York'));
```

### 서브쿼리의 주요 용도:

1. **조건 필터링 (Filtering Conditions):** 서브쿼리를 사용하여 특정 조건을 만족하는 데이터를 필터링할 수 있습니다.
    
    - 예: `SELECT * FROM Products WHERE CategoryID = (SELECT CategoryID FROM Categories WHERE CategoryName = 'Electronics');`
    
1. **집계 계산 (Aggregate Calculation):** 서브쿼리를 사용하여 집계 함수를 계산하고 이를 기반으로 결과를 필터링할 수 있습니다.
    
    - 예: `SELECT * FROM Orders WHERE TotalAmount > (SELECT AVG(TotalAmount) FROM Orders);`
    
1. **데이터 집합 비교 (Data Set Comparison):** 서브쿼리를 사용하여 두 데이터 집합 간의 비교를 수행할 수 있습니다.
    
    - 예: `SELECT * FROM Students WHERE StudentID IN (SELECT StudentID FROM Enrollments WHERE CourseID = 'CS101');`