### 🧱 구조체(struct)란?

- **서로 다른 자료형**을 하나의 집합으로 묶을 수 있는 사용자 정의 자료형
    
- 예를 들어 학생 1명의 정보를 저장하려면 `이름`, `학번`, `학과` 같은 다양한 자료형이 필요하겠지? → 이럴 때 구조체 사용!
    

### 📋 구조체 선언 예시

c

```
struct Student {
    int id;
    char name[20];
    char major[20];
};
```

- `struct` 키워드로 선언하고, 내부에 다양한 **속성(멤버)**을 정의함
    
- 위 예제는 학생을 표현하는 자료형(Student) 하나 만든 거야
    

### 🛠️ 구조체 변수 선언과 사용

c

```
struct Student s1;

strcpy(s1.name, "홍길동");
s1.id = 2023001;
strcpy(s1.major, "컴퓨터공학");
```

- `.` 연산자를 사용해 멤버에 접근
    
- `strcpy()`는 문자열 복사 함수 (문자열은 `=`으로 못 넣음!)
    

### 🧠 typedef로 구조체 간소화

c

```
typedef struct {
    int id;
    char name[20];
} Person;

Person p1;
```

- `typedef`를 사용하면 `struct`를 계속 쓰지 않아도 돼서 간편함
    

### 🔗 구조체 배열도 가능해!

c

```
struct Student class[30];
```

→ 학생 30명의 정보를 구조체 배열로 관리 가능!

### 📦 구조체 안에 구조체도 가능!

c

```
struct Date {
    int year, month, day;
};

struct Employee {
    int empno;
    struct Date hireDate;  // 구조체 안에 구조체 포함
};
```

### 🎯 시험 포인트 요약

- **구조체는 여러 자료형을 하나로 묶는 도구**
    
- `.`은 일반 구조체 멤버 접근
    
- `->`는 **포인터로 접근할 때** 사용
    
- `typedef struct {...} 타입명;` 형태는 자주 출제됨
    
- 구조체 배열, 구조체 안의 구조체도 가능함