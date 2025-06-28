카데인 알고리즘 (Kadane's Algorithm)은 배열에서 연속된 부분 배열의 최대 합을 찾는 알고리즘입니다. 
이 알고리즘은 [[다이나믹프로그래밍(DynamicProgramming)]] 한 예로, 각 단계에서 현재 요소를 포함한 부분 배열의 최대 합을 계산하며 이를 전체 배열에 대해 반복적으로 수행하여 최종적으로 최대 합을 찾아냅니다.


**기본 원리**: 선형 시간 복잡도 O(N)로 매우 효율적이다.

- 각 요소를 순차적으로 검사하면서 최대값 를 업데이트하고, 이 값을 결과 값와 비교하여 결과값을 갱신한다.
- 만약 최대값이 음수가 되면 0으로 초기화하여 부분 배열의 재시작을 시도한다(음수 부분 배열은 최대 합을 낮추기 때문).


```
#include <iostream>
using namespace std;

int main() {
    ios_base::sync_with_stdio(false); cin.tie(nullptr); cout.tie(nullptr);
    int N,io,max=-1000,tmp; cin >> N;

    for (int i = 1; i <= N; ++i)
    {
        cin >> io;
      
        if (i == 1)tmp = io;
        else tmp += io;

        if (tmp > max)max = tmp;
        if (tmp < 0)tmp = 0;

    }
    cout << max;
    return 0;
}
```