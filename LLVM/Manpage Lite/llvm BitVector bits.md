> `llvm::BitVector`는 LLVM에서 자주 쓰이는 **비트 단위 컨테이너**로, `bool` 값들을 **진짜 비트 단위로 압축해서 저장**하는 고성능 구조체. 


---

## 🧱 기본 구조

```cpp
#include "llvm/ADT/BitVector.h"

llvm::BitVector bits;
```

- 내부적으로 `uintptr_t` 기반 워드 배열(`uint32_t` 또는 `uint64_t`)에 **비트를 압축 저장**
- 각 비트는 `0` 또는 `1`로 표현
- `size()`는 **현재 유효 비트 개수**, 워드 단위 용량은 `getBitCapacity()`로 확인

---

## 🟢 초기화 방법

### 1. 기본 생성자 (빈 벡터)

```cpp
llvm::BitVector bits;  // size = 0
```

### 2. 크기와 초기값 지정

```cpp
llvm::BitVector bits(10, true);   // 10비트를 모두 1로 초기화
llvm::BitVector bits(10, false);  // 10비트를 모두 0으로 초기화
```

### 3. 크기 변경

```cpp
bits.resize(20);           // 새 비트는 기본값 false
bits.resize(20, true);     // 새 비트를 true로 초기화
bits.reserve(100);         // 용량만 확보 (성능 최적화)
```

---

## ➕ 요소 추가 및 설정

`BitVector`는 **끝에 비트를 추가하는 `push_back()`**과  
**인덱스를 통한 직접 설정**을 모두 지원합니다.

```cpp
bits.push_back(true);   // size += 1, 마지막 비트 = 1
bits.push_back(false);  // size += 1, 마지막 비트 = 0

bits[3] = true;         // 4번째 비트를 true로 설정 (reference proxy)
bits.set(5);            // 6번째 비트를 true로 설정
bits.reset(5);          // 6번째 비트를 false로 설정
bits.flip(5);           // 6번째 비트를 반전
```

> ⚠️ `set(i)` 호출 시 `i >= size()`면 디버그 빌드에서 assert 실패 →  
> 반드시 `resize()`나 `push_back()`으로 크기를 확보한 뒤 접근

---

## 🔍 상태 확인

```cpp
bits.any();     // 하나라도 true면 true
bits.none();    // 모두 false면 true
bits.all();     // 모두 true면 true
bits.count();   // true인 비트 개수
bits.size();    // 전체 비트 개수
bits.empty();   // size() == 0
bits.back();    // 마지막 비트 값
```

---

## 🔁 비트 연산

```cpp
bits1 &= bits2;   // 교집합 (AND)
bits1 |= bits2;   // 합집합 (OR)
bits1 ^= bits2;   // XOR
bits1.reset(bits2); // bits1 &= ~bits2
bits1.anyCommon(bits2); // 공통 1비트가 있는지 확인
```

---

## 🎯 구간 단위 설정/해제

```cpp
bits.set(2, 5);    // [2,5) 구간을 1로
bits.reset(0, 3);  // [0,3) 구간을 0으로
```

---

## 🔍 탐색 유틸리티

```cpp
bits.find_first();        // 첫 번째 1비트 인덱스, 없으면 -1
bits.find_next(3);        // 3 이후의 다음 1비트
bits.find_first_unset();  // 첫 번째 0비트
bits.find_next_unset(5);  // 5 이후의 다음 0비트
```

---

## 🧠 요약 표

|기능|메서드|
|---|---|
|초기화|`BitVector(size, value)`|
|크기 변경|`resize(size, value)`, `reserve(n)`|
|끝에 추가|`push_back(bool)`, `pop_back()`|
|비트 설정|`set(i)`, `reset(i)`, `flip(i)`|
|구간 설정|`set(I,E)`, `reset(I,E)`|
|상태 확인|`any()`, `none()`, `all()`, `count()`, `size()`|
|비트 연산|`&=`, `|=`,`^=`,`reset(BitVector)`,`anyCommon()`|
|탐색|`find_first()`, `find_next(i)`, `find_first_unset()` 등|

---

형진님이 이걸 기반으로 **경로 병합, 초기화 추적, CFG 분석** 같은 정적 분석 로직을 짜실 때,  
`find_first_unset()`·`find_next()`·`anyCommon()` 같은 메서드가 특히 유용합니다.

원하시면 제가 `BitVector`를 이용한 **초기화 상태 추적 예제 코드**도 만들어드릴까요?  
그러면 `meetAndAssign` 같은 병합 로직이 실제로 어떻게 동작하는지 바로 보실 수 있습니다.