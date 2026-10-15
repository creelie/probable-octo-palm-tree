#!/usr/bin/env bash
# build_paper.sh -- build the paper and the release files.
#
#   dist/OdaConjectureSevenFacets_final2.pdf             the paper
#   dist/OdaConjectureSevenFacets_final2-tex.zip         LaTeX source (main.tex, the journal
#                                                        style e-jc.sty) with the figures as PNG
#   dist/OdaConjectureSevenFacets_final2-arxiv.tar.gz    the same source as an arXiv tarball
#   preprintOdaConjectureSevenFacets/OdaConjectureSevenFacets.pdf
#                                                        the paper (the only file there)
#   MasterOdaConjectureSevenFacets_masterzipfinal2.zip   every tracked file with dist/ and the
#                                                        preprint folder
#
# Needs pdflatex, git, zip and tar. The figures in paper/figures/ are made from the TikZ
# sources in figures/ by figures/make_figures.sh.
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$ROOT"
for t in pdflatex zip tar git; do
  command -v "$t" >/dev/null || { echo "missing tool: $t" >&2; exit 2; }
done

NAME=OdaConjectureSevenFacets
PRE=preprint$NAME
MASTER=Master${NAME}_masterzipfinal2.zip

latex() { pdflatex -interaction=nonstopmode -halt-on-error "$@" > /dev/null; }

echo "== paper"
(
  cd paper
  rm -f main.aux main.out main.log
  for run in 1 2 3 4; do
    latex main.tex
    grep -q "Rerun to get" main.log || break
  done
)
if grep -E "Overfull|Underfull|undefined|Rerun to get" paper/main.log; then
  echo "build_paper.sh: fix the warnings above" >&2
  exit 1
fi
rm -f paper/main.aux paper/main.out paper/main.log

rm -rf dist
mkdir -p dist "$PRE"
cp paper/main.pdf "dist/${NAME}_final2.pdf"
cp paper/main.pdf "$PRE/$NAME.pdf"

STAGE="$(mktemp -d)"
trap 'rm -rf "$STAGE"' EXIT

echo "== tex zip and arXiv tarball"
SRC="$STAGE/${NAME}_final2-tex"
mkdir -p "$SRC/figures"
cp paper/main.tex paper/e-jc.sty "$SRC/"
cp paper/figures/*.png "$SRC/figures/"
( cd "$SRC" && latex main.tex && latex main.tex && rm -f main.aux main.log main.out main.pdf )
( cd "$STAGE" && zip -qrX "${NAME}_final2-tex.zip" "${NAME}_final2-tex" )
mv "$STAGE/${NAME}_final2-tex.zip" dist/
tar -czf "dist/${NAME}_final2-arxiv.tar.gz" -C "$SRC" main.tex e-jc.sty figures

echo "== $MASTER"
mkdir -p "$STAGE/master/Master$NAME"
git ls-files -z | grep -zvE "^(dist/|$PRE/|$MASTER\$)" \
  | while IFS= read -r -d '' f; do
      [ -e "$f" ] || continue
      mkdir -p "$STAGE/master/Master$NAME/$(dirname "$f")"
      cp -p "$f" "$STAGE/master/Master$NAME/$f"
    done
mkdir -p "$STAGE/master/Master$NAME/dist" "$STAGE/master/Master$NAME/$PRE"
cp dist/* "$STAGE/master/Master$NAME/dist/"
cp "$PRE/$NAME.pdf" "$STAGE/master/Master$NAME/$PRE/"
rm -f "$MASTER"
( cd "$STAGE/master" && zip -qrX "$ROOT/$MASTER" "Master$NAME" )

echo "built: $(ls dist | tr '\n' ' ')$PRE/$NAME.pdf $MASTER"
