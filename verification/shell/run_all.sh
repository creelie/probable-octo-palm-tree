#!/usr/bin/env bash
# run_all.sh -- run every check of data/certificates.txt.
#
#   verification/shell/run_all.sh            Python, C, C++ and Julia checkers, the table of
#                                            the paper, the random sanity test, then the
#                                            Lean build
#   verification/shell/run_all.sh --no-lean  the same without the Lean build
#
# Tools: python3, a C compiler (cc), a C++17 compiler (c++), julia, and for the Lean part
# elan (see verification/lean/check.sh).
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
CERTS="$ROOT/data/certificates.txt"
WORK="$(mktemp -d)"
trap 'rm -rf "$WORK"' EXIT
LEAN=1
[ "${1:-}" = "--no-lean" ] && LEAN=0

echo "== Python (exact polynomial arithmetic)"
python3 -B "$ROOT/verification/python/check_certificates.py" "$CERTS"

echo "== C (evaluation on the grid {0,1,2,3}^k)"
cc -O2 -std=c99 -o "$WORK/check7c" "$ROOT/verification/c/check7.c"
"$WORK/check7c" "$CERTS"

echo "== C++ (evaluation on the grid {-2,-1,1,3}^k)"
c++ -O2 -std=c++17 -o "$WORK/check7cpp" "$ROOT/verification/cpp/check7.cpp"
"$WORK/check7cpp" "$CERTS"

echo "== Julia (exact polynomial arithmetic)"
julia --startup-file=no "$ROOT/verification/julia/check7.jl" "$CERTS"

echo "== Appendix A of the paper agrees with the table"
python3 -B "$ROOT/verification/python/paper_table.py" "$CERTS" "$ROOT/paper/main.tex"

echo "== Python sanity test (random polytopes, not part of the proof)"
python3 -B "$ROOT/verification/python/idp_sample.py" 1 2026

if [ "$LEAN" = 1 ]; then
  echo "== Lean 4 kernel"
  "$ROOT/verification/lean/check.sh"
fi

echo "ALL CHECKS PASSED"
