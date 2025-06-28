- **Lightweight Directory Access Protocol**

**디렉터리 서비스에 접근하고 정보를 조회하거나 수정할 수 있게 해주는 프로토콜**, 사용자 인증, 조직 정보 관리, 주소록 서비스 등에 주로 사용



| 용어      | 의미 (영어)             | 역할               | 예시                                 | 설명                                 |
| ------- | ------------------- | ---------------- | ---------------------------------- | ---------------------------------- |
| **dc**  | Domain Component    | 도메인 이름 구성 요소     | `dc=ihd`, `dc=org`                 | 인터넷 도메인 이름을 계층적으로 분리하여 LDAP 트리에 반영 |
| **cn**  | Common Name         | 개체(사람, 그룹 등)의 이름 | `cn=Alice`, `cn=Admins`            | 사용자나 그룹의 고유 이름을 나타냄                |
| **ou**  | Organizational Unit | 조직 단위 (부서 등)     | `ou=People`, `ou=IT`               | 사용자나 자원을 분류하는 그룹 또는 부서             |
| **dn**  | Distinguished Name  | 고유 경로            | `cn=Alice,ou=People,dc=ihd,dc=org` | LDAP 내에서 개체를 유일하게 식별하는 전체 경로       |
| **sn**  | Surname             | 성(last name)     | `sn=Kim`                           | 사용자 성 정보 (이름 구성 요소)                |
| **uid** | User ID             | 사용자 계정 ID        | `uid=alice`                        | 로그인 ID 또는 시스템 계정 ID                |

EX)
`cn=Alice,ou=People,dc=ihd,dc=org`