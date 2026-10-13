"""Smooth complete 3-fans with seven rays, with rays affine in integer parameters.

Polynomials in the parameters p0, ..., p5 are dictionaries {exponent tuple: integer}.
Every family of the classification is built here; the certificates of Propositions 6
(slicing), 7 (projection) and 8 (projection with one upper facet) of the paper are
checked as polynomial identities, so a check holds for every integer value of the
parameters.
"""
import itertools

NV = 6  # number of parameters


def const(c):
    return {(0,) * NV: c} if c else {}


def var(i):
    e = [0] * NV
    e[i] = 1
    return {tuple(e): 1}


def padd(*ps):
    r = {}
    for p in ps:
        for e, c in p.items():
            r[e] = r.get(e, 0) + c
    return {e: c for e, c in r.items() if c}


def pscale(k, p):
    return {e: k * c for e, c in p.items() if k * c}


def pmul(p, q):
    r = {}
    for e1, c1 in p.items():
        for e2, c2 in q.items():
            e = tuple(x + y for x, y in zip(e1, e2))
            r[e] = r.get(e, 0) + c1 * c2
    return {e: c for e, c in r.items() if c}


def pneg(p):
    return pscale(-1, p)


def is_const(p):
    return all(sum(e) == 0 for e in p)


def cval(p):
    assert is_const(p)
    return p.get((0,) * NV, 0)


def aff(c, coeffs=None):
    """c + sum coeffs[i] * p_i"""
    p = const(c)
    for i, k in (coeffs or {}).items():
        p = padd(p, pscale(k, var(i)))
    return p


def vec(*xs):
    return tuple(x if isinstance(x, dict) else const(x) for x in xs)


def vadd(*vs):
    return tuple(padd(*cs) for cs in zip(*vs))


def vsub(u, v):
    return tuple(padd(a, pneg(b)) for a, b in zip(u, v))


def det3(u, v, w):
    t1 = pmul(u[0], padd(pmul(v[1], w[2]), pneg(pmul(v[2], w[1]))))
    t2 = pmul(u[1], padd(pmul(v[0], w[2]), pneg(pmul(v[2], w[0]))))
    t3 = pmul(u[2], padd(pmul(v[0], w[1]), pneg(pmul(v[1], w[0]))))
    return padd(t1, pneg(t2), t3)


def cross(u, v):
    return (padd(pmul(u[1], v[2]), pneg(pmul(u[2], v[1]))),
            padd(pmul(u[2], v[0]), pneg(pmul(u[0], v[2]))),
            padd(pmul(u[0], v[1]), pneg(pmul(u[1], v[0]))))


def dotc(m, v):
    """m constant integer vector, v polynomial vector"""
    return padd(*[pscale(k, x) for k, x in zip(m, v)])


def vev(v, pt):
    def ev(p):
        s = 0
        for e, c in p.items():
            t = c
            for x, k in zip(pt, e):
                t *= x ** k
            s += t
        return s
    return tuple(ev(x) for x in v)


class Fan:
    def __init__(self, rays, cones, name):
        self.rays = list(rays)
        self.cones = sorted(tuple(sorted(c)) for c in cones)
        self.name = name

    def faces2(self):
        return sorted({f for c in self.cones for f in itertools.combinations(c, 2)})

    def blow_point(self, c):
        i, j, k = c
        n = len(self.rays)
        w = vadd(self.rays[i], self.rays[j], self.rays[k])
        cones = [x for x in self.cones if x != c] + [(n, j, k), (i, n, k), (i, j, n)]
        return Fan(self.rays + [w], cones, self.name + " pt(%d,%d,%d)" % c)

    def blow_curve(self, f):
        i, j = f
        n = len(self.rays)
        w = vadd(self.rays[i], self.rays[j])
        cones = []
        for x in self.cones:
            if i in x and j in x:
                k = [t for t in x if t not in f][0]
                cones += [(n, j, k), (i, n, k)]
            else:
                cones.append(x)
        return Fan(self.rays + [w], cones, self.name + " cv(%d,%d)" % f)

    def blowups(self):
        return [self.blow_point(c) for c in self.cones] + [self.blow_curve(f) for f in self.faces2()]


def bundle_cones(k, top, bot):
    out = []
    for i in range(k):
        out.append((i, (i + 1) % k, top))
        out.append((i, (i + 1) % k, bot))
    return out


def Z():
    a = aff(0, {0: 1})
    rays = [vec(1, 0, 0), vec(0, 1, 0), vec(-1, -1, a), vec(0, 0, 1), vec(0, 0, -1)]
    cones = [(0, 1, 3), (0, 1, 4), (1, 2, 3), (1, 2, 4), (0, 2, 3), (0, 2, 4)]
    return Fan(rays, cones, "Z")


def W():
    a = aff(0, {0: 1}); b = aff(0, {1: 1})
    rays = [vec(1, 0, 0), vec(0, 1, 0), vec(-1, -1, 0), vec(0, 0, 1), vec(a, b, -1)]
    cones = [(0, 1, 3), (0, 1, 4), (1, 2, 3), (1, 2, 4), (0, 2, 3), (0, 2, 4)]
    return Fan(rays, cones, "W")


def Fa_base():
    a = aff(0, {0: 1})
    return [(const(1), const(0)), (const(0), const(1)), (const(-1), a), (const(0), const(-1))]


def bundle(base, name):
    k = len(base)
    rays = [(x, y, aff(0, {1 + i: 1})) for i, (x, y) in enumerate(base)]
    rays += [vec(0, 0, 1), vec(0, 0, -1)]
    return Fan(rays, bundle_cones(k, k, k + 1), name)


def F():
    return bundle(Fa_base(), "F")


def G(j):
    base = Fa_base()
    v, w = base[j], base[(j + 1) % 4]
    new = (padd(v[0], w[0]), padd(v[1], w[1]))
    return bundle(base[:j + 1] + [new] + base[j + 1:], "G%d" % j)


def C():
    a = aff(0, {0: 1})
    rays = [vec(1, 0, 0), vec(0, 1, 0), vec(0, 0, 1), vec(0, -1, a), vec(0, 0, -1), vec(0, 1, -1), vec(-1, 2, -1)]
    cones = [(0, 1, 2), (0, 2, 3), (0, 3, 4), (0, 4, 5), (0, 5, 6), (0, 1, 6), (1, 2, 6), (2, 3, 6), (3, 4, 6), (4, 5, 6)]
    return Fan(rays, cones, "C")


def all_fans():
    out = []
    for Y in Z().blowups():
        out += Y.blowups()
    for Y in W().blowups():
        out += Y.blowups()
    out += F().blowups()
    out += [G(j) for j in range(4)]
    out.append(C())
    return out


def smooth(X):
    if len(X.rays) != 7 or len(X.cones) != 10:
        return False
    cnt = {}
    for c in X.cones:
        for f in itertools.combinations(c, 2):
            cnt[f] = cnt.get(f, 0) + 1
    if any(v != 2 for v in cnt.values()):
        return False
    for c in X.cones:
        d = det3(*[X.rays[i] for i in c])
        if not is_const(d) or abs(cval(d)) != 1:
            return False
    return True


def adj_coords(X, c, u):
    """coordinates of the constant vector u in the cone c, as polynomials (det(c) = +-1)"""
    r = [X.rays[i] for i in c]
    d = cval(det3(*r))
    U = tuple(const(x) for x in u)
    # Cramer: x_i = det(cone with column i replaced by u) / det
    xs = []
    for i in range(3):
        rr = list(r)
        rr[i] = U
        xs.append(pscale(d, det3(*rr)))
    return xs


def check_S(X, u):
    for c in X.cones:
        xs = adj_coords(X, c, u)
        if not all(is_const(x) and abs(cval(x)) <= 1 for x in xs):
            return False
        lhs = vadd(*[tuple(pscale(cval(x), t) for t in X.rays[i]) for x, i in zip(xs, c)])
        if any(padd(l, pneg(const(k))) for l, k in zip(lhs, u)):
            return False
    U = tuple(const(x) for x in u)
    n = len(X.rays)
    nb = [set() for _ in range(n)]
    for i, j in itertools.combinations(range(n), 2):
        d = det3(U, X.rays[i], X.rays[j])
        if not is_const(d) or abs(cval(d)) >= 2:
            nb[i].add(j); nb[j].add(i)
    return all(len(nb[i] & nb[j]) <= 1 for i, j in itertools.combinations(range(n), 2))


def heights(X, m):
    return [dotc(m, r) for r in X.rays]


def check_P(X, m):
    h = heights(X, m)
    if not all(is_const(x) and abs(cval(x)) <= 1 for x in h):
        return False
    h = [cval(x) for x in h]
    low = [f for f in X.faces2() if h[f[0]] == 1 and h[f[1]] == 1]
    up = [f for f in X.faces2() if h[f[0]] == -1 and h[f[1]] == -1]
    k = next(i for i in range(3) if m[i])
    for f in low:
        da = vsub(X.rays[f[0]], X.rays[f[1]])
        for g in up:
            db = vsub(X.rays[g[0]], X.rays[g[1]])
            cr = cross(da, db)
            if not all(is_const(t) for t in cr):
                return False
            cr = [cval(t) for t in cr]
            if cr[k] % m[k]:
                return False
            d = cr[k] // m[k]
            if any(cr[i] != d * m[i] for i in range(3)) or abs(d) > 1:
                return False
    return True


def check_P2(X, m):
    h = heights(X, m)
    if not all(is_const(x) for x in h):
        return False
    h = [cval(x) for x in h]
    return sorted(x for x in h if x < 0) == [-1] and all(x <= 2 for x in h)


CHECK = {"S": check_S, "P": check_P, "P2": check_P2}


def directions(box=2):
    out = []
    for v in itertools.product(range(-box, box + 1), repeat=3):
        g = 0
        for x in v:
            g = abs(x) if g == 0 else __import__("math").gcd(g, abs(x))
        if g == 1:
            out.append(v)
    return out
