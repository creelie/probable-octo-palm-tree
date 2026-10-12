**Smooth lattice 3-polytopes with seven facets have the integer decomposition property**, by Deep Bhattacharjee.

**Theorem.** Every smooth lattice polytope in R^3 with seven facets has the integer decomposition property: for every k ≥ 1, every lattice point of kP is a sum of k lattice points of P. With the case of at most six facets (Robins 2024), every smooth lattice 3-polytope with at most seven facets has the property. This is the next case of Oda's conjecture in dimension three.

| Step | Content |
|---|---|
| Lemma 3 | a lattice 3-polytope has the property as soon as 2P ∩ Z^3 = (P ∩ Z^3) + (P ∩ Z^3) |
| Propositions 6, 7, 8 | three criteria on the normal fan: slicing S(u), projection P(m), projection with one upper facet Q(m) |
| Proposition 9 | the smooth complete projective fans with seven rays form 625 families with rays affine in integer parameters |
| Proposition 10, Appendix A | a certificate for every family, valid for all parameter values |

The criteria and the classification are proved by hand. The certificates of all 625 families are checked by four independent programs (Python and Julia with exact polynomials, C and C++ by evaluation on two grids) and by Lean 4 with Mathlib: `Oda.certificate` is proved with `decide +kernel` and soundness theorems, without `native_decide`, using only the axioms propext, Classical.choice and Quot.sound.

Files:
- `dist/oda-seven-facets.pdf`: the paper
- `dist/oda-seven-facets-tex.zip`: LaTeX source (`main.tex` and the journal style `e-jc.sty`)
- `dist/oda-seven-facets-arxiv.tar.gz`: the same source, ready for arXiv
- `dist/MasterOdaSevenFacetsZip.zip`: every file of the repository with the three files above

Run `verification/shell/run_all.sh` to repeat the checks and `build_paper.sh` to rebuild the files above.
