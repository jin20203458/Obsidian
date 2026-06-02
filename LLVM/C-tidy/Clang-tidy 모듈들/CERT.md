| 카테고리           | 체커 이름                               | 설명                                                                                     |
| -------------- | ----------------------------------- | -------------------------------------------------------------------------------------- |
| 시스템 명령 보안      | CommandProcessorCheck               | system()같은 명령 프로세서 사용을 감지합니다 .CERT ENV33-C 규칙에 해당합니다 .                                 |
| 난수 생성          | LimitedRandomnessCheck              | std::rand()처럼 제한된 난수 생성 함수 사용을 감지합니다 .CERT MSC50-CPP/MSC30-C 규칙에 해당합니다 .               |
|                | ProperlySeededRandomGeneratorCheck  | 난수 생성기가 적절히 시드되지 않은 경우를 감지합니다 .                                                        |
| 문자열 변환         | StrToNumCheck                       | atoi()같이 에러 보고를 하지 않는 문자열-숫자 변환 함수 사용을 감지합니다 .CERT ERR34-C 규칙에 해당하며strtol()사용을 권장합니다 . |
| 메모리 정렬         | DefaultOperatorNewAlignmentCheck    | 기본operator new의 정렬 문제를 검사합니다 .                                                         |
| 네임스페이스 수정      | DontModifyStdNamespaceCheck         | 표준 네임스페이스(std)를 수정하는 경우를 감지합니다 .CERT DCL58-CPP 규칙에 해당합니다 .                             |
| 루프 카운터         | FloatLoopCounter                    | 부동소수점 타입을 루프 카운터로 사용하는 경우를 감지합니다 .정밀도 문제로 인한 버그를 방지합니다 .                               |
| 복사 연산          | MutatingCopyCheck                   | 복사 중 객체를 변경하는 경우를 감지합니다 .                                                              |
| 메모리 함수         | NonTrivialTypesLibcMemoryCallsCheck | non-trivial 타입에 대해memcpy,memset등 C 라이브러리 메모리 함수를 사용하는 경우를 감지합니다 .                      |
| setjmp/longjmp | SetLongJmpCheck                     | C++에서setjmp/longjmp사용을 감지합니다 .예외 처리 메커니즘과 충돌할 수 있습니다 .                                 |
| 정적 객체 예외       | StaticObjectExceptionCheck          | 정적 객체의 생성자나 소멸자에서 예외가 발생하는 경우를 감지합니다 .CERT ERR58-CPP 규칙에 해당합니다 .                       |
| 예외 타입          | ThrownExceptionTypeCheck            | 던져진 예외의 타입을 검사합니다 .                                                                    |
| 가변 인자 함수       | VariadicFunctionDefCheck            | 가변 인자 함수 정의의 안전성을 검사합니다 .                                                              |