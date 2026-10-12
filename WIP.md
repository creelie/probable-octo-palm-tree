# Work in progress (paused, not released)

Search for a counterexample to Oda's conjecture (every smooth lattice polytope has the
integer decomposition property) in dimensions 3 and 4.

- `oda/common.h`, `oda/oda.cpp`, `oda/res.cpp`, `oda/gen.cpp`: generators of smooth complete
  fans with a strictly convex support function, and an IDP test of the resulting polytopes.
- `oda/validate.py`, `oda/validate_rel.py`: independent Python checks of smoothness and IDP.
- `oda/inst.txt`, `oda/rinst.txt`: sample generated instances.

Result so far: no non-IDP smooth polytope found in any run. Nothing here is a proof.
