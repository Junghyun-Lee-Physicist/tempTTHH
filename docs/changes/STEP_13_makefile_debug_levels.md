# STEP 13 — Makefile 빌드 레벨 (평소 -g 항상 / DEBUG=1 ASan 옵션)

- 날짜: 2026-06-15
- 요구: 기존 debug용 Makefile(ASan, -O0, -g, RPATH)을 참조해, 현 Makefile에
  옵션으로 디버그 강도를 토글. 필수 디버깅(-g)은 평소에도, 무거운 로그/ASan만
  옵션으로. README에 설명 추가.
- 변경: `Makefile`, `README.md`
- 백업: `docs/backup_20260611/Makefile.step13`

## 무엇을 / 왜
참조한 debug Makefile은 항상 `-O0 -g -fsanitize=address -fno-omit-frame-pointer`
+ 링크 ASan + `$ORIGIN/lib` RPATH 였다. 전부 평소에 켜면 느리고(2~3x) 무거우므로,
**필수(-g)는 항상 / 무거운 것(ASan, -O0)만 옵션** 으로 분리:

| 레벨 | 플래그 |
|---|---|
| 평소(default) | `-c -O2 -g -Wall -fPIC` |
| `DEBUG=1` | `-c -O0 -g -Wall -fPIC -Wextra -fsanitize=address -fno-omit-frame-pointer` + 링크 `-fsanitize=address` |

- **-g 를 평소에도** 포함 — 지난 segfault 추적 때 Makefile에 -g 없어 gdb 줄번호가
  안 나왔던 문제 해결. -g 는 런타임 성능 영향 없음(바이너리 크기만 증가).
- `DEBUG=1` 의 ASan 은 heap/stack overflow·use-after-free·invalid free 를
  런타임에 파일:줄로 잡는다.
- `$ORIGIN/lib`(Linux) / `@loader_path/lib`(macOS) RPATH 삽입 — 실행파일이
  `./lib` 의 libToolsForAnalysis/libEventShape 를 자동 탐색. (Linux 의 $ORIGIN 은
  make 변수 평가에서 사라지므로 app 링크 **레시피에 리터럴**로 전달.)
- `VERBOSE=1` 은 기존대로 빌드 로그 상세.

## 구현 노트
- `ifdef DEBUG` 로 `OPT_FLAGS`(-O0/-O2), `DEBUG_CXX`, `DEBUG_LINK` 분기.
- ASan 은 컴파일·링크 **양쪽**에 필요 → shared lib·app 링크 모두 `$(DEBUG_LINK)`.
- RPATH 변수화 함정: `APP_RPATH := ...$$ORIGIN...` 는 make 가 `$ORIGIN`→빈문자열로
  평가. 그래서 Linux 는 변수에 안 담고 `$(if $(APP_RPATH),,-Wl,-rpath,'$$ORIGIN/lib')`
  로 레시피에서 직접 전달 (probe 로 `$ORIGIN/lib` 리터럴 전달 확인).

## 검증
- `make`/`make DEBUG=1` 변수 평가: `[Build Level]` 로그 + COMPILE_FLAGS/DEBUG_LINK
  값이 의도대로 분기 (probe.mk 로 확인).
- app 링크 레시피 RPATH 전개: `-Wl,-rpath,$ORIGIN/lib` 리터럴 확인.
- 평소 COMPILE_FLAGS = `-c -O2 -g -Wall -fPIC`, DEBUG=1 = `-c -O0 -g -Wall -fPIC
  -Wextra -fsanitize=address -fno-omit-frame-pointer`.

## 로컬 검증
```bash
make clean && make -j4              # 평소
make clean && make DEBUG=1 -j4      # ASan
make VERBOSE=1                       # 빌드 명령 출력
```

## 롤백
`cp docs/backup_20260611/Makefile.step13 Makefile`
