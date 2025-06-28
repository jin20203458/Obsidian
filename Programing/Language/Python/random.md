`import random`은 파이썬에서 난수(random number)를 생성하기 위한 `random` 모듈을 불러오는 명령입니다. 이 모듈을 사용하면 다양한 난수 생성 기능을 이용할 수 있습니다.

✅ **기본 사용법**

python

```
import random  # random 모듈 불러오기

print(random.randint(1, 10))  # 1 이상 10 이하의 랜덤 정수
print(random.random())        # 0.0 이상 1.0 미만의 랜덤 실수
print(random.choice(["사과", "바나나", "포도"]))  # 리스트에서 랜덤 선택
```

- `randint(a, b)`: `a` 이상 `b` 이하의 정수를 반환.
    
- `random()`: 0.0 이상 1.0 미만의 실수를 반환.
    
- `choice(sequence)`: 리스트, 튜플 같은 시퀀스에서 랜덤한 요소 선택.
    

✅ **시드 값 지정 (**`random.seed()`**)** 난수 생성 결과를 고정하려면 `seed()`를 사용할 수 있습니다.

python

```
random.seed(42)  # 시드 값 설정
print(random.randint(1, 10))  # 실행할 때마다 같은 숫자 출력
```

- 시드 값을 설정하면 같은 난수를 반복적으로 얻을 수 있습니다. (디버깅에 유용)
    

✅ **랜덤 리스트 셔플링 (**`random.shuffle()`**)** 리스트 요소를 랜덤하게 섞을 수도 있습니다.

python

```
data = [1, 2, 3, 4, 5]
random.shuffle(data)  # 리스트 섞기
print(data)
```

