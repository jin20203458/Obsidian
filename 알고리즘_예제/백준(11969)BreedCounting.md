[11969번: Breed Counting (acmicpc.net)](https://www.acmicpc.net/problem/11969)
Farmer John's 𝑁\(N\) cows, conveniently numbered 1…𝑁\(1 \ldots N\), are all standing in a row (they seem to do so often that it now takes very little prompting from Farmer John to line them up). Each cow has a breed ID: 1 for Holsteins, 2 for Guernseys, and 3 for Jerseys. Farmer John would like your help counting the number of cows of each breed that lie within certain intervals of the ordering.

-------------------------------
3종류의 소가 섞여있는 줄에서 해당 범위안에 각 소의 수를 구하는
문제이다. 
간단한[[누적합(PrefixSum)]]문제인데 배열 1개로는 계산이 안되고,
3종류의소 누적합 배열을 이용하여 각각 계산하면된다.

```
#include<iostream>
#include<vector>
using namespace std;

int main() {
    ios_base::sync_with_stdio(false); cin.tie(nullptr); cout.tie(nullptr);
    int N, Q; cin >> N >> Q;   // N:소의 수(소의 품종은 1,2,3), Q:질문의 수
    
    vector<int>Holsteins(N + 1,0);  // 품종1
    vector<int>Guernseys(N + 1, 0); // 품종2
    vector<int>Jerseys(N + 1,0);    // 품종3

    int io1,io2;
    for (int i = 1; i <= N; ++i)
    {
        cin >> io1;
        if (io1 == 1) { Holsteins[i]++; }
        else if (io1 == 2) { Guernseys[i]++; }
        else { Jerseys[i]++; }

        Holsteins[i] += Holsteins[i - 1];
        Guernseys[i] += Guernseys[i - 1];
        Jerseys[i] += Jerseys[i - 1];
    }
    for (int i = 0; i < Q; ++i)
    {
        cin >> io1 >> io2; io1--;
        cout << Holsteins[io2] - Holsteins[io1] << ' ' 
            << Guernseys[io2] - Guernseys[io1] << ' ' 
            << Jerseys[io2] - Jerseys[io1] << '\n';
    }

    return 0;
}

```