#!/usr/bin/env bash
# AN_KR 빌드: XeLaTeX 를 두세 번 돌려(차례·참조) PDF 만 출력 디렉터리로 복사한다.
#   ./build.sh                                  전체 → out/AN_KR.pdf
#   ./build.sh -o DIR                           전체 → DIR/AN_KR.pdf
#   ./build.sh --chapter sections/05_btag.tex   한 장만 시험 빌드 → out/chapter_05_btag.pdf
#   ./build.sh --keep                           임시 빌드 디렉터리를 남기고 경로를 알린다
# 필요: xelatex(TeX Live 또는 MacTeX), kotex, Noto Serif/Sans/Sans Mono CJK KR 글꼴.
# 출력 디렉터리 out/ 은 git 이 무시한다(docs/AN_KR/.gitignore). PDF 는 커밋하지 않는다.
set -euo pipefail

here="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
outdir="$here/out"
chapter=""
keep=0
while [ $# -gt 0 ]; do
  case "$1" in
    -o) [ $# -ge 2 ] || { echo "build.sh: -o 다음에 디렉터리가 필요하다" >&2; exit 2; }
        outdir="$2"; shift 2 ;;
    --chapter) [ $# -ge 2 ] || { echo "build.sh: --chapter 다음에 파일이 필요하다" >&2; exit 2; }
        chapter="$2"; shift 2 ;;
    --keep) keep=1; shift ;;
    -h|--help) sed -n '2,8p' "$0"; exit 0 ;;
    *) echo "build.sh: 모르는 인자: $1" >&2; exit 2 ;;
  esac
done

command -v xelatex >/dev/null 2>&1 || { echo "build.sh: xelatex 이 없다" >&2; exit 3; }
if command -v fc-list >/dev/null 2>&1; then
  # 파이프에 grep -q 를 쓰면 pipefail 아래에서 SIGPIPE(141)로 거짓 실패가 난다. 변수에 담아 비교한다.
  fonts="$(fc-list : family 2>/dev/null || true)"
  for f in "Noto Serif CJK KR" "Noto Sans CJK KR" "Noto Sans Mono CJK KR"; do
    case "$fonts" in
      *"$f"*) ;;
      *) echo "build.sh: 글꼴 '$f' 이 없다(Noto CJK KR 설치 필요)" >&2; exit 3 ;;
    esac
  done
fi

tmpbase="${TMPDIR:-/tmp}"; tmpbase="${tmpbase%/}"
work="$(mktemp -d "$tmpbase/ankr_build.XXXXXX")"
cleanup() {
  if [ "$keep" -eq 1 ]; then
    echo "build.sh: 빌드 디렉터리를 남겼다: $work"
  else
    case "$work" in
      "$tmpbase"/ankr_build.??????) rm -rf -- "$work" ;;
    esac
  fi
}
trap cleanup EXIT

cp "$here/preamble.tex" "$here/main.tex" "$work/"
mkdir "$work/sections"
cp "$here"/sections/*.tex "$work/sections/"

if [ -n "$chapter" ]; then
  name="$(basename "$chapter" .tex)"
  [ -f "$here/sections/$name.tex" ] || { echo "build.sh: 장 파일이 없다: sections/$name.tex" >&2; exit 2; }
  appx=""
  case "$name" in [A-Z]_*) appx='\appendix' ;; esac
  cat > "$work/chapter.tex" <<EOF
\\documentclass[11pt,a4paper,oneside,openany]{report}
\\usepackage[a4paper,top=24mm,bottom=24mm,left=23mm,right=23mm,headheight=14pt]{geometry}
\\input{preamble}
\\newcommand{\\ANKRversion}{AN\\_KR 장 시험 빌드}
\\newcommand{\\ANKRdate}{시험}
\\begin{document}
$appx
\\input{sections/$name}
\\end{document}
EOF
  job=chapter
  pdfname="chapter_$name.pdf"
else
  job=main
  pdfname="AN_KR.pdf"
fi

run_tex() {
  ( cd "$work" && xelatex -interaction=nonstopmode -halt-on-error -file-line-error "$job.tex" >/dev/null 2>&1 ) || {
    echo "build.sh: xelatex 실패($1 번째). 로그의 오류 부분:" >&2
    grep -n -A4 -E '^(!|.*:[0-9]+: )' "$work/$job.log" | head -40 >&2 || true
    keep=1
    exit 4
  }
}
run_tex 1
run_tex 2
if grep -q -E 'Rerun to get|Label\(s\) may have changed' "$work/$job.log"; then run_tex 3; fi

mkdir -p "$outdir"
cp "$work/$job.pdf" "$outdir/$pdfname"

undef=$(grep -c -E 'Reference .* undefined|Citation .* undefined' "$work/$job.log" || true)
overfull=$(grep -c -E '^Overfull \\hbox \(([1-9][0-9]+|[1-9][0-9]*\.[0-9]+)pt' "$work/$job.log" || true)
if command -v pdfinfo >/dev/null 2>&1; then
  pages="$(pdfinfo "$work/$job.pdf" 2>/dev/null | awk '/^Pages:/{print $2" 쪽"}')"
else
  pages="쪽 수 모름(pdfinfo 없음)"
fi
echo "build.sh: 완료 → $outdir/$pdfname ($pages; 정의 안 된 참조 $undef 개; 1pt 이상 Overfull hbox $overfull 개)"
