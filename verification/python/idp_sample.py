"""Sanity test, not part of the proof: for random parameters and random ample support
functions on every fan of the classification, build the polytope and test
2P = P + P on lattice points directly.
Usage: python3 idp_sample.py [polytopes per fan] [seed]"""
import itertools, os, random, sys
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from fans7 import all_fans, vev
from fractions import Fraction

def solve3(rows, rhs):
    # exact solution of a 3x3 system rows * m = rhs
    a = [[Fraction(x) for x in r] + [Fraction(b)] for r, b in zip(rows, rhs)]
    for c in range(3):
        p = next(r for r in range(c, 3) if a[r][c] != 0)
        a[c], a[p] = a[p], a[c]
        for r in range(3):
            if r != c and a[r][c] != 0:
                f = a[r][c] / a[c][c]
                a[r] = [x - f * y for x, y in zip(a[r], a[c])]
    return [a[i][3] / a[i][i] for i in range(3)]

def polytope(R, cones, h):
    verts = {}
    for c in cones:
        m = solve3([R[i] for i in c], [-h[i] for i in c])
        if any(x.denominator != 1 for x in m):
            return None
        verts[c] = tuple(int(x) for x in m)
    for c in cones:
        for d in cones:
            shared = set(c) & set(d)
            if len(shared) == 2:
                l = (set(d) - shared).pop()
                if sum(x * y for x, y in zip(verts[c], R[l])) <= -h[l]:
                    return None
    return list(verts.values())

def points(R, h, verts, k):
    lo = [k * min(v[a] for v in verts) for a in range(3)]
    hi = [k * max(v[a] for v in verts) for a in range(3)]
    return [p for p in itertools.product(*[range(l, u + 1) for l, u in zip(lo, hi)])
            if all(sum(x * y for x, y in zip(p, r)) >= -k * hh for r, hh in zip(R, h))]

def main(per, seed):
    rnd = random.Random(seed)
    tested = 0
    for X in all_fans():
        got = 0
        tries = 0
        while got < per and tries < 4000:
            tries += 1
            pt = [rnd.randint(-3, 3) for _ in range(6)]
            R = [vev(r, pt) for r in X.rays]
            h = [rnd.randint(0, 4) for _ in R]
            V = polytope(R, X.cones, h)
            if V is None:
                continue
            P = points(R, h, V, 1)
            if len(P) > 60:
                continue
            S = {tuple(a + b for a, b in zip(x, y)) for x in P for y in P}
            for q in points(R, h, V, 2):
                if q not in S:
                    print("NOT IDP:", X.name, pt, h)
                    return 1
            got += 1
            tested += 1
    print("IDP sample: %d polytopes over %d fans, every one satisfies 2P = P + P" % (tested, len(all_fans())))
    return 0

if __name__ == "__main__":
    sys.exit(main(int(sys.argv[1]) if len(sys.argv) > 1 else 3, int(sys.argv[2]) if len(sys.argv) > 2 else 1))
