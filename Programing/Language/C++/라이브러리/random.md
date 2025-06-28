C++에서 랜덤 번호를 생성하기 위해 사용되는 `random` 헤더 파일은 C++11부터 도입되었습니다. 이전에는 `rand()` 함수와 `srand()`를 사용했지만, `random` 헤더는 더 유연하고 정확한 난수 생성을 제공합니다.

`random` 헤더 파일에는 난수 생성을 위한 다양한 클래스와 함수들이 정의되어 있습니다. 주요 컴포넌트는 다음과 같습니다:

### 1. `std::random_device`

`std::random_device`는 진정한 난수를 생성하는 데 사용됩니다. 하드웨어 기반의 난수 생성 장치가 존재할 경우 그것을 사용하여 랜덤 값을 생성합니다. 주로 시드 값으로 사용됩니다.

`#include <random>  std::random_device rd; `

### 2. `std::mt19937`

`std::mt19937`은 Mersenne Twister 알고리즘을 기반으로 한 난수 생성기입니다. 이 난수 생성기는 고속으로 난수를 생성하며, 실제로 대부분의 응용 프로그램에서 사용됩니다.


`std::mt19937 gen(rd()); 
`// random_device로 시드값을 설정한 Mersenne Twister`

### 3. `std::uniform_int_distribution` 및 `std::uniform_real_distribution`

이들은 난수를 특정 범위 내에서 균등하게 생성할 수 있게 해주는 분포 함수입니다.

- **`std::uniform_int_distribution`**: 정수형 범위에서 균등한 분포로 난수를 생성합니다.

cpp

복사편집

`std::uniform_int_distribution<> dis(1, 6); // 1부터 6까지의 정수`

- **`std::uniform_real_distribution`**: 실수형 범위에서 균등한 분포로 난수를 생성합니다.

cpp

복사편집

`std::uniform_real_distribution<> dis(0.0, 1.0); // 0.0부터 1.0까지의 실수`

### 4. 사용 예시

```
#include <iostream>
#include <random>
#include <iomanip>

int main() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.0, 1.0);

    for (int i = 0; i < 10; ++i) {
        // 소숫점 2자리로 출력
        std::cout<< std::setprecision(2) << dis(gen) << " ";
    }
    std::cout << std::endl;

    return 0;
}
```

### 5. 다양한 분포

`random` 헤더는 다양한 분포를 지원하여, 사용자 정의 분포에 맞는 난수를 생성할 수 있습니다. 예를 들어, **정규 분포**(`std::normal_distribution`)나 **포아송 분포**(`std::poisson_distribution`)도 사용할 수 있습니다.

### 결론

C++의 `random` 헤더는 더 정밀하고 유연한 난수 생성 방법을 제공합니다. `std::random_device`, `std::mt19937`, `std::uniform_int_distribution` 등 다양한 도구들을 결합하여 요구 사항에 맞는 난수를 생성할 수 있습니다.