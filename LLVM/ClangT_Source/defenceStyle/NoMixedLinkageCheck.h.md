
전역영역의 Linkage가 없는 변수, 함수들은 기본적으로 external의 성질을 갖춘다.
```cpp
// 일반적인 전역 변수들
int global_var;              // → LinkKind::External
extern int explicit_var;     // → LinkKind::External  
static int file_local;       // → LinkKind::Internal
const int const_var = 10;    // → LinkKind::Internal
extern const int ext_const;  // → LinkKind::External
```
 clang-tidy *.cpp -checks="-*,defence-style-no-mixed-linkage" -header-filter=.* -- -std=c++17 -w

 clang-tidy .\*.cpp -checks="-*,defence-style-no-mixed-linkage" -- -std=c++17 -w
