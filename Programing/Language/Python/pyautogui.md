
`pyautogui`는 파이썬으로 GUI 자동화를 할 수 있는 라이브러리입니다. 주로 마우스와 키보드를 자동으로 제어하고 화면을 캡처하는 기능을 제공합니다. 이를 통해 사용자는 반복적인 작업을 자동화하거나, 테스트용 스크립트를 작성하는 등의 용도로 사용할 수 있습니다.

주요 기능은 다음과 같습니다:

### 1. **마우스 제어**

- `pyautogui.moveTo(x, y)`: 지정된 좌표 `(x, y)`로 마우스를 이동합니다.
    
- `pyautogui.move(x, y)`: 현재 위치에서 상대적으로 마우스를 이동합니다.
    
- `pyautogui.click(x, y)`: 지정된 좌표에서 마우스 클릭을 수행합니다.
    
- `pyautogui.doubleClick(x, y)`: 지정된 좌표에서 더블 클릭을 합니다.
    
- `pyautogui.rightClick(x, y)`: 지정된 좌표에서 오른쪽 클릭을 합니다.
    

### 2. **키보드 제어**

- `pyautogui.write('text')`: 지정된 텍스트를 입력합니다.
    
- `pyautogui.press('key')`: 키보드에서 특정 키를 한 번 누릅니다.
    
- `pyautogui.hotkey('ctrl', 'c')`: 여러 키를 동시에 눌러 조합 키를 사용합니다 (예: Ctrl+C).
    

### 3. **스크린샷**

- `pyautogui.screenshot()`: 화면의 스크린샷을 찍습니다.
    
- `pyautogui.locateOnScreen('image.png')`: 화면에서 특정 이미지 파일을 찾아 그 위치를 반환합니다.
    

### 4. **화면 크기 및 위치**

- `pyautogui.size()`: 화면 해상도 (가로, 세로)를 반환합니다.
    
- `pyautogui.position()`: 현재 마우스의 위치를 반환합니다.
    

### 5. **시간 제어**

- `pyautogui.sleep(seconds)`: 지정된 시간(초) 동안 멈춥니다.
    
- `pyautogui.PAUSE`: 각 함수 호출 후 자동으로 대기하는 시간(초)을 설정할 수 있습니다.