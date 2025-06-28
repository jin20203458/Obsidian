### ✅ `enum class`란?

C++의 `enum class`(Scoped Enum)는 기존 `enum`보다 **타입 안전성(type safety)**을 제공하는 열거형(enum)이다.  
**네임스페이스가 적용**되어 **이름 충돌을 방지**하며, **암시적 형 변환이 되지 않음**이 특징이다.

---

### ✅ `enum class`의 특징

1️⃣ **스코프(scope) 적용**

- `enum class`를 사용하면 반드시 **이름을 명시해야 접근 가능**
- 예: `PacketType::PlayerUpdate` 처럼 접근

2️⃣ **암시적 형 변환 방지**

- 기존 `enum`은 정수형으로 암시적 변환됨
- `enum class`는 **형 변환을 명시적으로 해야 함** (`static_cast` 사용)

3️⃣ **기본 크기 지정 가능 (`uint8_t`, `int`, `char` 등)**

- 기본적으로 `int`를 사용하지만, **메모리 절약을 위해 `uint8_t` 등 명시 가능**

---

### ✅ `enum class` 기본 예제

```
#include <iostream>

enum class PacketType : uint8_t 
{
    PlayerInit = 0x01,     // 플레이어 초기화
    PlayerUpdate = 0x02,   // 플레이어 상태 업데이트
    MonsterUpdate = 0x03,  // 몬스터 상태 업데이트
    WorldUpdate = 0x04     // 월드 전체 상태 업데이트
};

void processPacket(PacketType type) 
{
    switch (type) {
        case PacketType::PlayerInit:
            std::cout << "Initializing player...\n";
            break;
        case PacketType::PlayerUpdate:
            std::cout << "Updating player state...\n";
            break;
        case PacketType::MonsterUpdate:
            std::cout << "Updating monster state...\n";
            break;
        case PacketType::WorldUpdate:
            std::cout << "Updating world state...\n";
            break;
        default:
            std::cout << "Unknown packet type!\n";
    }
}

int main() {
    PacketType packet = PacketType::PlayerUpdate;
    processPacket(packet);  // "Updating player state..." 출력

    // 형 변환이 필요한 경우 (암시적 변환 방지됨)
    std::cout << "PacketType::PlayerUpdate 값: " << static_cast<int>(packet) << '\n';

    return 0;
}
```

---

### ✅ `enum class` vs `enum` 비교

| 비교 항목         | `enum`           | `enum class`                 |
| ------------- | ---------------- | ---------------------------- |
| **네임스페이스**    | 전역에서 접근 가능       | `EnumName::Value` 형태로 접근해야 함 |
| **암시적 변환**    | 정수형(int)으로 자동 변환 | 명시적 형 변환 필요 (`static_cast`)  |
| **타입 안정성**    | 낮음               | 높음                           |
| **메모리 크기 지정** | 불가능              | 가능 (`uint8_t`, `int` 등)      |
