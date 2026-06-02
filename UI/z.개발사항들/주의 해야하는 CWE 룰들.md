
제공해주신 목록을 CWE 공식 가이드라인(v4.16 기준)의 **'Mapping Friendly'** 상태에 따라 분류하여 표로 정리해 드립니다.

---

## 🚫 1. DISCOURAGED (매핑 권장 안 함)

이 항목들은 **추상화 수준(Class)**이 너무 높아, 실제 취약점의 근본 원인을 설명하기에 부적절합니다. 분석 시 반드시 **하위(Base/Variant) 항목**을 찾아 매핑해야 합니다.

|**CWE ID**|**명칭**|**유형**|**권장되지 않는 이유 (대체 필요성)**|
|---|---|---|---|
|**CWE-119**|Improper Restriction of Operations within the Bounds of a Memory Buffer|**Class**|메모리 관리 실수의 '결과'에 가깝습니다. 구체적으로 쓰기(787)인지 읽기(125)인지 명시가 필요합니다.|
|**CWE-285**|Improper Authorization|**Class**|인가 실패의 최상위 개념입니다. 수평적/수직적 권한 상승 등 구체적인 메커니즘을 명시해야 합니다.|
|**CWE-330**|Use of Insufficiently Random Values|**Class**|난수가 부족한 이유(시드 문제, 알고리즘 문제 등)를 설명하지 못하므로 하위 항목 사용이 필수입니다.|
|**CWE-400**|Uncontrolled Resource Consumption|**Class**|자원 소모의 대상과 방식이 모호합니다. 원인에 기반한 구체적인 자원 관리 룰을 써야 합니다.|
|**CWE-755**|Improper Handling of Exceptional Conditions|**Class**|예외 처리 미흡의 결과가 보안에 미치는 영향(정보 노출 등)을 구체화해야 합니다.|

---

## ⚠️ 2. ALLOWED (with review) (검토 후 허용)

이 항목들은 **Class 수준**이지만, 특정 상황에서 매핑이 허용됩니다. 단, **"이보다 더 구체적인 하위 룰이 없는지 반드시 확인하라"**는 주의 사항이 붙습니다.

|**CWE ID**|**명칭**|**유형**|**매핑 시 주의사항**|
|---|---|---|---|
|**CWE-312**|Cleartext Storage of Sensitive Information|**Class**|저장되는 매체(로그, 파일, DB 등)에 따른 구체적인 하위 항목이 있는지 먼저 확인해야 합니다.|
|**CWE-327**|Use of a Broken or Risky Cryptographic Algorithm|**Class**|단순히 '취약한 암호화'라고 하기보다, 특정 알고리즘(AES-ECB 등)의 문제인지 구체화가 권장됩니다.|
|**CWE-404**|Improper Resource Shutdown or Release|**Class**|자원 해제 미흡의 일반적 형태입니다. 메모리 누수(401) 등 구체적 자원 유형이 있다면 그것을 우선합니다.|
|**CWE-497**|Exposure of Sensitive System Information to an Unauthorized Control Sphere|**Class**|시스템 정보 노출의 포괄적 개념입니다. 노출 경로(에러 메시지 등)가 명확하다면 하위 항목을 씁니다.|
|**CWE-732**|Incorrect Permission Assignment for Critical Resource|**Class**|권한 설정 오류의 일반 형태입니다. 파일 시스템인지, 객체인지 등 대상에 따라 하위 항목을 고려합니다.|

---

## 💡 참고: 그 외 항목들 (ALLOWED)

목록에 포함된 나머지 항목들(CWE-22, 78, 89, 190, 476 등)은 **Base** 또는 **Variant** 수준으로, 실제 취약점 분석 및 보고 시 **공식적으로 매핑이 권장되는(Allowed)** 항목들입니다.

> **핵심 요약:**
> 
> - **DISCOURAGED:** "이것은 너무 넓으니 쓰지 말고 더 구체적인 것을 찾아라."
>     
> - **ALLOWED (with review):** "쓸 수는 있지만, 더 딱 맞는 하위 항목이 있는지 한 번 더 검토해라."
>     

이 리스트를 바탕으로 보안 진단 리포트를 작성하시나요? 혹시 특정 CWE를 대체할 하위 항목을 찾는 데 도움이 필요하시면 말씀해 주세요!