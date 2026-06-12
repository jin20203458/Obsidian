
- 문제를 해결하는 데 있어서 **현재 상황에서 가장 좋다고** 생각되는 선택을 하는 방법입니다. 
  이러한 선택이 전체적으로도 최적해를 보장하는 경우에 그리디 알고리즘을 사용합니다.


#### 그리디 알고리즘의 기본 원리

1. **선택 기준**: 매 단계에서 가장 좋다고 생각되는 선택을 합니다.
2. **지역 최적 선택**: 현재 상황에서의 최적 선택이 전체 문제의 최적 해결책으로 이어진다고 가정합니다.
3. **결과의 누적**: 각 단계에서의 선택을 누적하여 최종 해결책을 도출합니다.

#### 그리디 알고리즘의 특징

- **단순성**: 구현이 비교적 간단하고 이해하기 쉽습니다.
- **효율성**: 보통의 경우 시간 복잡도가 낮으며, 빠르게 해를 도출할 수 있습니다.
- **최적해 보장 조건**: 모든 경우에 최적해를 보장하는 것은 아니며, 최적 부분 구조와 탐욕적 선택 속성이 있어야 합니다.

예시 ) 백준1047 동전0
```
#include<iostream>
using namespace std;

int main()
{
    ios_base::sync_with_stdio(false); cin.tie(nullptr); cout.tie(nullptr);
    int N, k, Ccount = 0; cin >> N >> k;      //가진 동전의 개수, 만들어야하는 합
    int coin_arr[10];

    for (int i = 0; i < N; ++i)
        cin >> coin_arr[i];                  
    int Ocount = 0;
    while (k != 0)
    {
        for (int i = N - 1; i >=0; --i)
        {
            if (k / coin_arr[i] > 0)
            {
                int tmp; tmp = k;
                k = k - coin_arr[i] * (k / coin_arr[i]);
                Ccount += tmp / coin_arr[i];
                N = i ;
                break;
            }
        }
        
        ++Ocount;
    }
    cout  << Ccount<<endl;

    return 0;
}
```