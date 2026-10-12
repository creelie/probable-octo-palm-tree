#!/usr/bin/env bash
# check.sh -- regenerate the Lean data from data/certificates.txt, compare it with the
# committed files, build the Lean project and print the axioms of the main theorem.
#
# Tools: python3, elan (Lean 4 and Lake). The first build downloads Mathlib and its compiled
# files (lake exe cache get).
set -euo pipefail

HERE="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT="$(cd "$HERE/../.." && pwd)"
WORK="$(mktemp -d)"
trap 'rm -rf "$WORK"' EXIT
export PATH="$HOME/.elan/bin:$PATH"

pass() { printf '  [ok] %s\n' "$1"; }
fail() { printf '  [FAIL] %s\n' "$1"; exit 1; }

echo "== Python: the Lean data are generated from data/certificates.txt"
python3 -B "$HERE/generate/make_lean.py" "$ROOT/data/certificates.txt" "$WORK" > /dev/null
for d in Cert Data; do
  diff -r -q "$WORK/OdaLean/$d" "$HERE/OdaLean/$d" > /dev/null \
    && pass "make_lean.py reproduces OdaLean/$d" || fail "OdaLean/$d differs"
done
cmp -s "$WORK/OdaLean/Poly.lean" "$HERE/OdaLean/Poly.lean" \
  && pass "make_poly.py reproduces OdaLean/Poly.lean" || fail "OdaLean/Poly.lean differs"

echo "== Lean: no sorry, no native_decide, no new axioms"
if grep -rn -E '\b(sorry|admit|native_decide)\b|^axiom |implemented_by|@\[extern' \
    "$HERE/OdaLean" "$HERE/OdaLean.lean"; then
  fail "forbidden construct in verification/lean"
fi
pass "no sorry, admit, native_decide, axiom, implemented_by or extern"

echo "== Lean: build (every certificate checked by the kernel)"
cd "$HERE"
lake exe cache get > /dev/null
lake build 2>&1 | tail -1
lake env lean OdaLean/Axioms.lean | tee "$WORK/axioms.txt"
grep -qx "'Oda.certificate' depends on axioms: \[propext, Classical.choice, Quot.sound\]" \
  "$WORK/axioms.txt" && pass "Oda.certificate uses only propext, Classical.choice, Quot.sound" \
  || fail "unexpected axioms"

echo "ALL LEAN CHECKS PASSED"
