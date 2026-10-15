# Target journal: The Electronic Journal of Combinatorics

| Item | Details |
|---|---|
| Journal | The Electronic Journal of Combinatorics (E-JC) |
| Publisher | The Electronic Journal of Combinatorics (independent, diamond open access: no fees for authors or readers) |
| ISSN | 1077-8926 |
| Homepage | https://www.combinatorics.org |
| Submission | Web system at https://www.combinatorics.org/ojs/index.php/eljc/about/submissions (register, then "Make a submission"); the initial submission is the PDF `dist/OdaConjectureSevenFacets_final2.pdf` |
| Source files | Only at the final version: the paper must use the journal's style file `e-jc.sty` in the manner of `ejc-sample.tex`, with all LaTeX, including the bibliography, in one file, uploaded as supplementary files. `paper/main.tex` already does this, and `dist/OdaConjectureSevenFacets_final2-tex.zip` holds `main.tex` with `e-jc.sty` and the figures as PNG |

## Scope fit

E-JC publishes papers in all branches of discrete mathematics, and lattice
polytopes and the integer decomposition property are a standing topic there.
Two papers the article builds on appeared in E-JC: S. Robins, *Integer
decomposition property of polytopes*, Electron. J. Combin. 31(1) (2024), #P1.1,
whose theorem on smooth fans with at most n + 3 rays settles the case of at
most six facets in dimension three, and C. Haase, B. Nill, A. Paffenholz and
F. Santos, *Lattice points in Minkowski sums*, Electron. J. Combin. 15 (2008),
#N11, whose Proposition 2.1 is used in the slicing criterion. The article settles the next case of Oda's conjecture
in dimension three, smooth 3-polytopes with seven facets.

MSC 2020: primary 52B20; secondary 14M25, 68V15.

## Length and format

- 16 pages in `e-jc.sty`, with four schematic figures,, built with no overfull or underfull boxes, as the
  final-version rules require; the journal sets no page limit.
- `main.tex` begins with `\documentclass[12pt]{article}` and
  `\usepackage[amsmath]{e-jc}`, uses only ASCII characters, and sets the
  `\dateline`, `\MSC` and `\Copyright` fields of the sample (CC BY 4.0). The
  submission and acceptance dates in `\dateline` are `TBD`; fill them in when
  the editors give them.
- Title without footnotes. Two authors, Deep Bhattacharjee (corresponding
  author) and Ushashi Bhattacharya, marked with `\authornote{1}` and
  `\authornote{2}`, with the affiliations, e-mail addresses and ORCID iDs in
  the matching `\authortext` lines, as the sample does for several authors.
- The abstract has 45 words, states the result, and has no citations,
  equation numbers or undefined notation, as the guidelines ask.
  At submission the system asks for an HTML copy of the abstract: paste the
  abstract with `$...$` for the mathematics and no `\emph`.
- The references are in the source (`thebibliography`) with `\doi` and
  `\arxiv` links, as the sample recommends.
- Supplementary material: the certificate table is printed in Appendix A, and
  the programs and the Lean project are cited through the Zenodo record of
  the repository in the section "Data availability". The master zip
  `MasterOdaConjectureSevenFacets_masterzipfinal2.zip` can be uploaded as a
  supplementary file.
- Declarations at the end: "Data availability" (who computed what, the
  Zenodo DOI of the Lean formalization and the link to the PDF in
  `preprintOdaConjectureSevenFacets/`), "Acknowledgements" (use of AI),
  "Competing interests: None" and "Funding: None".

## Submission checklist of the journal

At submission the authors check off these items:

- the paper is not published or under consideration elsewhere;
- it is written in the authors' own words, with due credit to all techniques
  and results taken from other sources;
- the authors agree to the final typesetting with `e-jc.sty`;
- the authors have read the journal's Artificial Intelligence Policy and
  adhered to it;
- no author has submitted another paper to E-JC within the last 3 calendar
  months, including papers that were rejected. Check this before submitting.

## Policy on AI assistance

- E-JC submission checklist: "If you obtained any proof using AI, do not
  proceed unless you rewrote the proof in your own words and can vouch for
  every step. Papers mostly written by AI without thorough checking by humans
  will not be accepted." The editors may also use AI for a final check before
  publication. (https://www.combinatorics.org/ojs/index.php/eljc/about/submissions,
  read on 2026-10-12.)
- The proofs of this paper were developed with Claude Code (Anthropic), and
  the paper says so in its sections "Data availability" and
  "Acknowledgements". Under the policy above,
  the authors have to go through every proof, rewrite it in their own words
  where needed and be able to vouch for every step before submitting.
- What is checked by machine: the certificate of every one of the 625
  families (Proposition 10) is checked by the Lean 4 kernel, with no
  `native_decide` and only the axioms `propext`, `Classical.choice` and
  `Quot.sound`, and by four independent programs (Python, Julia, C, C++).
  What is proved by hand in the paper: the reduction (Lemma 3), the three
  criteria (Propositions 6, 7 and 8), the classification of the fans
  (Proposition 9, from the cited classifications of Miyake-Oda-Nagaya and
  Kleinschmidt) and the 25 families of Lemma 11 and Section 5.
