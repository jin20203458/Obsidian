- 주요타입들 계층구조 예시

```
clang::Type
├── BuiltinType
│   (int, char, void, float, bool 등 기본 내장 타입)
├── PointerType
│   (int*, char*, 함수 포인터 등 모든 포인터 타입)
├── ReferenceType
│   ├── LValueReferenceType   (int&, const char& 등 lvalue 참조)
│   └── RValueReferenceType   (int&& 등 rvalue 참조, C++11)
├── ArrayType
│   ├── ConstantArrayType     (고정 크기 배열: int arr[10])
│   ├── IncompleteArrayType   (크기 미지정 배열: extern int arr[])
│   └── VariableArrayType     (가변 길이 배열: int arr[n])
├── FunctionType
│   ├── FunctionProtoType     (매개변수 정보가 있는 함수: int func(int, float))
│   └── FunctionNoProtoType   (매개변수 정보 없는 함수: void func();)
├── RecordType
│   (struct, class, union 등 사용자 정의 타입)
├── EnumType
│   (enum 타입)
├── TypedefType
│   (typedef로 정의된 타입 별칭)
└── AutoType
    (auto로 추론된 타입, C++11)

```

QualType
- qualifier Type:한정자 타입
- Clang의 타입 시스템에서 "타입 정보(type)와 수식어(qualifier, 예: const/volatile/restrict 등)를 함께 표현하는 래퍼 클래스
