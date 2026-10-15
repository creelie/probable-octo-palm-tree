**Smooth lattice 3-polytopes with seven facets have the integer decomposition property**, by Deep Bhattacharjee (corresponding author) and Ushashi Bhattacharya.

**Theorem.** Every smooth lattice polytope in R^3 with seven facets has the integer decomposition property: for every k ≥ 1, every lattice point of kP is a sum of k lattice points of P. With the case of at most six facets (Robins 2024), Oda's conjecture holds for every smooth lattice 3-polytope with at most seven facets. The whole three-dimensional case was proved earlier by M. Michałek (AI-generated proof, 25 September 2026) and by J. Curtis and G. Liu (arXiv:2610.17270), so this theorem is a special case of theirs, proved by a different method. The conjecture in dimension four and higher stays open.

| Step | Content |
|---|---|
| Lemma 3 | a lattice 3-polytope has the property as soon as 2P ∩ Z^3 = (P ∩ Z^3) + (P ∩ Z^3) |
| Propositions 6, 7, 8 | three criteria on the normal fan: slicing S(u), projection P(m), projection with one upper facet Q(m) |
| Proposition 9 | the smooth complete projective fans with seven rays form 625 families with rays affine in integer parameters |
| Proposition 10, Appendix A | a certificate for every family, valid for all parameter values |

The reduction, the criteria and the classification are proved by hand in the paper. The certificates of all 625 families are checked by Lean 4 with Mathlib: the kernel evaluates the decision procedures (no `native_decide`), soundness theorems turn them into the hypotheses of Propositions 6 to 8 at every parameter value, and the proof uses only the axioms propext, Classical.choice and Quot.sound. Four independent programs repeat the check (Python and Julia with exact polynomials, C and C++ by evaluation on two grids).

Version 2.0.0 adds the second author, four schematic figures, the declarations of the journal and the preprint folder.

Files:
- `OdaConjectureSevenFacets_final2.pdf`: the paper
- `OdaConjectureSevenFacets_final2-tex.zip`: LaTeX source (`main.tex`, the journal style `e-jc.sty`, figures as PNG)
- `OdaConjectureSevenFacets_final2-arxiv.tar.gz`: the same source, ready for arXiv

The repository also holds `preprintOdaConjectureSevenFacets/OdaConjectureSevenFacets.pdf` and `MasterOdaConjectureSevenFacets_masterzipfinal2.zip`, which holds every file of the repository. Run `verification/shell/run_all.sh` to repeat the checks and `build_paper.sh` to rebuild the files above.

Zenodo: release v2.0.1 [10.5281/zenodo.23387693](https://doi.org/10.5281/zenodo.23387693); all versions [10.5281/zenodo.23329590](https://doi.org/10.5281/zenodo.23329590).
