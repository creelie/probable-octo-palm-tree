import OdaLean.Fan

/-!
# The three certificates

These are the hypotheses of Propositions 6, 7 and 8 of the paper, which give `2P ∩ ℤ³ = (P ∩ ℤ³) +
(P ∩ ℤ³)` for every lattice polytope `P` with normal fan `F`:

* `SCond F u` (slicing): `u ≠ 0`; in every maximal cone, the coordinates of `u` lie in
  `{-1, 0, 1}`; and the graph on the rays in which `ρ` and `σ` are joined when
  `|det(u, ρ, σ)| ≥ 2` contains no cycle of length four.
* `PCond F m` (projection): every height `⟨ρ, m⟩` lies in `{-1, 0, 1}`, and for every
  two-dimensional cone `{a, b}` with both heights `1` and every two-dimensional cone `{c, d}`
  with both heights `-1`, `(ρ_a - ρ_b) × (ρ_c - ρ_d) = e m` with `e ∈ {-1, 0, 1}`.
* `P2Cond F m` (projection with one upper facet): exactly one height `⟨ρ, m⟩` is negative,
  it is `-1`, and every other height lies in `{0, 1, 2}`.
-/

namespace Oda

/-- `x ∈ {-1, 0, 1}`. -/
def Unit3 (x : Int) : Prop := x = -1 ∨ x = 0 ∨ x = 1

/-- `{i, j}` is a two-dimensional cone of `F`. -/
def IFan.Face (F : IFan) (i j : Nat) : Prop :=
  i ≠ j ∧ ∃ t ∈ F.cones, InCone i t ∧ InCone j t

/-- The edge relation of the graph `G_u`. -/
def Big (u a b : V3) : Prop := 2 ≤ |det3 u a b|

def SCond (F : IFan) (u : V3) : Prop :=
  u ≠ V3.zero ∧
  (∀ t ∈ F.cones, ∃ a b c : Int, Unit3 a ∧ Unit3 b ∧ Unit3 c ∧
      (((F.ray t.1).smul a).add ((F.ray t.2.1).smul b)).add ((F.ray t.2.2).smul c) = u) ∧
  ∀ i j k l, i < F.n → j < F.n → k < F.n → l < F.n → i ≠ k → j ≠ l →
    ¬ (Big u (F.ray i) (F.ray j) ∧ Big u (F.ray j) (F.ray k) ∧ Big u (F.ray k) (F.ray l) ∧
        Big u (F.ray l) (F.ray i))

def PCond (F : IFan) (m : V3) : Prop :=
  m ≠ V3.zero ∧ (∀ i < F.n, Unit3 ((F.ray i).dot m)) ∧
  ∀ a b c d, a < F.n → b < F.n → c < F.n → d < F.n → F.Face a b → F.Face c d →
    (F.ray a).dot m = 1 → (F.ray b).dot m = 1 → (F.ray c).dot m = -1 → (F.ray d).dot m = -1 →
    ∃ e, Unit3 e ∧ ((F.ray a).sub (F.ray b)).cross ((F.ray c).sub (F.ray d)) = m.smul e

def P2Cond (F : IFan) (m : V3) : Prop :=
  ∃ j < F.n, (F.ray j).dot m = -1 ∧
    ∀ i < F.n, i ≠ j → 0 ≤ (F.ray i).dot m ∧ (F.ray i).dot m ≤ 2

/-- One of the three certificates holds for `F`. -/
def HasCert (F : IFan) : Prop := ∃ v : V3, SCond F v ∨ PCond F v ∨ P2Cond F v

end Oda
