

- **템플릿 파라미터를 '여러 개' 동적으로 받을 수 있게 해주는 문법**
    
- 템플릿 정의부에서
    
    ```cpp
    template <typename... Args>`
    ```

    처럼 "..."을 붙이면  
    Args가 **"파라미터 팩(parameter pack)"으로**  
    여러 타입을 한꺼번에 전달받을 수 있음