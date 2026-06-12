
- 다이나믹 프로그래밍(DP)은 복잡한 문제를 작은 하위 문제로 나누어 해결하고, 이미 계산된 결과를 저장하여 다시 계산하지 않도록 함으로써 효율성을 높이는 알고리즘 설계 기법

- 하위 문제들의 해결 방법을 저장하고 재활용함으로써 중복 계산을 줄여 효율적 해결가능

크게 재귀를 활용한 **Top-Down (Memoization) 방식**, 
과 반복문을 활용한 **Bottom-Up (Tabulation) 방식** 으로 구현가능하다.

##### 예시) 피보나치 함수
**ex1) TopDown**
```
#include <iostream>
#include <vector>
using namespace std;

vector<int> memo;
int fibonacci_top_down(int n) {
    if (n <= 1)
        return n;
    if (memo[n] != -1)
        return memo[n];
    return memo[n] = fibonacci_top_down(n - 1) + fibonacci_top_down(n - 2);
}

int main() {
    int n;
    cin >> n;
   
    memo.assign(n + 1, -1);
    cout << "Fibonacci number at position " << n << " is: " << fibonacci_top_down(n) << endl;

    return 0;
}
```

**ex1) BottomUp**
```
#include <iostream>
#include <vector>

using namespace std;

int fibonacci_bottom_up(int n) { 
    if (n <= 1) return n;

    vector<int> dp(n + 1);
    dp[0] = 0; dp[1] = 1;

    for (int i = 2; i <= n; ++i)
        dp[i] = dp[i - 1] + dp[i - 2];

    return dp[n];
}

int main() {
    int n;
    cin >> n;

    cout << "Fibonacci number at position " << n << " is: " << fibonacci_bottom_up(n) << endl;

    return 0;
}
```