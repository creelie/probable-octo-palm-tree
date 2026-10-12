// Random search for counterexamples to Oda's conjecture.
// A smooth complete fan in Z^D (built by star subdivisions of base fans) with
// an ample support vector c defines the smooth polytope
//   P_c = { m in R^D : <m,u_r> >= -c_r for every ray u_r }.
// We test: (i) kP is IDP for k <= D-1, (iii) (P_c, P_c') relatively IDP for
// nef c' obtained by pulling back earlier support vectors.
#include <bits/stdc++.h>
using namespace std;
typedef long long ll;
#ifndef DIM
#define DIM 3
#endif
const int D = DIM;
typedef array<ll, D> Vec;
typedef array<int, D> Cone;

static mt19937_64 rng;
static ll SCALEMAX = 3, NBMAX = 20, TMAX = 3, MINPTS = 17;
static ll rnd(ll a, ll b) { return uniform_int_distribution<ll>(a, b)(rng); }

ll det(vector<vector<ll>> a) {
  int n = a.size();
  if (n == 1) return a[0][0];
  if (n == 2) return a[0][0] * a[1][1] - a[0][1] * a[1][0];
  ll s = 0;
  for (int j = 0; j < n; j++) {
    if (!a[0][j]) continue;
    vector<vector<ll>> m;
    for (int i = 1; i < n; i++) {
      vector<ll> row;
      for (int k = 0; k < n; k++) if (k != j) row.push_back(a[i][k]);
      m.push_back(row);
    }
    s += ((j & 1) ? -1 : 1) * a[0][j] * det(m);
  }
  return s;
}

// inverse of a unimodular matrix (rows = vectors), returns false if |det| != 1
bool inv(const vector<Vec>& rows, vector<vector<ll>>& out) {
  vector<vector<ll>> a(D, vector<ll>(D));
  for (int i = 0; i < D; i++) for (int j = 0; j < D; j++) a[i][j] = rows[i][j];
  ll d = det(a);
  if (d != 1 && d != -1) return false;
  out.assign(D, vector<ll>(D));
  for (int i = 0; i < D; i++)
    for (int j = 0; j < D; j++) {
      vector<vector<ll>> m;
      for (int r = 0; r < D; r++) {
        if (r == j) continue;
        vector<ll> row;
        for (int k = 0; k < D; k++) if (k != i) row.push_back(a[r][k]);
        m.push_back(row);
      }
      ll cof = (((i + j) & 1) ? -1 : 1) * (D == 1 ? 1 : det(m));
      out[i][j] = cof * d;  // adj/d with d = +-1
    }
  return true;
}

struct Fan {
  vector<Vec> rays;
  vector<Cone> cones;
};

struct Wall { int k, l; vector<int> tau; vector<ll> alpha; };

// walls: pairs of maximal cones sharing a facet. u_k + u_l = sum alpha_i u_i
bool walls(const Fan& F, vector<Wall>& W) {
  map<vector<int>, vector<pair<int,int>>> mp;  // facet -> (cone, opposite ray)
  for (int ci = 0; ci < (int)F.cones.size(); ci++) {
    for (int o = 0; o < D; o++) {
      vector<int> f;
      for (int t = 0; t < D; t++) if (t != o) f.push_back(F.cones[ci][t]);
      sort(f.begin(), f.end());
      mp[f].push_back({ci, F.cones[ci][o]});
    }
  }
  W.clear();
  for (auto& [f, v] : mp) {
    if (v.size() != 2) return false;  // not complete
    int k = v[0].second, l = v[1].second;
    vector<Vec> rows;
    for (int i : f) rows.push_back(F.rays[i]);
    rows.push_back(F.rays[k]);
    vector<vector<ll>> I;
    if (!inv(rows, I)) return false;
    // coordinates of u_l in basis rows: x with sum x_i rows_i = u_l -> x = u_l * rows^{-1}
    vector<ll> x(D, 0);
    for (int j = 0; j < D; j++)
      for (int i = 0; i < D; i++) x[j] += F.rays[l][i] * I[i][j];
    if (x[D - 1] != -1) return false;  // not smooth across the wall
    Wall w; w.k = k; w.l = l; w.tau = f;
    for (int i = 0; i < D - 1; i++) w.alpha.push_back(x[i]);
    W.push_back(w);
  }
  return true;
}

ll wallval(const Wall& w, const vector<ll>& c) {
  ll s = c[w.k] + c[w.l];
  for (int i = 0; i < D - 1; i++) s -= w.alpha[i] * c[w.tau[i]];
  return s;
}

bool ample(const vector<Wall>& W, const vector<ll>& c, ll lo = 1) {
  for (auto& w : W) if (wallval(w, c) < lo) return false;
  return true;
}

// vertices of P_c
vector<Vec> vertices(const Fan& F, const vector<ll>& c) {
  vector<Vec> V;
  for (auto& s : F.cones) {
    vector<Vec> rows;
    for (int i : s) rows.push_back(F.rays[i]);
    vector<vector<ll>> I;
    inv(rows, I);
    // U m = -c_s  ->  m = -I c_s  (I = U^{-1})
    Vec m{};
    for (int i = 0; i < D; i++) {
      ll t = 0;
      for (int j = 0; j < D; j++) t += I[i][j] * c[s[j]];
      m[i] = -t;
    }
    V.push_back(m);
  }
  return V;
}

struct Poly {
  vector<Vec> pts;
};

inline bool inside(const Fan& F, const vector<ll>& c, ll k, const Vec& m) {
  for (size_t r = 0; r < F.rays.size(); r++) {
    ll s = 0;
    for (int i = 0; i < D; i++) s += m[i] * F.rays[r][i];
    if (s < -k * c[r]) return false;
  }
  return true;
}

void latpts(const Fan& F, const vector<ll>& c, ll k, vector<Vec>& out, ll cap) {
  auto V = vertices(F, c);
  Vec lo, hi;
  for (int i = 0; i < D; i++) { lo[i] = LLONG_MAX; hi[i] = LLONG_MIN; }
  for (auto& v : V) for (int i = 0; i < D; i++) { lo[i] = min(lo[i], k * v[i]); hi[i] = max(hi[i], k * v[i]); }
  out.clear();
  Vec m = lo;
  while (true) {
    if (inside(F, c, k, m)) { out.push_back(m); if ((ll)out.size() > cap) return; }
    int i = 0;
    while (i < D) { if (++m[i] <= hi[i]) break; m[i] = lo[i]; i++; }
    if (i == D) break;
  }
}

// relative IDP: every lattice point of P_{a+b} is p+q, p in P_a, q in P_b.
// Returns number of non-decomposable points (stops at first by default).
ll relcheck(const Fan& F, const vector<ll>& a, const vector<Vec>& Pa,
            const vector<ll>& b, ll cap, Vec* witness) {
  vector<ll> s(a.size());
  for (size_t i = 0; i < a.size(); i++) s[i] = a[i] + b[i];
  vector<Vec> S;
  latpts(F, s, 1, S, cap);
  if ((ll)S.size() > cap) return -1;
  for (auto& m : S) {
    bool ok = false;
    for (auto& p : Pa) {
      Vec q;
      for (int i = 0; i < D; i++) q[i] = m[i] - p[i];
      if (inside(F, b, 1, q)) { ok = true; break; }
    }
    if (!ok) { if (witness) *witness = m; return 1; }
  }
  return 0;
}

// ---------- base fans ----------
Fan simplexFan() {
  Fan F;
  for (int i = 0; i < D; i++) { Vec e{}; e[i] = 1; F.rays.push_back(e); }
  Vec e{}; for (int i = 0; i < D; i++) e[i] = -1; F.rays.push_back(e);
  for (int o = 0; o <= D; o++) { Cone c; int t = 0; for (int i = 0; i <= D; i++) if (i != o) c[t++] = i; F.cones.push_back(c); }
  return F;
}

// toric P^1-bundle over a smooth complete fan B in dimension D-1 with twists
// rays (w_i, a_i), e_D, -e_D + (b, 0) ... we use (w_i,a_i), +e_D, -e_D
struct Fan2 { vector<array<ll,2>> rays; };  // cyclic smooth 2-fan

Fan2 randSurface(int blowups) {
  Fan2 S;
  int t = rnd(0, 2);
  if (t == 0) S.rays = {{1,0},{0,1},{-1,-1}};
  else { ll a = rnd(0, 4); S.rays = {{1,0},{0,1},{-1,a},{0,-1}}; }
  for (int b = 0; b < blowups; b++) {
    int n = S.rays.size(); int i = rnd(0, n - 1); int j = (i + 1) % n;
    array<ll,2> w = {S.rays[i][0] + S.rays[j][0], S.rays[i][1] + S.rays[j][1]};
    S.rays.insert(S.rays.begin() + i + 1, w);
  }
  return S;
}

#if DIM == 3
Fan bundleOverSurface(const Fan2& S, ll A) {
  Fan F;
  int n = S.rays.size();
  for (int i = 0; i < n; i++) F.rays.push_back({S.rays[i][0], S.rays[i][1], rnd(-A, A)});
  F.rays.push_back({0, 0, 1});
  F.rays.push_back({0, 0, -1});
  for (int i = 0; i < n; i++) {
    int j = (i + 1) % n;
    F.cones.push_back({i, j, n});
    F.cones.push_back({i, j, n + 1});
  }
  return F;
}
// P^2-bundle over P^1: fiber rays e1,e2,-e1-e2 ; base rays e3, -e3+a e1+b e2
Fan p2bundle(ll a, ll b) {
  Fan F;
  F.rays = {{1,0,0},{0,1,0},{-1,-1,0},{0,0,1},{a,b,-1}};
  for (int i = 0; i < 3; i++) for (int j = i + 1; j < 3; j++) { F.cones.push_back({i,j,3}); F.cones.push_back({i,j,4}); }
  return F;
}
#endif

#if DIM == 4
// P^1-bundle over a smooth 3-fan B
Fan bundleOver(const Fan& B3dummy);
#endif

// find an ample c by perceptron, then greedily shrink
bool findAmple(const Fan& F, const vector<Wall>& W, vector<ll>& c) {
  int r = F.rays.size();
  c.assign(r, 0);
  for (int it = 0; it < 200000; it++) {
    int bad = -1;
    vector<int> order(W.size()); iota(order.begin(), order.end(), 0);
    shuffle(order.begin(), order.end(), rng);
    for (int i : order) if (wallval(W[i], c) < 1) { bad = i; break; }
    if (bad < 0) break;
    const Wall& w = W[bad];
    c[w.k]++; c[w.l]++;
    for (int i = 0; i < D - 1; i++) c[w.tau[i]] -= w.alpha[i];
    if (it == 199999) return false;
  }
  if (!ample(W, c)) return false;
  // greedy shrink: decrease entries while ample (random order, repeated)
  bool improved = true;
  while (improved) {
    improved = false;
    vector<int> order(r); iota(order.begin(), order.end(), 0);
    shuffle(order.begin(), order.end(), rng);
    for (int i : order) {
      for (int sgn : {-1, 1}) {
        c[i] += sgn;
        ll tot0 = 0;
        if (ample(W, c)) { improved = true; break; }
        c[i] -= sgn;
        (void)tot0;
      }
    }
    // only accept moves that reduce total edge length
    break;
  }
  // proper descent on total edge length
  auto total = [&](const vector<ll>& cc) { ll s = 0; for (auto& w : W) s += wallval(w, cc); return s; };
  ll best = total(c);
  for (int pass = 0; pass < 50; pass++) {
    bool imp = false;
    for (int i = 0; i < r; i++)
      for (int sgn : {-1, 1}) {
        c[i] += sgn;
        if (ample(W, c)) { ll t = total(c); if (t < best) { best = t; imp = true; continue; } }
        c[i] -= sgn;
      }
    if (!imp) break;
  }
  return ample(W, c);
}

// star subdivision at face tau (subset of a maximal cone), cut depth t
bool blowup(Fan& F, vector<ll>& c, vector<vector<ll>>& nefs, const vector<int>& tau, ll t) {
  Vec u{};
  for (int i : tau) for (int j = 0; j < D; j++) u[j] += F.rays[i][j];
  int n = F.rays.size();
  F.rays.push_back(u);
  ll cs = 0; for (int i : tau) cs += c[i];
  c.push_back(cs - t);
  for (auto& q : nefs) { ll s = 0; for (int i : tau) s += q[i]; q.push_back(s); }
  vector<Cone> nc;
  for (auto& s : F.cones) {
    bool contains = true;
    for (int i : tau) if (find(s.begin(), s.end(), i) == s.end()) { contains = false; break; }
    if (!contains) { nc.push_back(s); continue; }
    for (int i : tau) {
      Cone s2 = s;
      for (int j = 0; j < D; j++) if (s2[j] == i) s2[j] = n;
      nc.push_back(s2);
    }
  }
  F.cones = nc;
  return true;
}

string fanstr(const Fan& F, const vector<ll>& c) {
  ostringstream os;
  os << "rays";
  for (size_t i = 0; i < F.rays.size(); i++) {
    os << " (";
    for (int j = 0; j < D; j++) os << F.rays[i][j] << (j + 1 < D ? "," : "");
    os << ")";
  }
  os << " c";
  for (auto x : c) os << " " << x;
  os << " cones";
  for (auto& s : F.cones) { os << " "; for (int j = 0; j < D; j++) os << s[j] << (j + 1 < D ? "." : ""); }
  return os.str();
}

int main(int argc, char** argv) {
  ll seed = argc > 1 ? atoll(argv[1]) : 1;
  ll iters = argc > 2 ? atoll(argv[2]) : 1000;
  ll maxpts = argc > 3 ? atoll(argv[3]) : 400;
  rng.seed(seed);
  if (getenv("SCALEMAX")) SCALEMAX = atoll(getenv("SCALEMAX"));
  if (getenv("NBMAX")) NBMAX = atoll(getenv("NBMAX"));
  if (getenv("TMAX")) TMAX = atoll(getenv("TMAX"));
  if (getenv("MINPTS")) MINPTS = atoll(getenv("MINPTS"));
  ll tested = 0, relTested = 0, skipped = 0;
  map<ll, ll> sizeHist;
  for (ll it = 0; it < iters; it++) {
    Fan F;
#if DIM == 3
    int bt = rnd(0, 3);
    if (bt == 0) F = simplexFan();
    else if (bt == 1) F = p2bundle(rnd(-4, 4), rnd(-4, 4));
    else F = bundleOverSurface(randSurface(rnd(0, 5)), rnd(0, 4));
#else
    F = simplexFan();
#endif
    vector<Wall> W;
    if (!walls(F, W)) { fprintf(stderr, "bad base fan\n"); continue; }
    vector<ll> c;
    if (!findAmple(F, W, c)) { skipped++; continue; }
    // optional scaling of the start
    { ll sc = rnd(1, SCALEMAX); for (auto& x : c) x *= sc; }
    vector<vector<ll>> nefs;
    nefs.push_back(c);
    int nb = rnd(1, NBMAX);
    bool fail = false;
    for (int b = 0; b < nb; b++) {
      // choose a random face of a random maximal cone
      bool done = false;
      for (int attempt = 0; attempt < 30 && !done; attempt++) {
        const Cone& s = F.cones[rnd(0, F.cones.size() - 1)];
        int sz = rnd(2, D);
        vector<int> idx(s.begin(), s.end());
        shuffle(idx.begin(), idx.end(), rng);
        vector<int> tau(idx.begin(), idx.begin() + sz);
        ll t = rnd(1, TMAX);
        Fan F2 = F; vector<ll> c2 = c; auto n2 = nefs;
        blowup(F2, c2, n2, tau, t);
        vector<Wall> W2;
        if (!walls(F2, W2)) { fail = true; break; }
        if (ample(W2, c2)) { F = F2; c = c2; nefs = n2; W = W2; done = true; if (rnd(0, 2) == 0) nefs.push_back(c); }
      }
      if (fail) break;
    }
    if (fail) { skipped++; continue; }
    vector<Vec> P;
    latpts(F, c, 1, P, maxpts);
    if ((ll)P.size() > maxpts || (ll)P.size() < MINPTS) { skipped++; continue; }
    sizeHist[P.size() / 10 * 10]++;
    if (getenv("DUMP") && it % atoi(getenv("DUMP")) == 0) printf("INST npts=%zu | %s\n", P.size(), fanstr(F, c).c_str());
    shuffle(P.begin(), P.end(), rng);
    // (i): kP for k = 2..D-1, via relcheck(P, (k-1)P)
    for (ll k = 2; k <= D - 1; k++) {
      vector<ll> km(c.size());
      for (size_t i = 0; i < c.size(); i++) km[i] = (k - 1) * c[i];
      Vec wit;
      ll r = relcheck(F, c, P, km, maxpts * 40, &wit);
      tested++;
      if (r > 0) {
        printf("NONIDP k=%lld witness", k);
        for (int i = 0; i < D; i++) printf(" %lld", wit[i]);
        printf(" | %s\n", fanstr(F, c).c_str());
        fflush(stdout);
      }
    }
    // (iii): (P, Q) for nef Q pulled back
    for (auto& q : nefs) {
      Vec wit;
      ll r = relcheck(F, c, P, q, maxpts * 40, &wit);
      relTested++;
      if (r > 0) {
        printf("NONREL witness");
        for (int i = 0; i < D; i++) printf(" %lld", wit[i]);
        printf(" | %s | q", fanstr(F, c).c_str());
        for (auto x : q) printf(" %lld", x);
        printf("\n");
        fflush(stdout);
      }
    }
  }
  printf("DONE seed=%lld tested=%lld rel=%lld skipped=%lld\n", seed, tested, relTested, skipped);
  for (auto& [k, v] : sizeHist) printf("  pts %lld-%lld: %lld\n", k, k + 9, v);
}
