import OdaLean.Check
import Mathlib.Tactic.NormNum

/-!
# Soundness of the checkers

`Cert.check_sound`: if `c.check X = true`, then for every value `p` of the parameters the fan
`X.ev p` satisfies one of `SCond`, `PCond`, `P2Cond`.
-/

namespace Oda

theorem allN_eq_true {n : Nat} {f : Nat → Bool} : allN n f = true ↔ ∀ i < n, f i = true := by
  induction n with
  | zero => simp [allN]
  | succ n ih =>
    simp only [allN, Bool.and_eq_true, ih]
    constructor
    · rintro ⟨h1, h2⟩ i hi
      rcases Nat.lt_succ_iff_lt_or_eq.1 hi with h | h
      · exact h1 i h
      · subst h; exact h2
    · intro h
      exact ⟨fun i hi => h i (Nat.lt_succ_of_lt hi), h n (Nat.lt_succ_self n)⟩

theorem anyN_eq_true {n : Nat} {f : Nat → Bool} : anyN n f = true ↔ ∃ i < n, f i = true := by
  induction n with
  | zero => simp [anyN]
  | succ n ih =>
    simp only [anyN, Bool.or_eq_true, ih]
    constructor
    · rintro (⟨i, hi, h⟩ | h)
      · exact ⟨i, Nat.lt_succ_of_lt hi, h⟩
      · exact ⟨n, Nat.lt_succ_self n, h⟩
    · rintro ⟨i, hi, h⟩
      rcases Nat.lt_succ_iff_lt_or_eq.1 hi with h' | h'
      · exact Or.inl ⟨i, h', h⟩
      · subst h'; exact Or.inr h

theorem unit_of_isUnit {x : Int} (h : isUnit x = true) : Unit3 x := by
  simp only [isUnit, Bool.or_eq_true, beq_iff_eq] at h
  unfold Unit3
  omega

theorem SFan.ray_mem {X : SFan} {i : Nat} (h : i < X.n) : X.ray i ∈ X.rays := by
  unfold SFan.ray
  rw [List.getD_eq_getElem?_getD, List.getElem?_eq_getElem h, Option.getD_some]
  exact List.getElem_mem h

theorem inC_iff {i : Nat} {t : Cone} : inC i t = true ↔ InCone i t := by
  simp [inC, InCone, or_assoc]

/-! ### Slicing -/

theorem coneOK_sound {X : SFan} {u : V3} {t : Cone} (h : coneOK X u t = true) (p : Par) :
    ∃ a b c : Int, Unit3 a ∧ Unit3 b ∧ Unit3 c ∧
      ((((X.ev p).ray t.1).smul a).add (((X.ev p).ray t.2.1).smul b)).add
        (((X.ev p).ray t.2.2).smul c) = u := by
  simp only [coneOK, List.any_eq_true, Bool.and_eq_true] at h
  obtain ⟨w, -, ⟨⟨h1, h2⟩, h3⟩, h4⟩ := h
  refine ⟨w.1, w.2.1, w.2.2, unit_of_isUnit h1, unit_of_isUnit h2, unit_of_isUnit h3, ?_⟩
  have e := A3.ev_of_isConstEq h4 p
  simp only [A3.ev_add, A3.ev_smul] at e
  simpa only [SFan.ev_ray] using e

theorem det3_self (u a : V3) : det3 u a a = 0 := by
  simp only [det3, V3.dot, V3.cross]; ring

theorem big_symm {u a b : V3} (h : Big u a b) : Big u b a := by
  unfold Big at *
  rw [det3_swap, abs_neg]; exact h

theorem not_big_of_small {u : V3} {a b : A3} (h : smallQ (A3.detQ u a b) = true) (p : Par) :
    ¬ Big u (a.ev p) (b.ev p) := by
  simp only [smallQ, Bool.and_eq_true] at h
  have e := Quad.ev_of_isConst h.1 p
  rw [A3.ev_detQ] at e
  intro hb
  unfold Big at hb
  rw [e] at hb
  rcases unit_of_isUnit h.2 with h2 | h2 | h2 <;> rw [h2] at hb <;> norm_num at hb

theorem mem_pairsBelow {n i j : Nat} : (i, j) ∈ SFan.pairsBelow n ↔ i < j ∧ j < n := by
  induction n with
  | zero => simp [SFan.pairsBelow]
  | succ n ih =>
    simp only [SFan.pairsBelow, List.mem_append, ih, List.mem_map, List.mem_range,
      Prod.mk.injEq]
    constructor
    · rintro (⟨h1, h2⟩ | ⟨a, h1, rfl, rfl⟩)
      · exact ⟨h1, Nat.lt_succ_of_lt h2⟩
      · exact ⟨h1, Nat.lt_succ_self _⟩
    · rintro ⟨h1, h2⟩
      rcases Nat.lt_succ_iff_lt_or_eq.1 h2 with h | h
      · exact Or.inl ⟨h1, h⟩
      · exact Or.inr ⟨i, h ▸ h1, rfl, h.symm⟩

theorem adj_comm (E : List (Nat × Nat)) (i j : Nat) : adj E i j = adj E j i := by
  simp only [adj, Bool.or_comm]

theorem mem_bigPairs {X : SFan} {u : V3} {p : Par} {i j : Nat} (hij : i < j) (hj : j < X.n)
    (hb : Big u ((X.ray i).ev p) ((X.ray j).ev p)) : (i, j) ∈ bigPairs X u := by
  simp only [bigPairs, List.mem_filter, mem_pairsBelow]
  refine ⟨⟨hij, hj⟩, ?_⟩
  cases hs : smallQ (A3.detQ u (X.ray (i, j).1) (X.ray (i, j).2))
  · rfl
  · exact absurd hb (not_big_of_small hs p)

theorem adj_of_big {X : SFan} {u : V3} {p : Par} {i j : Nat} (hi : i < X.n) (hj : j < X.n)
    (hb : Big u ((X.ray i).ev p) ((X.ray j).ev p)) : adj (bigPairs X u) i j = true := by
  rcases Nat.lt_trichotomy i j with h | h | h
  · simp [adj, mem_bigPairs h hj hb]
  · subst h
    unfold Big at hb
    rw [det3_self] at hb
    norm_num at hb
  · simp [adj, mem_bigPairs h hi (big_symm hb)]

theorem c4free_sound {E : List (Nat × Nat)} {n : Nat} (h : c4free E n = true) :
    ∀ i j k l, i < n → j < n → k < n → l < n → i ≠ k → j ≠ l →
      ¬ (adj E i j = true ∧ adj E j k = true ∧ adj E k l = true ∧ adj E l i = true) := by
  have key : ∀ i j k l, i < n → j < n → k < n → l < n → i < k → j < l →
      ¬ (adj E i j = true ∧ adj E j k = true ∧ adj E k l = true ∧ adj E l i = true) := by
    intro i j k l hi hj hk hl hik hjl ⟨h1, h2, h3, h4⟩
    have e1 := allN_eq_true.1 (allN_eq_true.1 h i hi) k hk
    simp only [Bool.or_eq_true, Bool.not_eq_true', decide_eq_false_iff_not] at e1
    rcases e1 with e1 | e1
    · exact e1 hik
    have e2 := allN_eq_true.1 (allN_eq_true.1 e1 j hj) l hl
    simp only [Bool.or_eq_true, Bool.not_eq_true', decide_eq_false_iff_not] at e2
    rcases e2 with e2 | e2
    · exact e2 hjl
    · simp [h1, h2, h3, h4] at e2
  intro i j k l hi hj hk hl hik hjl ⟨h1, h2, h3, h4⟩
  rcases Nat.lt_or_gt_of_ne hik with hik | hik <;> rcases Nat.lt_or_gt_of_ne hjl with hjl | hjl
  · exact key i j k l hi hj hk hl hik hjl ⟨h1, h2, h3, h4⟩
  · rw [adj_comm] at h1 h2 h3 h4
    exact key i l k j hi hl hk hj hik hjl ⟨h4, h3, h2, h1⟩
  · rw [adj_comm] at h1 h2 h3 h4
    exact key k j i l hk hj hi hl hik hjl ⟨h2, h1, h4, h3⟩
  · exact key k l i j hk hl hi hj hik hjl ⟨h3, h4, h1, h2⟩

theorem checkS_sound {X : SFan} {u : V3} (h : checkS X u = true) (p : Par) :
    SCond (X.ev p) u := by
  simp only [checkS, Bool.and_eq_true, bne_iff_ne, ne_eq, List.all_eq_true] at h
  obtain ⟨⟨h0, hc⟩, hg⟩ := h
  refine ⟨h0, fun t ht => coneOK_sound (hc t ht) p, ?_⟩
  intro i j k l hi hj hk hl hik hjl ⟨h1, h2, h3, h4⟩
  rw [SFan.ev_n] at hi hj hk hl
  simp only [SFan.ev_ray] at h1 h2 h3 h4
  exact c4free_sound hg i j k l hi hj hk hl hik hjl
    ⟨adj_of_big hi hj h1, adj_of_big hj hk h2, adj_of_big hk hl h3, adj_of_big hl hi h4⟩

/-! ### Projection -/

theorem ray_dot {X : SFan} {m : V3} {i : Nat} (hc : (A3.dotC m (X.ray i)).isConst = true)
    (p : Par) : ((X.ev p).ray i).dot m = hgt X m i := by
  rw [SFan.ev_ray, ← A3.ev_dotC, Aff.ev_of_isConst hc]
  rfl

theorem isFace_of_face {X : SFan} {p : Par} {a b : Nat} (h : (X.ev p).Face a b) :
    isFace X a b = true := by
  obtain ⟨hab, t, ht, ha, hb⟩ := h
  simp only [isFace, Bool.and_eq_true, bne_iff_ne, ne_eq, List.any_eq_true]
  exact ⟨hab, t, ht, inC_iff.2 ha, inC_iff.2 hb⟩

theorem crossOK_sound {X : SFan} {m : V3} {a b c d : Nat} (h : crossOK X m a b c d = true)
    (p : Par) : ∃ e, Unit3 e ∧
      (((X.ev p).ray a).sub ((X.ev p).ray b)).cross (((X.ev p).ray c).sub ((X.ev p).ray d)) =
        m.smul e := by
  simp only [crossOK, Bool.and_eq_true, Bool.or_eq_true, beq_iff_eq] at h
  obtain ⟨⟨⟨hx, hy⟩, hz⟩, hw⟩ := h
  have key := A3.ev_crossQ ((X.ray a).sub (X.ray b)) ((X.ray c).sub (X.ray d)) p
  rw [Quad.ev_of_isConst hx, Quad.ev_of_isConst hy, Quad.ev_of_isConst hz, A3.ev_sub,
    A3.ev_sub] at key
  simp only [SFan.ev_ray]
  rw [← key]
  rcases hw with (hw | hw) | hw
  · exact ⟨0, by simp [Unit3], by rw [hw]; simp [V3.smul, V3.zero]⟩
  · refine ⟨1, by simp [Unit3], ?_⟩
    rw [hw]; cases m; simp [V3.smul]
  · exact ⟨-1, by simp [Unit3], hw⟩

theorem checkP_sound {X : SFan} {m : V3} (h : checkP X m = true) (p : Par) :
    PCond (X.ev p) m := by
  simp only [checkP, Bool.and_eq_true, bne_iff_ne, ne_eq, List.all_eq_true] at h
  obtain ⟨⟨h0, hr⟩, hpc⟩ := h
  have hc : ∀ i < X.n, (A3.dotC m (X.ray i)).isConst = true ∧ isUnit (hgt X m i) = true :=
    fun i hi => hr _ (SFan.ray_mem hi)
  refine ⟨h0, fun i hi => ?_, ?_⟩
  · rw [SFan.ev_n] at hi
    rw [ray_dot (hc i hi).1]
    exact unit_of_isUnit (hc i hi).2
  intro a b c d ha hb hc' hd hab hcd h1 h2 h3 h4
  rw [SFan.ev_n] at ha hb hc' hd
  rw [ray_dot (hc a ha).1] at h1
  rw [ray_dot (hc b hb).1] at h2
  rw [ray_dot (hc c hc').1] at h3
  rw [ray_dot (hc d hd).1] at h4
  have e := allN_eq_true.1 (allN_eq_true.1 (allN_eq_true.1 (allN_eq_true.1 hpc a ha) b hb)
    c hc') d hd
  simp only [Bool.or_eq_true, Bool.not_eq_true'] at e
  rcases e with e | e
  · simp [h1, h2, h3, h4, isFace_of_face hab, isFace_of_face hcd] at e
  · exact crossOK_sound e p

theorem checkP2_sound {X : SFan} {m : V3} (h : checkP2 X m = true) (p : Par) :
    P2Cond (X.ev p) m := by
  simp only [checkP2, Bool.and_eq_true, List.all_eq_true] at h
  obtain ⟨hc, hj⟩ := h
  obtain ⟨j, hjn, hj⟩ := anyN_eq_true.1 hj
  simp only [Bool.and_eq_true, beq_iff_eq] at hj
  obtain ⟨hj1, hj2⟩ := hj
  have hd : ∀ i < X.n, ((X.ev p).ray i).dot m = hgt X m i :=
    fun i hi => ray_dot (hc _ (SFan.ray_mem hi)) p
  refine ⟨j, by rw [SFan.ev_n]; exact hjn, by rw [hd j hjn, hj1], fun i hi hij => ?_⟩
  rw [SFan.ev_n] at hi
  have e := allN_eq_true.1 hj2 i hi
  simp only [Bool.or_eq_true, beq_iff_eq] at e
  rw [hd i hi]
  rcases e with ((e | e) | e) | e
  · exact absurd e hij
  all_goals omega

theorem Cert.check_sound {X : SFan} {c : Cert} (h : c.check X = true) (p : Par) :
    HasCert (X.ev p) := by
  cases c with
  | S u => exact ⟨u, Or.inl (checkS_sound h p)⟩
  | P m => exact ⟨m, Or.inr (Or.inl (checkP_sound h p))⟩
  | P2 m => exact ⟨m, Or.inr (Or.inr (checkP2_sound h p))⟩

end Oda
