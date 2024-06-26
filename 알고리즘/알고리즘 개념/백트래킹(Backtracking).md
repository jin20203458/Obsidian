
- 재귀적으로 문제를 해결하는 기법중 하나로 해를 찾기 위해 모든 가능한 후보를 탐색하는 과정에서 잘못된 후보를 만나면 되돌아가서 다른 후보를 탐색하는 알고리즘 설계기법

- 해가 될 가능성이 있으면 지속적 탐색, 가능성이 없다면 가지치기 하여 빠르게 전체 해를 탐색
- 해가  되지 않는 경우의 수는 배재하여 해를 찾는 시간을 단축

예시  문제 : 백준 N과M

```
#include <iostream>
#include<vector>
using namespace std;

void backtracking(vector<int>&V, vector<bool>&visit,vector<int>&path, int M)
{
	if (path.size() == M)
	{
		for (int num : path)
			cout << num << ' ';

		cout << '\n'; return;
	}
	for (int i = 0; i < V.size(); ++i)
	{
		if (visit[i] == true)
			continue;

		visit[i] = true;
		path.emplace_back(V[i]);

		backtracking(V, visit, path, M);

		path.pop_back();
		visit[i] = false;
	}
}

int main()
{
	int N, M; cin >> N >> M;
	vector<int>v(N);
	vector<bool>visit(N,false);
	vector<int>path;
	for (int i = 0; i < N; ++i)
		v[i] = i+1;
	backtracking(v, visit,path, M);
}
```