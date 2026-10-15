# Smooth lattice 3-polytopes with seven facets have the integer decomposition property

Paper, certificates, verification programs and Lean 4 formalization by

- **Deep Bhattacharjee** (corresponding author), Formerly, Electro-Gravitational
  Space Propulsion Laboratory (EGSPL), Bhubaneswar, Odisha 751030, India
  (itsdeep@live.com, d.bhattacharjee@erl-forschung.de;
  ORCID [0000-0003-0466-750X](https://orcid.org/0000-0003-0466-750X));
- **Ushashi Bhattacharya**, Formerly, National Taiwan University, Taipei
  106319, Taiwan (rai.bhattacharya.in@gmail.com;
  ORCID [0009-0002-2254-3914](https://orcid.org/0009-0002-2254-3914)).

The paper: [preprintOdaConjectureSevenFacets/OdaConjectureSevenFacets.pdf](preprintOdaConjectureSevenFacets/OdaConjectureSevenFacets.pdf).
Everything in one file: [MasterOdaConjectureSevenFacets_masterzipfinal2.zip](https://github.com/creelie/probable-octo-palm-tree/raw/main/MasterOdaConjectureSevenFacets_masterzipfinal2.zip).

**Theorem.** Every smooth lattice polytope in R^3 with seven facets has the
integer decomposition property: for every integer k ≥ 1, every lattice point
of kP is a sum of k lattice points of P. With the known case of at most six
facets (Robins, Electron. J. Combin. 31 (2024)), every smooth lattice
3-polytope with at most seven facets has the property.

Oda asked whether every smooth lattice polytope has the integer decomposition
property (Oda's conjecture). In dimension three it is open; it was known for
centrally symmetric polytopes, for at most six facets and for pseudo-prisms.

## Method

- **Reduction** (Lemma 3). A lattice 3-polytope P has the property as soon as
  2P ∩ Z^3 = (P ∩ Z^3) + (P ∩ Z^3).
- **Three criteria on the normal fan** (Propositions 6, 7, 8), proved by hand.
  A slicing criterion S(u): u has coordinates in {-1, 0, 1} in every maximal
  cone, and the graph of pairs of rays with |det(u, ρ_i, ρ_j)| ≥ 2 has no
  4-cycle. Two projection criteria P(m) and Q(m) on the heights ⟨ρ_i, m⟩.
- **Classification** (Proposition 9). A smooth complete fan in R^3 with seven
  rays and projective toric variety is, up to GL_3(Z), one of 625 families
  with rays affine in integer parameters: double blowups of the two fans with
  five rays (600), blowups of P^1-bundles over Hirzebruch surfaces (20),
  P^1-bundles over the toric surfaces with five rays (4), and the family (a)
  of Fujino and Payne (1).
- **Certificates** (Proposition 10, Appendix A). Every family has a vector
  that satisfies one of the three criteria for all values of its parameters;
  25 families are done by hand, and all 625 are checked by the Lean 4 kernel
  and by four independent programs.

## Files

| Path | Content |
|---|---|
| `paper/main.tex`, `paper/main.pdf` | the paper (Electronic Journal of Combinatorics style `paper/e-jc.sty`, 16 pages, two authors, MSC primary 52B20, secondary 14M25, 68V15) |
| `paper/figures/`, `figures/` | the four schematic figures as PNG, and their TikZ sources with `figures/make_figures.sh` |
| `preprintOdaConjectureSevenFacets/` | the paper as a PDF, the only file in the folder |
| `dist/` | `OdaConjectureSevenFacets_final2.pdf`, the LaTeX source zip `OdaConjectureSevenFacets_final2-tex.zip` (figures as PNG) and the arXiv tarball |
| `MasterOdaConjectureSevenFacets_masterzipfinal2.zip` | every file of the repository with `dist/` and the preprint folder |
| `journal/JOURNAL.md` | target journal (The Electronic Journal of Combinatorics), format and AI policy |
| `data/certificates.txt` | the certificate of each of the 625 families, one line per family |
| `verification/python/` | `fans7.py` (the families and the three criteria as polynomial identities), `check_certificates.py` (the checker), `find_certificates.py` (the search that produced the table), `paper_table.py` (Appendix A of the paper from the table), `idp_sample.py` (a direct test of 2P ∩ Z^3 = (P ∩ Z^3) + (P ∩ Z^3) on random polytopes, not part of the proof) |
| `verification/julia/`, `verification/c/`, `verification/cpp/` | independent checkers: Julia with exact polynomials, C and C++ by evaluation on the grids {0,1,2,3}^k and {-2,-1,1,3}^k |
| `verification/lean/` | Lean 4 + Mathlib project; `Oda.certificate` proves, in the kernel, that every family has a certificate for every value of its parameters |
| `verification/shell/run_all.sh` | runs every check |
| `build_paper.sh` | builds the paper, the files in `dist/`, the preprint folder and the master zip |
| `.github/workflows/release.yml` | runs every check and publishes a release (Zenodo mints its DOI) |

## Running the checks

```
verification/shell/run_all.sh            # the four checkers, Appendix A against the table,
                                         # the random test, then the Lean build
verification/shell/run_all.sh --no-lean  # the same without Lean
```

Needs Python 3, a C compiler, a C++17 compiler, Julia, and elan for Lean
(`verification/lean/lean-toolchain`: leanprover/lean4:v4.34.1; the first build
downloads Mathlib and its compiled files).

## What Lean checks

`Oda.certificate` (in `verification/lean/OdaLean/Main.lean`) states that for
every centre c_1 of Z_a or W_{a,b}, every centre c_2 of the subdivided fan and
every parameter vector p, the fan obtained by the two star subdivisions at p
satisfies `HasCert`, and likewise for the blowups of F_{a,p}, for G^j_{a,p}
and for C_a. `HasCert F` says that some integer vector satisfies `SCond`,
`PCond` or `P2Cond` (`OdaLean/Defs.lean`), the hypotheses of Propositions 6, 7
and 8 of the paper. Boolean checkers compute on the polynomial coefficients of
the rays and are evaluated by `decide +kernel`; the soundness theorems in
`OdaLean/Sound.lean` turn a checker returning true into the hypothesis at every
parameter value. `native_decide` is not used, and `#print axioms Oda.certificate`
gives propext, Classical.choice and Quot.sound. The build takes about two
minutes on four cores.

The reduction (Lemma 3), the three criteria (Propositions 6 to 8) and the
classification (Proposition 9, from the classifications of Miyake-Oda-Nagaya
and Kleinschmidt) are proved by hand in the paper; Lean checks the
certificates.

## Archive

Zenodo, all versions: [10.5281/zenodo.23329590](https://doi.org/10.5281/zenodo.23329590);
release v1.0.0: [10.5281/zenodo.23329591](https://doi.org/10.5281/zenodo.23329591);
release v1.0.1: [10.5281/zenodo.23329690](https://doi.org/10.5281/zenodo.23329690).
Release v2.0.0, with both authors and the figures, gets its DOI when it is published.

## License

MIT, see `LICENSE`.
