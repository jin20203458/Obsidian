```
PS C:\Users\user\Desktop\ErrorTest\ErrorTest> clang-tidy .\*.cpp -checks="-*,defence-style-extern-function-declaration" -- -std=c++17 -w


clang-tidy .\*.cpp -checks="-*,defence-style-extern-function-declaration" -config="{CheckOptions: [{key: defence-style-extern-function-declaration.RequireExplicitExtern, value: true}]}" -- -std=c++17 -w
```