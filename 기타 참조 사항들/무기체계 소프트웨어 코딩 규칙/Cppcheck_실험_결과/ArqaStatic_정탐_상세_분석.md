# ArqaStatic 정탐 (True Positive) 상세 분석

ArqaStatic이 검출해낸 60개의 규칙 중 핵심적인 유형을 정리합니다.

## 1. 컴파일 모드 분리(C/C++)로 성공한 검출
C 문법과 C++ 문법이 섞였을 때 Clang 파서가 멈춰서 검출하지 못했던 규칙들이, **-x c** 모드로 분리 검사하여 모두 정탐으로 전환되었습니다.
* **Rule 06**: 외부 함수 선언 부재 (C 모드 성공)
* **Rule 13**: char 배열 초기화 사이즈 오버플로우 (C 모드 성공)
* **Rule 16**: External/Internal Linkage 혼용 (C 모드 성공)
* **Rule 22**: switch-case 도달 불가능한 상수 조건 (C 모드 성공)
* **Rule 24**: switch 문 내부 도달 불가능한 코드 (C 모드 성공)
* **Rule 48**: include 구문에서 비표준 문자 사용 (C 모드 성공)
* **Rule 58**: main 함수에서 처리되지 않는 throw (다른 문법 파괴 규칙들로부터 격리하여 파싱에 성공했을 때 정상 검출 완료)

## 2. 체커 로직 수정을 통해 성공한 검출
* **Rule 42**: Unsigned 타입에 단항 '-' 적용 금지
  * (수정사항): `UnsignedMinusAssignmentCheck` 내부 로직을 타겟 변수(LHS)가 아닌 피연산자(RHS) 기준으로 판단하도록 튜닝하여 완벽하게 검출에 성공.

## 3. Cppcheck가 놓쳤지만 ArqaStatic이 완벽히 잡은 항목
* **코딩 스타일 검사**: Cppcheck는 AST 분석의 한계로 단순 스타일(예: 함수 선언시 auto 사용 금지, goto 금지, switch default 의무화)을 잡지 못하나, ArqaStatic은 `ast-` 기반의 정밀 매처로 100% 탐지해냄.
* **타입 변환 안정성**: 정수/실수 축소 변환, 부호 변환 등에 대해 Clang의 자료형 분석력을 바탕으로 정교하게 경고 발생 (`ast-representable-cast` 등).
