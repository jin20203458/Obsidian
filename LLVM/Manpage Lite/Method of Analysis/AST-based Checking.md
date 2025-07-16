> **핵심 아이디어**: 코드의 **구조(Syntax)와 패턴**을 분석하여 버그를 찾습니다. 마치 맞춤법 검사기나 문법 교정기가 글의 의미를 깊게 이해하지 않고도 문법적 오류를 찾아내는 것과 같습니다


**동작 방식**:  
이 방식은 코드를 컴파일하여 만들어진 **추상 구문 트리(Abstract Syntax Tree, AST)**를 직접 순회합니다[7](https://par.nsf.gov/servlets/purl/10285754). AST는 코드의 구조를 나무 형태로 표현한 것입니다[6](https://github.com/haoNoQ/clang-analyzer-guide/blob/master/clang-analyzer-guide.tex). 체커는 이 나무를 처음부터 끝까지 돌아다니며, 특정 규칙에 어긋나는 코드 패턴을 찾습니다[6](https://github.com/haoNoQ/clang-analyzer-guide/blob/master/clang-analyzer-guide.tex).

- **예시**: "C++ 프로젝트에서 C-style 캐스트를 사용하지 말자"는 규칙이 있다면, AST 기반 체커는 코드를 순회하다가 `(Type)expression` 형태의 C-style 캐스트 노드를 발견할 때마다 경고를 보낼 수 있습니다[6](https://github.com/haoNoQ/clang-analyzer-guide/blob/master/clang-analyzer-guide.tex).
    

**장점**:

- **속도**: 코드의 실행을 시뮬레이션하지 않고 구조만 보기 때문에 분석 속도가 컴파일만큼 빠릅니다[6](https://github.com/haoNoQ/clang-analyzer-guide/blob/master/clang-analyzer-guide.tex).
    
- **단순성**: 구현이 비교적 간단하고 명확한 코드 패턴을 찾는 데 매우 효과적입니다[6](https://github.com/haoNoQ/clang-analyzer-guide/blob/master/clang-analyzer-guide.tex)[7](https://par.nsf.gov/servlets/purl/10285754).
    

**단점**:

- **분석 깊이의 한계**: 프로그램의 실행 흐름이나 변수의 실제 값을 추적하지 못합니다[7](https://par.nsf.gov/servlets/purl/10285754). 예를 들어, `z = 0; ...; y = x / z;`와 같이 변수 `z`의 값이 0이 되어 발생하는 '0으로 나누기' 오류는 변수의 상태를 추적해야 하므로 AST 분석만으로는 찾기 매우 어렵습니다[6](https://github.com/haoNoQ/clang-analyzer-guide/blob/master/clang-analyzer-guide.tex).
    

**대표적인 체커: `CastToStructChecker`**  
이 체커는 "구조체가 아닌 타입을 구조체로 캐스팅하는가?" 또는 "작은 구조체를 더 큰 구조체로 캐스팅하는가?"와 같은 **타입의 구조적인 정보**만으로 버그를 판단할 수 있습니다. 변수의 값이나 실행 경로와는 무관하므로, 빠르고 간단한 AST 기반 분석이 최적의 도구입니다.