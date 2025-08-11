
```
Checks: '-*,defence-style-char-array-string-size'
CheckOptions:
  - key: defence-style-char-array-string-size.RequireExactSize
    value: 'false'   # 기본 false. true면 정확히 len+1 강제
  - key: defence-style-char-array-string-size.OfferFixIt
    value: 'true'    # [N]에서 N이 정수 리터럴일 때 자동 교체 제안
```
