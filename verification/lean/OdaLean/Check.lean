import OdaLean.Defs

/-!
# Checking a certificate on a family of fans

Each checker returns a `Bool`, computed on the coefficients of the affine rays, so that a
`true` result gives the certificate at every value of the parameters (`OdaLean.Sound`).
-/

namespace Oda

/-- `allN n f` holds when `f i` holds for every `i < n`. -/
def allN : Nat → (Nat → Bool) → Bool
  | 0, _ => true
  | n + 1, f => allN n f && f n

/-- `anyN n f` holds when `f i` holds for some `i < n`. -/
def anyN : Nat → (Nat → Bool) → Bool
  | 0, _ => false
  | n + 1, f => anyN n f || f n

def isUnit (x : Int) : Bool := x == -1 || x == 0 || x == 1

/-- The 27 triples with coordinates in `{-1, 0, 1}`. -/
def units3 : List (Int × Int × Int) :=
  [-1, 0, 1].flatMap fun a => [-1, 0, 1].flatMap fun b => [-1, 0, 1].map fun c => (a, b, c)

/-- `u` is a combination of the rays of the cone `t` with coefficients in `{-1, 0, 1}`. -/
def coneOK (X : SFan) (u : V3) (t : Cone) : Bool :=
  units3.any fun w => isUnit w.1 && isUnit w.2.1 && isUnit w.2.2 &&
    ((((X.ray t.1).smul w.1).add ((X.ray t.2.1).smul w.2.1)).add
      ((X.ray t.2.2).smul w.2.2)).isConstEq u

/-- `q` is a constant in `{-1, 0, 1}`. -/
def smallQ (q : Quad) : Bool := q.isConst && isUnit q.c

/-- The pairs `i < j` for which `det(u, ρ_i, ρ_j)` is not a constant in `{-1, 0, 1}`. -/
def bigPairs (X : SFan) (u : V3) : List (Nat × Nat) :=
  (SFan.pairsBelow X.n).filter fun f => !smallQ (A3.detQ u (X.ray f.1) (X.ray f.2))

def adj (E : List (Nat × Nat)) (i j : Nat) : Bool := E.contains (i, j) || E.contains (j, i)

/-- The graph with edge list `E` on `0, …, n - 1` has no cycle `i, j, k, l` with `i < k` and
`j < l`. -/
def c4free (E : List (Nat × Nat)) (n : Nat) : Bool :=
  allN n fun i => allN n fun k => !decide (i < k) || allN n fun j => allN n fun l =>
    !decide (j < l) || !(adj E i j && adj E j k && adj E k l && adj E l i)

def checkS (X : SFan) (u : V3) : Bool :=
  u != V3.zero && X.cones.all (coneOK X u) && c4free (bigPairs X u) X.n

/-- The height `⟨ρ_i, m⟩` (its constant term). -/
def hgt (X : SFan) (m : V3) (i : Nat) : Int := (A3.dotC m (X.ray i)).c

def isFace (X : SFan) (i j : Nat) : Bool := i != j && X.cones.any fun t => inC i t && inC j t

/-- `(ρ_a - ρ_b) × (ρ_c - ρ_d)` is a constant in `{0, m, -m}`. -/
def crossOK (X : SFan) (m : V3) (a b c d : Nat) : Bool :=
  let q := A3.crossQ ((X.ray a).sub (X.ray b)) ((X.ray c).sub (X.ray d))
  let w : V3 := ⟨q.x.c, q.y.c, q.z.c⟩
  q.x.isConst && q.y.isConst && q.z.isConst && (w == V3.zero || w == m || w == m.smul (-1))

def checkP (X : SFan) (m : V3) : Bool :=
  m != V3.zero && X.rays.all (fun r => (A3.dotC m r).isConst && isUnit (A3.dotC m r).c) &&
  allN X.n fun a => allN X.n fun b => allN X.n fun c => allN X.n fun d =>
    !(hgt X m a == 1 && hgt X m b == 1 && hgt X m c == -1 && hgt X m d == -1 &&
      isFace X a b && isFace X c d) || crossOK X m a b c d

def checkP2 (X : SFan) (m : V3) : Bool :=
  X.rays.all (fun r => (A3.dotC m r).isConst) &&
  anyN X.n fun j => hgt X m j == -1 && allN X.n fun i =>
    i == j || hgt X m i == 0 || hgt X m i == 1 || hgt X m i == 2

/-- A certificate: the slicing direction `u`, or the projection direction `m`. -/
inductive Cert where
  | S (u : V3)
  | P (m : V3)
  | P2 (m : V3)

def Cert.check (X : SFan) : Cert → Bool
  | .S u => checkS X u
  | .P m => checkP X m
  | .P2 m => checkP2 X m

/-- A row `(c₁, c₂, cert)` of the table for the double blowups of `B`. -/
def rowOK2 (B : SFan) (r : Center × Center × Cert) : Bool := r.2.2.check ((B.blow r.1).blow r.2.1)

/-- A row `(c, cert)` of the table for the blowups of `B`. -/
def rowOK1 (B : SFan) (r : Center × Cert) : Bool := r.2.check (B.blow r.1)

/-- Every double blowup of `B` has a row in `rows`. -/
def covered2 (B : SFan) (rows : List (Center × Center × Cert)) : Bool :=
  B.centers.all fun c₁ => (B.blow c₁).centers.all fun c₂ =>
    rows.any fun r => r.1 == c₁ && r.2.1 == c₂

/-- Every blowup of `B` has a row in `rows`. -/
def covered1 (B : SFan) (rows : List (Center × Cert)) : Bool :=
  B.centers.all fun c => rows.any fun r => r.1 == c

end Oda
