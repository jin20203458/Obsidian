 dd - convert and copy a file

|옵션|의미|설명|
|---|---|---|
|`if=`|input file|읽어올 입력 파일을 지정 (default는 표준 입력)|
|`of=`|output file|저장할 출력 파일을 지정 (default는 표준 출력)|
|`bs=`|block size|읽기/쓰기 작업에 사용할 블록 크기 (ex: `bs=1M`은 1메가 단위로 읽고 씀)|
|`count=`|복사할 블록 수|지정한 블록 수만큼 데이터 복사|
|`skip=`|입력 파일에서 건너뛸 블록 수|읽을 때 처음 몇 블록을 무시|
|`seek=`|출력 파일에서 건너뛸 블록 수|쓸 때 처음 몇 블록을 건너뛰고 작성|
|`conv=`|변환 작업|데이터 변환 옵션 지정 (예: `notrunc`, `sync`, `noerror`, `ucase`, `lcase` 등)|
