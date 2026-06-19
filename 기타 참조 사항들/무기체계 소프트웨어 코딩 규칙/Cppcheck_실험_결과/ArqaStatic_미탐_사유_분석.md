# ArqaStatic 최종 미탐 (False Negative) 사유 분석

모든 튜닝과 모드 분리 후에도 남은 **최종 미탐 5종**의 분석 내용입니다. 이들은 정적 분석 엔진(AST 검사기)의 구조적 한계 또는 C/C++ 언어 스펙 자체에 기인한 필연적 미탐입니다.

## 1. 컴파일러 원천 차단 (Fatal Error)
코드에 존재하는 결함이 논리적 오류를 넘어서서, **치명적인 C/C++ 구문 오류(Syntax Error)**에 해당하여 정적 분석기가 도달하기 전에 컴파일러(Clang)가 분석을 중단시켜버립니다.
* **Rule 30**: Character 문자열과 Wide character 문자열 혼용 (Cannot initialize variable)
* **Rule 58**: main 함수에서 처리되지 않는 throw 작성 금지
* **Rule 62**: 순수 가상 함수는 반드시 '= 0' 으로 초기화 (클래스 문법 오류)
* **Rule 63**: 순수 함수는 반드시 virtual로 선언 (가상 함수 선언 오류)

> **대응 방안**: 도구 차원의 미탐이라기보다는, 개발자가 코드를 빌드하는 시점에 즉시 에러로 검출되므로 실제 품질 저하로 이어지지 않는 안전한 미탐입니다.

## 2. 링킹(추후 재시도) 분석 한계
단일 파일(AST) 분석만 진행하는 현재 환경의 구조적 한계입니다.
* **Rule 17**: External linkage scope에서 선언된 함수나 Object의 이름은 유일해야 한다.

> **대응 방안**: 전역 심볼 중복은 링커(Linker) 단계에서 `multiple definition` 에러로 잡히거나, 전체 프로젝트 단위의 CTU (Cross-Translation Unit) 분석 아키텍처를 연동해야만 탐지할 수 있습니다.
