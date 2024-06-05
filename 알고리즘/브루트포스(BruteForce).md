
- 문제를 해결하기 위해 가능한 **모든 경우**를 전부 시도하는 접근법
  말그대로 폭력적인 알고리즘...

- 가능한 모든 경우를 시도한다.
- 구현이 쉽고 직관적이다.
- 경우의 수가 많아지면 시간복잡도가 급격히 상승한다는 것이 단점.

예시 문제 :백준 약수구하기

```
#include <iostream>
#include<vector>
using namespace std;
int main() {

    int A, B;   cin >> A >> B;
    vector<int> v;
    for (int i = 1; i <= A; ++i)
        if(!(A%i))
            v.push_back(i);
    
    if (v.size() < B)
        cout << 0;
    else
        cout << v[B-1];
    return 0;
}

```