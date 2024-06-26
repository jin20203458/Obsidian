
- 누적합 알고리즘은 배열의 각 요소까지의 누적된 합을 저장해두는 방식으로, 배열의 부분합을 효율적으로 계산하는 알고리즘입니다.

예를 들어, 주어진 배열이 [1, 2, 3, 4, 5]라고 가정하면

| 인덱스 | 배열 요소 | 누적합 |
| --- | ----- | --- |
| 0   | 1     | 1   |
| 1   | 2     | 3   |
| 2   | 3     | 6   |
| 3   | 4     | 10  |
| 4   | 5     | 15  |

이때 인덱스 2에서 4까지의 합을 구하고 싶다면 15에서 3을 빼주면 된다.
(인덱스 5까지의 누적합 - 인덱스 1까지의 누적합)

만약 2차원배열이라면?
![image](https://github.com/jin20203458/Obsidian/assets/127675852/709a1388-34df-4d0a-ba46-64788598b587)

구하고싶은 범위 - 범위밖에 범위를 해주면 된다. 다만 이때 중복으로 뺴기가 발생하는 부분은
처리해주어야 한다.
sum[i][j]=sum[i−1][j]+sum[i][j−1]−sum[i−1][j−1]+arr[i][j]
