#!/usr/bin/env sh
# ────────────────────────────────────────────────────────────────
# envSetupCorrection.sh
#  • POSIX sh 호환
#  • 반드시 ". ./envSetupCorrection.sh" 으로 소싱해서 쓰세요.
# ────────────────────────────────────────────────────────────────

# (1) 로컬 venv 활성화 (프로젝트 루트의 ./corrlib)
if [ -f "./corrlib/bin/activate" ]; then
  . ./corrlib/bin/activate
else
  echo "Warning: ./corrlib/bin/activate not found" >&2
fi

# (2) correction CLI 확인
if ! command -v correction >/dev/null 2>&1; then
  echo "Error: 'correction' CLI not found in PATH" >&2
  # 소싱 상태라면 return, 아니면 exit
  return 1 2>/dev/null || exit 1
fi

# (3) 빌드/링크 플래그 뽑기
CORRECTION_CXXFLAGS=$(correction config --cflags)
LDFLAGS_RAW=$(correction config --ldflags)

# (4) 실제 lib 디렉토리
#     sed 패턴: -L<경로> 중 <경로>만 추출
CORRECTION_LIB_DIR=$(echo "$LDFLAGS_RAW" \
  | sed -e 's/.*-L\([^ ]*\).*/\1/')

# (5) rpath 포함 LDFLAGS
CORRECTION_LDFLAGS="$LDFLAGS_RAW -Wl,-rpath,$CORRECTION_LIB_DIR"

# (6) export
export CORRECTION_CXXFLAGS \
       CORRECTION_LDFLAGS \
       CORRECTION_LIB_DIR

# (7) 런타임 라이브러리 경로 설정
UNAME_S=$(uname -s)
if [ "$UNAME_S" = "Darwin" ]; then
  export DYLD_LIBRARY_PATH="$CORRECTION_LIB_DIR:${DYLD_LIBRARY_PATH:-}"
else
  export LD_LIBRARY_PATH="$CORRECTION_LIB_DIR:${LD_LIBRARY_PATH:-}"
fi

# (8) 결과 표시
echo "=== correctionlib environment loaded ==="
echo " CXXFLAGS    = $CORRECTION_CXXFLAGS"
echo " LDFLAGS     = $CORRECTION_LDFLAGS"
echo " LIB_DIR     = $CORRECTION_LIB_DIR"
echo " RUNTIME_LD  = ${DYLD_LIBRARY_PATH:-$LD_LIBRARY_PATH}"
