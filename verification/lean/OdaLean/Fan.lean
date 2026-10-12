import OdaLean.Vec

/-!
# Fans with seven rays

An `IFan` is a list of integer rays and a list of maximal cones, each cone a triple of ray
indices. An `SFan` has rays whose coordinates are affine functions of the parameters; its
value at `p` is `SFan.ev X p`. The families of the classification and their blowups are built
here exactly as in the paper.
-/

namespace Oda

/-- Ray indices of a maximal cone. -/
abbrev Cone := Nat × Nat × Nat

/-- A fan: rays, and maximal cones given by ray indices. -/
structure IFan where
  rays : List V3
  cones : List Cone

namespace IFan

/-- The number of rays. -/
def n (F : IFan) : Nat := F.rays.length

/-- Ray `i` (the zero vector when `i ≥ n`). -/
def ray (F : IFan) (i : Nat) : V3 := F.rays.getD i V3.zero

end IFan

/-- `i` is a ray of the cone `t`. -/
def InCone (i : Nat) (t : Cone) : Prop := i = t.1 ∨ i = t.2.1 ∨ i = t.2.2

/-- A fan whose rays are affine in the parameters. -/
structure SFan where
  rays : List A3
  cones : List Cone

namespace SFan

def n (X : SFan) : Nat := X.rays.length

def ray (X : SFan) (i : Nat) : A3 := X.rays.getD i A3.zero

/-- The fan at the parameter values `p`. -/
def ev (X : SFan) (p : Par) : IFan := ⟨X.rays.map (A3.ev · p), X.cones⟩

theorem ev_n (X : SFan) (p : Par) : (X.ev p).n = X.n := by
  simp [ev, IFan.n, n]

theorem getD_map {α β : Type} (f : α → β) (l : List α) (d : α) (i : Nat) :
    (l.map f).getD i (f d) = f (l.getD i d) := by
  induction l generalizing i with
  | nil => simp
  | cons a l => cases i <;> simp

theorem ev_ray (X : SFan) (p : Par) (i : Nat) : (X.ev p).ray i = (X.ray i).ev p := by
  simp only [ev, IFan.ray, ray]
  rw [← A3.ev_zero p, getD_map]

theorem ev_cones (X : SFan) (p : Par) : (X.ev p).cones = X.cones := rfl

end SFan

/-! ### Blowups -/

/-- The triple `(a, b, c)` in increasing order. -/
def sort3 (a b c : Nat) : Cone :=
  if a ≤ b then
    if b ≤ c then (a, b, c) else if a ≤ c then (a, c, b) else (c, a, b)
  else
    if a ≤ c then (b, a, c) else if b ≤ c then (b, c, a) else (c, b, a)

/-- Membership of `i` in the cone `t`, as a `Bool`. -/
def inC (i : Nat) (t : Cone) : Bool := i == t.1 || i == t.2.1 || i == t.2.2

/-- The ray of `t` other than `i` and `j`. -/
def third (t : Cone) (i j : Nat) : Nat :=
  if t.1 != i && t.1 != j then t.1 else if t.2.1 != i && t.2.1 != j then t.2.1 else t.2.2

/-- A torus-invariant centre: a fixed point (a maximal cone) or an invariant curve
(a two-dimensional cone). -/
inductive Center where
  | pt (i j k : Nat)
  | cv (i j : Nat)
deriving DecidableEq

namespace SFan

/-- Pairs `(i, j)` with `i < j < n`. -/
def pairsBelow : Nat → List (Nat × Nat)
  | 0 => []
  | n + 1 => pairsBelow n ++ (List.range n).map fun i => (i, n)

/-- The two-dimensional cones, as pairs `i < j`. -/
def faces (X : SFan) : List (Nat × Nat) :=
  (pairsBelow X.n).filter fun f => X.cones.any fun t => inC f.1 t && inC f.2 t

/-- The centres of the blowups: every maximal cone and every two-dimensional cone. -/
def centers (X : SFan) : List Center :=
  X.cones.map (fun t => Center.pt t.1 t.2.1 t.2.2) ++ X.faces.map fun f => Center.cv f.1 f.2

/-- The blowup at a fixed point: the star subdivision of the cone `(i, j, k)` at
`ρ_i + ρ_j + ρ_k`. -/
def blowPt (X : SFan) (i j k : Nat) : SFan :=
  let n := X.n
  ⟨X.rays ++ [((X.ray i).add (X.ray j)).add (X.ray k)],
   X.cones.filter (fun t => t != (i, j, k)) ++ [sort3 n j k, sort3 i n k, sort3 i j n]⟩

/-- The blowup along an invariant curve: the star subdivision of the cone `(i, j)` at
`ρ_i + ρ_j`. -/
def blowCv (X : SFan) (i j : Nat) : SFan :=
  let n := X.n
  ⟨X.rays ++ [(X.ray i).add (X.ray j)],
   X.cones.flatMap fun t =>
     if inC i t && inC j t then [sort3 n j (third t i j), sort3 i n (third t i j)] else [t]⟩

def blow (X : SFan) : Center → SFan
  | .pt i j k => X.blowPt i j k
  | .cv i j => X.blowCv i j

end SFan

/-! ### The families -/

/-- The affine function `p i`. -/
def var (i : Fin 6) : Aff :=
  ⟨0, if i = 0 then 1 else 0, if i = 1 then 1 else 0, if i = 2 then 1 else 0,
   if i = 3 then 1 else 0, if i = 4 then 1 else 0, if i = 5 then 1 else 0⟩

/-- The constant vector `(x, y, z)`. -/
def cv3 (x y z : Int) : A3 := A3.const ⟨x, y, z⟩

/-- `ℙ¹`-bundles over `ℙ²`: the fan `Z_a`, `a = p 0`. -/
def famZ : SFan :=
  ⟨[cv3 1 0 0, cv3 0 1 0, ⟨Aff.const (-1), Aff.const (-1), var 0⟩, cv3 0 0 1, cv3 0 0 (-1)],
   [(0, 1, 3), (0, 1, 4), (0, 2, 3), (0, 2, 4), (1, 2, 3), (1, 2, 4)]⟩

/-- `ℙ²`-bundles over `ℙ¹`: the fan `W_{a,b}`, `a = p 0`, `b = p 1`. -/
def famW : SFan :=
  ⟨[cv3 1 0 0, cv3 0 1 0, cv3 (-1) (-1) 0, cv3 0 0 1, ⟨var 0, var 1, Aff.const (-1)⟩],
   [(0, 1, 3), (0, 1, 4), (0, 2, 3), (0, 2, 4), (1, 2, 3), (1, 2, 4)]⟩

/-- The `ℙ¹`-bundle over the surface with rays `base` (in cyclic order): ray `i` is
`(base i, p (i + 1))`, followed by `(0, 0, 1)` and `(0, 0, -1)`. -/
def bundle (base : List (Aff × Aff)) : SFan :=
  let k := base.length
  ⟨(base.zipIdx.map fun ⟨v, i⟩ => (⟨v.1, v.2, if h : i + 1 < 6 then var ⟨i + 1, h⟩ else
      Aff.const 0⟩ : A3)) ++ [cv3 0 0 1, cv3 0 0 (-1)],
   (List.range k).flatMap fun i =>
     [sort3 i ((i + 1) % k) k, sort3 i ((i + 1) % k) (k + 1)]⟩

/-- The Hirzebruch surface `F_a`, `a = p 0`. -/
def baseF : List (Aff × Aff) :=
  [(Aff.const 1, Aff.const 0), (Aff.const 0, Aff.const 1), (Aff.const (-1), var 0),
   (Aff.const 0, Aff.const (-1))]

/-- `F_a` blown up at the fixed point between rays `j` and `j + 1`. -/
def baseG (j : Nat) : List (Aff × Aff) :=
  let v := baseF.getD j (Aff.const 0, Aff.const 0)
  let w := baseF.getD ((j + 1) % 4) (Aff.const 0, Aff.const 0)
  baseF.take (j + 1) ++ [(Aff.add v.1 w.1, Aff.add v.2 w.2)] ++ baseF.drop (j + 1)

/-- `ℙ¹`-bundles over Hirzebruch surfaces. -/
def famF : SFan := bundle baseF

/-- `ℙ¹`-bundles over the surfaces with five rays. -/
def famG (j : Nat) : SFan := bundle (baseG j)

/-- The minimal projective family (a) of Fujino and Payne, `a = p 0`. -/
def famC : SFan :=
  ⟨[cv3 1 0 0, cv3 0 1 0, cv3 0 0 1, ⟨Aff.const 0, Aff.const (-1), var 0⟩, cv3 0 0 (-1),
    cv3 0 1 (-1), cv3 (-1) 2 (-1)],
   [(0, 1, 2), (0, 2, 3), (0, 3, 4), (0, 4, 5), (0, 5, 6), (0, 1, 6), (1, 2, 6), (2, 3, 6),
    (3, 4, 6), (4, 5, 6)]⟩

end Oda
