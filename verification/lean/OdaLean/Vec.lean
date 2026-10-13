import OdaLean.Poly

/-!
# Vectors in `ℤ³` and vectors of affine functions

`V3` is an integer vector; `A3` is a vector whose coordinates are affine functions of the
parameters, and `A3.ev p` is its value at `p`. The symbolic determinant `A3.detQ u a b` and
cross product `A3.crossQ a b` have quadratic coordinates and evaluate to `det3` and
`V3.cross`.
-/

namespace Oda

/-- An integer vector. -/
structure V3 where
  x : Int
  y : Int
  z : Int
deriving DecidableEq

namespace V3

def zero : V3 := ⟨0, 0, 0⟩

def add (a b : V3) : V3 := ⟨a.x + b.x, a.y + b.y, a.z + b.z⟩

def sub (a b : V3) : V3 := ⟨a.x - b.x, a.y - b.y, a.z - b.z⟩

def smul (k : Int) (a : V3) : V3 := ⟨k * a.x, k * a.y, k * a.z⟩

def dot (a b : V3) : Int := a.x * b.x + a.y * b.y + a.z * b.z

def cross (a b : V3) : V3 :=
  ⟨a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x⟩

end V3

/-- The determinant of the matrix with rows `a`, `b`, `c`. -/
def det3 (a b c : V3) : Int := a.dot (b.cross c)

theorem det3_swap (u a b : V3) : det3 u b a = -det3 u a b := by
  simp only [det3, V3.dot, V3.cross]; ring

/-- A vector of affine functions of the parameters. -/
structure A3 where
  x : Aff
  y : Aff
  z : Aff
deriving DecidableEq

/-- Three quadratic polynomials. -/
structure Q3 where
  x : Quad
  y : Quad
  z : Quad

namespace A3

def ev (v : A3) (p : Par) : V3 := ⟨v.x.ev p, v.y.ev p, v.z.ev p⟩

/-- The constant vector `u`. -/
def const (u : V3) : A3 := ⟨Aff.const u.x, Aff.const u.y, Aff.const u.z⟩

def zero : A3 := const V3.zero

def add (a b : A3) : A3 := ⟨Aff.add a.x b.x, Aff.add a.y b.y, Aff.add a.z b.z⟩

def sub (a b : A3) : A3 := ⟨Aff.sub a.x b.x, Aff.sub a.y b.y, Aff.sub a.z b.z⟩

def smul (k : Int) (a : A3) : A3 := ⟨Aff.smul k a.x, Aff.smul k a.y, Aff.smul k a.z⟩

/-- `⟨u, a⟩` for a constant vector `u`. -/
def dotC (u : V3) (a : A3) : Aff :=
  Aff.add (Aff.smul u.x a.x) (Aff.add (Aff.smul u.y a.y) (Aff.smul u.z a.z))

/-- `u × a` for a constant vector `u`. -/
def crossC (u : V3) (a : A3) : A3 :=
  ⟨Aff.sub (Aff.smul u.y a.z) (Aff.smul u.z a.y),
   Aff.sub (Aff.smul u.z a.x) (Aff.smul u.x a.z),
   Aff.sub (Aff.smul u.x a.y) (Aff.smul u.y a.x)⟩

/-- `⟨a, b⟩`. -/
def dotQ (a b : A3) : Quad :=
  Quad.add (Aff.mul a.x b.x) (Quad.add (Aff.mul a.y b.y) (Aff.mul a.z b.z))

/-- `det(u, a, b) = ⟨u × a, b⟩`. -/
def detQ (u : V3) (a b : A3) : Quad := dotQ (crossC u a) b

/-- `a × b`. -/
def crossQ (a b : A3) : Q3 :=
  ⟨Quad.sub (Aff.mul a.y b.z) (Aff.mul a.z b.y),
   Quad.sub (Aff.mul a.z b.x) (Aff.mul a.x b.z),
   Quad.sub (Aff.mul a.x b.y) (Aff.mul a.y b.x)⟩

/-- `v` is the constant vector `u`. -/
def isConstEq (v : A3) (u : V3) : Bool :=
  v.x == Aff.const u.x && v.y == Aff.const u.y && v.z == Aff.const u.z

theorem ev_zero (p : Par) : zero.ev p = V3.zero := by
  simp [zero, const, ev, Aff.ev_const, V3.zero]

theorem ev_add (a b : A3) (p : Par) : (add a b).ev p = (a.ev p).add (b.ev p) := by
  simp [add, ev, V3.add, Aff.ev_add]

theorem ev_sub (a b : A3) (p : Par) : (sub a b).ev p = (a.ev p).sub (b.ev p) := by
  simp [sub, ev, V3.sub, Aff.ev_sub]

theorem ev_smul (k : Int) (a : A3) (p : Par) : (smul k a).ev p = (a.ev p).smul k := by
  simp [smul, ev, V3.smul, Aff.ev_smul]

theorem ev_dotC (u : V3) (a : A3) (p : Par) : (dotC u a).ev p = (a.ev p).dot u := by
  simp only [dotC, Aff.ev_add, Aff.ev_smul, ev, V3.dot]; ring

theorem ev_detQ (u : V3) (a b : A3) (p : Par) :
    (detQ u a b).ev p = det3 u (a.ev p) (b.ev p) := by
  simp only [detQ, dotQ, crossC, Quad.ev_add, Aff.ev_mul, Aff.ev_sub, Aff.ev_smul, ev, det3,
    V3.dot, V3.cross]
  ring

theorem ev_crossQ (a b : A3) (p : Par) :
    (⟨(crossQ a b).x.ev p, (crossQ a b).y.ev p, (crossQ a b).z.ev p⟩ : V3) =
      (a.ev p).cross (b.ev p) := by
  simp [crossQ, Quad.ev_sub, Aff.ev_mul, ev, V3.cross]

theorem ev_of_isConstEq {v : A3} {u : V3} (h : v.isConstEq u = true) (p : Par) :
    v.ev p = u := by
  simp only [isConstEq, Bool.and_eq_true, beq_iff_eq] at h
  obtain ⟨⟨hx, hy⟩, hz⟩ := h
  simp [ev, hx, hy, hz, Aff.ev_const]

end A3

end Oda
