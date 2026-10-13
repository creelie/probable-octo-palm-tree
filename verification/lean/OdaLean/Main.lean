import OdaLean.Cert.All
import OdaLean.Sound

/-!
# The certificate theorem

Every fan of the classification of smooth complete fans with seven rays (Proposition 9 of
the paper) carries one of the three certificates, for every value of its parameters:

* the double blowups of `Z_a` and of `W_{a,b}`,
* the blowups of the `ℙ¹`-bundles over the Hirzebruch surfaces,
* the `ℙ¹`-bundles over the surfaces with five rays,
* the family (a) of Fujino and Payne.
-/

namespace Oda

theorem coverZ : covered2 famZ rowsZ = true := by decide +kernel

theorem coverW : covered2 famW rowsW = true := by decide +kernel

theorem coverF : covered1 famF rowsF = true := by decide +kernel

theorem double_sound {B : SFan} {rows : List (Center × Center × Cert)}
    (hcov : covered2 B rows = true) (hok : rows.all (rowOK2 B) = true) :
    ∀ c₁ ∈ B.centers, ∀ c₂ ∈ (B.blow c₁).centers, ∀ p, HasCert (((B.blow c₁).blow c₂).ev p) := by
  intro c₁ h₁ c₂ h₂ p
  simp only [covered2, List.all_eq_true, List.any_eq_true, Bool.and_eq_true, beq_iff_eq] at hcov
  obtain ⟨r, hr, e₁, e₂⟩ := hcov c₁ h₁ c₂ h₂
  have h := List.all_eq_true.1 hok r hr
  rw [rowOK2, e₁, e₂] at h
  exact Cert.check_sound h p

theorem single_sound {B : SFan} {rows : List (Center × Cert)}
    (hcov : covered1 B rows = true) (hok : rows.all (rowOK1 B) = true) :
    ∀ c ∈ B.centers, ∀ p, HasCert ((B.blow c).ev p) := by
  intro c hc p
  simp only [covered1, List.all_eq_true, List.any_eq_true, beq_iff_eq] at hcov
  obtain ⟨r, hr, e⟩ := hcov c hc
  have h := List.all_eq_true.1 hok r hr
  rw [rowOK1, e] at h
  exact Cert.check_sound h p

/-- The finite part of the proof: every fan of the classification has a certificate, for every
value of the parameters. -/
theorem certificate :
    (∀ c₁ ∈ famZ.centers, ∀ c₂ ∈ (famZ.blow c₁).centers, ∀ p,
      HasCert (((famZ.blow c₁).blow c₂).ev p)) ∧
    (∀ c₁ ∈ famW.centers, ∀ c₂ ∈ (famW.blow c₁).centers, ∀ p,
      HasCert (((famW.blow c₁).blow c₂).ev p)) ∧
    (∀ c ∈ famF.centers, ∀ p, HasCert ((famF.blow c).ev p)) ∧
    (∀ j < 4, ∀ p, HasCert ((famG j).ev p)) ∧
    (∀ p, HasCert (famC.ev p)) := by
  refine ⟨double_sound coverZ rowsZ_ok, double_sound coverW rowsW_ok,
    single_sound coverF rowsF_ok, fun j hj p => ?_, fun p => Cert.check_sound certC_ok p⟩
  exact Cert.check_sound (allN_eq_true.1 certsG_ok j hj) p

end Oda
