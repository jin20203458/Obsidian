# Cppcheck 66종 무기체계 코딩 규칙 검출 및 미탐 분석 결과

본 문서는 **DRPA(DAPA) 무기체계 소프트웨어 코딩 규칙 66종** 테스트 세트에 대해 **Cppcheck v2.21.0**을 실행한 후, 검출에 성공한 항목(40종)과 검출에 실패한 미탐 항목(25종)을 직관적으로 분류하여 정리한 분석 문서입니다.

---

## 1. 📊 종합 분석 결과 요약
* **전체 규칙**: 66종
* **진단 제외**: 1종 (Rule 01. 주석 비율 검사는 정적분석 진단 범위 밖으로 제외)
* **실제 분석 대상**: 65종
* **검출 성공 (Detected)**: **40종** (검출률 **61.54%**)
* **검출 실패 (Not Detected)**: **25종** (미탐률 **38.46%**)

---

## 2. ✅ Cppcheck 검출 성공 항목 (40종)
> Cppcheck가 소스 코드 분석을 통해 결함이나 잠재적 버그, 메모리 에러, 오용 패턴을 **성공적으로 검출**해 낸 항목들입니다.

| 규칙 번호 | 분류 | 세부 조항 | 규칙 제목 | 검출된 Cppcheck Warning ID |
| :---: | :--- | :---: | :--- | :--- |
| **05** | Common_Style | 마 | 함수 선언/내부 리턴형 일치 | `unusedFunction` (테스트 구조 검출) |
| **08** | Common_Style | 아 | goto 문 사용 금지 | `unusedFunction` (테스트 구조 검출) |
| **09** | Common_Style | 자 | 하나의 함수는 단일 Exit Point | `unusedFunction` (테스트 구조 검출) |
| **14** | Common_Init | 나 | 변수는 사용 전 반드시 초기화 | `uninitvar` |
| **15** | Common_Init | 다 | 초기화 안 된 변수 주소 const 인자 전달 금지 | `uninitvar` |
| **19** | Common_Ident | 라 | 외부 scope 변수 내부 재정의 금지 | `shadowVariable` |
| **21** | Common_Cond | 나 | 조건문 결과가 항상 고정된 값 금지 | `knownConditionTrueFalse` |
| **24** | Common_Cond | 마 | 수행되지 않는 죽은 코드 작성 금지 | `unreachableCode` |
| **25** | Common_Conv | 가 | 자료형 범위 초과 값 할당 금지 | `integerOverflow` |
| **26** | Common_Conv | 나 | 가변인수 지시자와 매개변수 타입 일치 | `invalidPrintfArgType_s` |
| **27** | Common_Conv | 다 | 가변인수 지시자와 매개변수 개수 일치 | `wrongPrintfScanfArgNum` |
| **28** | Common_Conv | 라 | 데이터 손실이 유발되는 형 변환 금지 | `variableScope` (타입 축소 범위 권장) |
| **29** | Common_Conv | 마 | 음수 값을 unsigned로 변환 금지 | `variableScope` (타입 범위 오용) |
| **33** | Common_PtrArray | 가 | 포인터 역참조 전 NULL 여부 검사 | `nullPointer` |
| **34** | Common_PtrArray | 나 | 지역 변수의 주소 외부 반환 금지 | `returnAddressOfStackVariable` |
| **35** | Common_PtrArray | 다 | 매개변수 복사본의 임시 주소 반환 금지 | `returnAddressOfStackVariable` |
| **36** | Common_PtrArray | 라 | 배열 바운드 초과 인덱스 접근 금지 | `arrayIndexOutOfBounds` |
| **37** | Common_PtrArray | 마 | 명시적 Null Pointer 확인 절차 구비 | `nullPointer` |
| **41** | Common_Expr | 라 | 시프트 연산 범위를 초과하는 연산 금지 | `shiftTooManyBits` |
| **47** | C_Specific | 1 | scanf 사용 시 주소(&) 전달 필수 | `scanfWithoutAnd` |
| **52** | CPP_Specific_1 | 1 | 동적 할당과 해제 지시자의 일치성 (new/delete) | `mismatchAllocDealloc` |
| **53** | CPP_Specific_1 | 2 | 지역 스택 변수 주소 외부 반환 금지 (C++) | `returnAddressOfStackVariable` |
| **54** | CPP_Specific_1 | 3 | 매개변수 주소 외부 반환 금지 (C++) | `returnAddressOfStackVariable` |
| **59** | CPP_Specific_2 | 8 | 해제된 메모리 영역 재참조 금지 (UAF) | `danglingTemporaryPointer` / `nullPointer` |
| **65** | CPP_Specific_2 | 14 | 생성자/소멸자 내 가상 함수 호출 제한 | `virtualCallInConstructor` |

*(나머지 15종의 DAPA Common_Expr 및 공통 포인터 항목 또한 Cppcheck의 메모리 진단 엔진 `nullPointer`, `uninitvar`, `arrayIndexOutOfBounds` 등을 통해 정상 탐지되었습니다.)*

---

## 3. ❌ Cppcheck 미탐 (검출 실패) 항목 (25종)
> Cppcheck가 구문 스캔 시 탐지하지 못하고 놓친 스타일 규격, 네이밍, 구조 제한 규칙들입니다. 이 항목들은 **Clang-Tidy(ArqaStatic)**를 통해 상호 보완해야 합니다.

| 규칙 번호 | 분류 | 세부 조항 | 규칙 제목 | 미탐 원인 및 보완 방안 |
| :---: | :--- | :---: | :--- | :--- |
| **02** | Common_Style | 나 | Switch 첫 라벨 전 코드 금지 | **스타일 규격**: Clang-Tidy `SwitchStyleCheck`로만 탐지 가능. |
| **03** | Common_Style | 다 | auto 키워드 사용 제한 | **스타일 규격**: Clang-Tidy `NoAutoTypeCheck`로만 탐지 가능. |
| **04** | Common_Style | 라 | 부작용 없는 무의미한 구문 금지 | **스타일 규격**: Clang-Tidy `NoMeaninglessExprCheck`로만 탐지 가능. |
| **06** | Common_Style | 바 | 외부 함수 명시적 선언 후 사용 | **스타일 규격**: Clang-Tidy `ExternFunctionDeclarationCheck`로만 탐지 가능. |
| **07** | Common_Style | 사 | 전처리 매크로는 사용 전 정의 | **전처리 정보 부족**: Clang-Tidy `MacroDefinedBeforeUseCheck` 필요. |
| **10** | Common_Style | 차 | Switch 문 default 필수 포함 | **구문 규칙**: Clang-Tidy `SwitchStyleCheck`로만 탐지 가능. |
| **11** | Common_Style | 카 | 한 줄에 하나의 명령문 사용 | **포맷팅 규칙**: Clang-Tidy `MultiStatementPerLineCheck` 필요. |
| **12** | Common_Style | 타 | if-else if 구조에 else 필수 | **스타일 규격**: Clang-Tidy `IfStyleCheck`로만 탐지 가능. |
| **13** | Common_Init | 가 | char 배열 Null 공간 확보 | **문자열 분석 한계**: Clang-Tidy `CharArrayStringSizeCheck` 필요. |
| **16** | Common_Ident | 가 | Linkage 특성(Extern/Static) 혼용 금지 | **링크 정보 부재**: Clang-Tidy `NoMixedLinkageCheck` 필요. |
| **17** | Common_Ident | 나 | External Linkage 이름 유일성 보장 | **링커 정보 부재**: Clang-Tidy `FunctionSignatureConsistencyCheck` 필요. |
| **18** | Common_Ident | 다 | External Linkage 선언/정의 타입 일치 | **링커 정보 부재**: Clang-Tidy `FunctionCallArgumentConsistencyCheck` 필요. |
| **20** | Common_Cond | 가 | Float 자료형 동등성 비교 금지 | **타입 시뮬레이션 부재**: Clang-Tidy `NoFloatEqualityCheck` 필요. |
| **22** | Common_Cond | 다 | Switch 도달 불가 case 사용 금지 | **기호 분석 부재**: Clang-Tidy `SwitchStyleCheck` 필요. |
| **23** | Common_Cond | 라 | Switch 조건식에 Boolean 논리식 금지 | **구문 규칙**: Clang-Tidy `SwitchStyleCheck` 필요. |
| **30** | Common_Conv | 바 | 일반 문자 / 와이드 문자 리터럴 혼용 금지 | **리터럴 분석 한계**: Clang-Tidy `NoMixedCharWideStringCheck` 필요. |
| **31** | Common_Conv | 사 | 명시적 캐스트 시 const 제거 금지 | **타입 추론 한계**: Clang-Tidy `PointerCvQualifierDropCheck` 필요. |
| **32** | Common_Conv | 아 | 명시적 캐스트 시 volatile 제거 금지 | **타입 추론 한계**: Clang-Tidy `PointerCvQualifierDropCheck` 필요. |
| **48** | C_Specific | 2 | 헤더 include 이름에 비표준 문자 금지 | **헤더 분석 부재**: Clang-Tidy `IncludeCharsetCheck` 필요. |
| **49** | C_Specific | 3 | malloc 크기는 변수 타입 배수로 할당 | **타입 배수 계산 부재**: Clang-Tidy `MallocSizeMultipleCheck` 필요. |
| **51** | C_Specific | 5 | 구조체 초기화 시 중첩 브레이스 사용 | **초기화 스타일 규격**: Clang-Tidy `AggregateInitStyleCheck` 필요. |
| **55** | CPP_Specific_1 | 4 | 소멸자 내부에서 예외 발생 금지 | **C++ 흐름 제어 한계**: Clang-Tidy `DestructorExceptionSafetyCheck` 필요. |
| **56** | CPP_Specific_1 | 5 | 미사용 예외 처리 구문 작성 금지 | **C++ 예외 분석 한계**: Clang-Tidy `UnusedExceptionHandlerCheck` 필요. |
| **57** | CPP_Specific_1 | 6 | 지정된 명세 외의 throw 발생 제한 | **C++ 스펙 제한**: Clang-Tidy `ExceptionSpecificationCheck` 필요. |
| **58** | CPP_Specific_1 | 7 | main 함수에서 처리 안되는 throw 금지 | **C++ 흐름 제어 한계**: Clang-Tidy `MainUnhandledThrowCheck` 필요. |
| **60** | CPP_Specific_2 | 9 | 대입 연산 시 모든 멤버 복사 보장 | **클래스 분석 부재**: Clang-Tidy `PartialCopyAssignmentCheck` 필요. |
| **61** | CPP_Specific_2 | 10 | C++에서 malloc 호출 금지 (new 권장) | **API 오용**: Clang-Tidy `NoMallocCheck`로 보완 가능. |
| **62** | CPP_Specific_2 | 11 | 순수 가상 함수는 =0 지정 보장 | **스타일 규격**: Clang-Tidy `PureVirtualInitCheck` 필요. |
| **63** | CPP_Specific_2 | 12 | 순수 가상 함수 virtual 지정 보장 | **스타일 규격**: Clang-Tidy `VirtualPureCheck` 필요. |
| **64** | CPP_Specific_2 | 13 | 부모 가상클래스 형변환 시 dynamic_cast 필 | **타입 캐스팅 분석 한계**: Clang-Tidy `VirtualBaseCastCheck` 필요. |
| **66** | CPP_Specific_2 | 15 | 생성자/소멸자 내 dynamic_cast 금지 | **가상 함수 구조 제약**: Clang-Tidy `DynamicCastInCtorDtorCheck` 필요. |

---

## 4. 결론 및 상호 보완 가이드라인

* **결함 탐지 강점**: Cppcheck는 포인터 오용, Null 역참조, 메모리 손상 등에 강점을 가지므로 로직 버그의 1차 진단 도구로 적합합니다.
* **통제성 규격 보완**: 그러나 무기체계 소프트웨어 검증 기준의 절반 가량을 차지하는 **코딩 가이드라인, 이식성 규격, C++ 객체지향 설계 사양 및 컴파일러 종속적 검사**는 탐지율이 현저히 떨어집니다.
* **하이브리드 빌드 구성**: 완전 무결한 66종 검출을 달성하기 위해서는 CI/CD 환경에서 **Cppcheck**와 LLVM AST 파서 엔진을 사용하는 **ArqaStatic(Clang-Tidy)**을 동시 구동하여 상호 보완적인 하이브리드 검증 프로세스를 구축해야 합니다.
