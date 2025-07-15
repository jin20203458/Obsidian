- **일반 함수처럼 보이는 내장 함수들**을 정적으로 분석하여 그 의미를 모델링합니다.
    
- `__builtin_expect`, `__builtin_assume`, `__builtin_addressof` 등과 같이 특별한 의미를 가지지만 일반 함수처럼 호출되는 내장 함수들을 처리합니다.
    
- `std::move`, `std::addressof` 등 C++ 표준 라이브러리에서 제공하는 **builtin-like 함수**들도 함께 처리합니다.

