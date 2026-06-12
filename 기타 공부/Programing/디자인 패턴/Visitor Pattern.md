

**Visitor Pattern**은 객체 지향 디자인 패턴 중 하나로, **객체의 구조를 변경하지 않고 새로운 연산을 추가할 수 있도록** 해주는 패턴입니다. 주로 복잡한 객체 구조를 가지고 있고, 이 구조에 다양한 연산을 적용해야 할 때 유용하게 사용됩니다.

### Visitor Pattern이 필요한 이유

객체 구조가 고정되어 있다고 가정해봅시다. 이 구조에 새로운 연산을 추가할 때마다 각 객체 클래스를 수정해야 한다면 다음과 같은 문제가 발생할 수 있습니다.

- **OCP (Open/Closed Principle) 위반:** 기존 코드를 수정해야 하므로 확장에 닫혀있고 변경에 열려있지 않게 됩니다.
    
- **코드 응집도 저하:** 하나의 연산이 여러 클래스에 분산되어 구현되므로 코드를 이해하고 유지보수하기 어려워집니다.
    
- **재사용성 저하:** 특정 연산이 특정 객체 구조에 강하게 결합되어 다른 곳에서 재사용하기 어렵습니다.
    

Visitor Pattern은 이러한 문제들을 해결하기 위해 고안되었습니다.

### Visitor Pattern의 주요 구성 요소

Visitor Pattern은 크게 두 가지 타입의 객체로 구성됩니다.

1. **Visitor (방문자):**
    
    - **방문할 객체(Element)의 타입별로 연산을 정의하는 인터페이스 또는 추상 클래스입니다.**
        
    - 각 `visit()` 메서드는 특정 `Element` 객체를 인자로 받아 해당 `Element`에 대한 특정 연산을 수행합니다.
        
    - 새로운 연산을 추가할 때 Visitor 인터페이스를 구현하는 새로운 Visitor 클래스를 만들면 됩니다.
        
2. **Element (요소):**
    
    - **Visitor 객체를 "수락(accept)"하는 메서드를 정의하는 인터페이스 또는 추상 클래스입니다.**
        
    - `accept(Visitor visitor)` 메서드를 통해 자신을 방문할 Visitor 객체를 받습니다. 이 메서드 내부에서 Visitor의 적절한 `visit()` 메서드를 호출하여 자신에 대한 연산을 수행하도록 위임합니다.
        
    - 각 구체적인 Element 클래스는 이 `accept()` 메서드를 구현하여 해당 Visitor의 `visit()` 메서드를 호출합니다.
        

### Visitor Pattern의 동작 방식

1. 클라이언트는 특정 `Element` 객체에 대한 연산을 수행하기 위해 `Visitor` 객체를 생성합니다.
    
2. 클라이언트는 `Element` 객체의 `accept()` 메서드를 호출하면서 생성한 `Visitor` 객체를 인자로 전달합니다.
    
3. `Element` 객체 내부의 `accept()` 메서드는 전달받은 `Visitor` 객체의 적절한 `visit()` 메서드를 호출합니다. 이때 `this` 키워드를 사용하여 자기 자신(`Element` 객체)을 `visit()` 메서드의 인자로 전달합니다.
    
4. `Visitor` 객체의 `visit()` 메서드는 전달받은 `Element` 객체에 대한 실제 연산을 수행합니다.
    

### Visitor Pattern의 장점

- **새로운 연산 추가 용이:** 객체 구조를 변경하지 않고 새로운 Visitor를 추가함으로써 쉽게 새로운 연산을 정의할 수 있습니다. 이는 OCP를 따르는 좋은 예시입니다.
    
- **책임 분리:** 각 연산이 Visitor 클래스에 캡슐화되어 객체 구조(Element)와 연산(Visitor)의 책임이 명확하게 분리됩니다.
    
- **코드 응집도 향상:** 특정 연관된 연산들을 하나의 Visitor 클래스에 모아 관리할 수 있으므로 코드의 응집도가 높아집니다.
    
- **객체 구조의 불변성 유지:** Element 클래스들이 특정 연산으로 인해 자주 변경될 필요가 없으므로 객체 구조의 안정성이 유지됩니다.
    

### Visitor Pattern의 단점

- **새로운 Element 추가 시 어려움:** 객체 구조에 새로운 `Element` 타입이 추가되면 모든 기존 `Visitor` 인터페이스 및 구현 클래스들을 수정해야 합니다. 이는 OCP를 위반할 수 있습니다.
    
- **캡슐화 위반 가능성:** `Visitor`가 `Element`의 내부 상태에 접근해야 할 경우, `Element`의 캡슐화를 깨뜨릴 수 있습니다. `Visitor`는 `Element`의 공개된 인터페이스에만 의존해야 합니다.
    
- **복잡성 증가:** 간단한 객체 구조나 몇 가지 연산만 필요한 경우에는 오히려 패턴의 복잡성만 증가시킬 수 있습니다.
    

### 언제 Visitor Pattern을 사용해야 하는가?

- **객체 구조가 안정적이고 자주 변경되지 않을 때.** (새로운 Element 추가가 드물 때)
    
- **객체 구조에 대해 다양한 종류의 연산을 수행해야 할 때.**
    
- **각 연산이 객체 구조의 여러 클래스에 걸쳐 분산되어 구현될 필요가 있을 때.**
    
- **객체 구조를 변경하지 않고 새로운 연산을 추가하고 싶을 때.**
    

### 예시 (간략한 Pseudocode)

```
// Visitor 인터페이스
interface ShapeVisitor {
    void visit(Circle circle);
    void visit(Square square);
    void visit(Triangle triangle);
}

// Element 인터페이스
interface Shape {
    void accept(ShapeVisitor visitor);
}

// Concrete Element 1: Circle
class Circle implements Shape {
    double radius;

    // ... constructor, getters ...

    @Override
    void accept(ShapeVisitor visitor) {
        visitor.visit(this); // Visitor의 visit(Circle) 메서드 호출
    }
}

// Concrete Element 2: Square
class Square implements Shape {
    double side;

    // ... constructor, getters ...

    @Override
    void accept(ShapeVisitor visitor) {
        visitor.visit(this); // Visitor의 visit(Square) 메서드 호출
    }
}

// Concrete Visitor 1: AreaCalculator (면적 계산)
class AreaCalculator implements ShapeVisitor {
    @Override
    void visit(Circle circle) {
        // 원의 면적 계산 로직
        double area = Math.PI * circle.radius * circle.radius;
        System.out.println("Circle Area: " + area);
    }

    @Override
    void visit(Square square) {
        // 사각형의 면적 계산 로직
        double area = square.side * square.side;
        System.out.println("Square Area: " + area);
    }

    @Override
    void visit(Triangle triangle) {
        // 삼각형의 면적 계산 로직
        // ...
        System.out.println("Triangle Area: ... ");
    }
}

// Concrete Visitor 2: PerimeterCalculator (둘레 계산)
class PerimeterCalculator implements ShapeVisitor {
    @Override
    void visit(Circle circle) {
        // 원의 둘레 계산 로직
        double perimeter = 2 * Math.PI * circle.radius;
        System.out.println("Circle Perimeter: " + perimeter);
    }

    @Override
    void visit(Square square) {
        // 사각형의 둘레 계산 로직
        double perimeter = 4 * square.side;
        System.out.println("Square Perimeter: " + perimeter);
    }

    @Override
    void visit(Triangle triangle) {
        // 삼각형의 둘레 계산 로직
        // ...
        System.out.println("Triangle Perimeter: ... ");
    }
}

// 사용 예시
public class Main {
    public static void main(String[] args) {
        Shape circle = new Circle(5.0);
        Shape square = new Square(4.0);
        // Shape triangle = new Triangle(3.0, 4.0, 5.0);

        AreaCalculator areaCalc = new AreaCalculator();
        PerimeterCalculator perimeterCalc = new PerimeterCalculator();

        circle.accept(areaCalc);     // Circle Area: 78.53...
        square.accept(areaCalc);    // Square Area: 16.0
        // triangle.accept(areaCalc);

        circle.accept(perimeterCalc); // Circle Perimeter: 31.41...
        square.accept(perimeterCalc); // Square Perimeter: 16.0
        // triangle.accept(perimeterCalc);
    }
}
```

이 예시에서 `Circle`, `Square`, `Triangle`과 같은 도형 객체들은 `AreaCalculator`와 `PerimeterCalculator`라는 두 가지 종류의 Visitor에 의해 방문되어 면적과 둘레를 계산하는 연산을 수행합니다. 만약 새로운 연산(예: 도형 그리기)을 추가하고 싶다면, `ShapeVisitor` 인터페이스를 구현하는 새로운 Visitor 클래스만 만들면 되고, 기존 `Shape` 클래스들을 수정할 필요가 없습니다.

---