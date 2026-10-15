#!/usr/bin/env bash
# make_figures.sh -- compile the TikZ figures and write them as PNG (200 dpi) to paper/figures/.
# Needs pdflatex (with tikz-3dplot) and pdftoppm.
set -euo pipefail
HERE="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
OUT="$HERE/../paper/figures"
for t in pdflatex pdftoppm; do
  command -v "$t" >/dev/null || { echo "missing tool: $t" >&2; exit 2; }
done
mkdir -p "$OUT"
WORK="$(mktemp -d)"
trap 'rm -rf "$WORK"' EXIT
for src in "$HERE"/fig_*.tex; do
  name="$(basename "$src" .tex)"
  cp "$src" "$WORK/"
  ( cd "$WORK" && pdflatex -interaction=nonstopmode -halt-on-error "$name.tex" > /dev/null ) \
    || { echo "make_figures.sh: $name.tex does not compile" >&2; exit 1; }
  pdftoppm -r 200 -png -singlefile "$WORK/$name.pdf" "$OUT/$name"
  echo "$OUT/$name.png"
done
