|카테고리|체커 이름|설명|
|---|---|---|
|불필요한 복사|UnnecessaryCopyInitialization|불필요한 객체 복사 초기화를 감지합니다 .|
||UnnecessaryValueParamCheck|값으로 전달되지만 수정되지 않는 매개변수를 const 참조로 변경하도록 권장합니다 .|
||ForRangeCopyCheck|range-based for 루프에서 불필요한 복사를 감지합니다 .|
|이동 시맨틱스|MoveConstArgCheck|const 객체에 대한 불필요한std::move사용을 감지합니다 .|
||MoveConstructorInitCheck|이동 생성자에서 멤버를 복사하는 대신 이동해야 하는 경우를 감지합니다 .|
||NoAutomaticMoveCheck|암묵적 이동이 발생하지 않는 경우를 감지합니다 .|
|루프 최적화|ImplicitConversionInLoopCheck|루프 내에서 암묵적 타입 변환으로 인한 성능 저하를 감지합니다 .|
||InefficientVectorOperationCheck|루프 내vector의 비효율적인 연산(반복적인push_back등)을 감지합니다 .|
|문자열 최적화|FasterStringFindCheck|string::find에서 단일 문자 검색 시 문자열 대신 문자 사용을 권장합니다 .|
||InefficientStringConcatenationCheck|루프 내에서 반복적인 문자열 연결로 인한 성능 저하를 감지합니다 .|
|알고리즘 효율성|InefficientAlgorithmCheck|비효율적인 STL 알고리즘 사용을 감지합니다 .|
|noexcept 최적화|NoexceptDestructorCheck|소멸자에noexcept지정을 권장합니다 .|
||NoexceptMoveConstructorCheck|이동 생성자에noexcept지정을 권장합니다 .|
||NoexceptSwapCheck|swap 함수에noexcept지정을 권장합니다 .|
||NoexceptFunctionBaseCheck|noexcept 함수 체크를 위한 기본 클래스입니다 .|
|타입 최적화|TypePromotionInMathFnCheck|수학 함수에서 불필요한 타입 승격을 감지합니다 .|
||TriviallyDestructibleCheck|trivially destructible 타입을 활용할 수 있는 경우를 감지합니다 .|
||EnumSizeCheck|enum의 크기를 최적화할 수 있는 경우를 권장합니다 .|
|I/O 최적화|AvoidEndlCheck|std::endl대신'\n'사용을 권장합니다 .|
|포인터 변환|NoIntToPtrCheck|정수를 포인터로 변환하는 비효율적인 연산을 감지합니다 .|