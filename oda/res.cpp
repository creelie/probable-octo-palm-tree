// Strong-form search: R = random lattice tetrahedron (often empty / non-IDP),
// Sigma = random smooth projective refinement of the normal fan of R by stellar
// subdivisions, P = small ample polytope on Sigma.  Test: P IDP, (P,R), (P,P_c').
#include "common.h"

typedef array<ll,3> V3;
V3 sub(V3 a, V3 b) { return {a[0]-b[0], a[1]-b[1], a[2]-b[2]}; }
V3 cross(V3 a, V3 b) { return {a[1]*b[2]-a[2]*b[1], a[2]*b[0]-a[0]*b[2], a[0]*b[1]-a[1]*b[0]}; }
ll dot(V3 a, V3 b) { return a[0]*b[0]+a[1]*b[1]+a[2]*b[2]; }
V3 prim(V3 a) { ll g = __gcd(__gcd(llabs(a[0]), llabs(a[1])), llabs(a[2])); return {a[0]/g, a[1]/g, a[2]/g}; }

ll coneDet(const Fan& F, const Cone& s) {
  vector<vector<ll>> a(3, vector<ll>(3));
  for (int i = 0; i < 3; i++) for (int j = 0; j < 3; j++) a[i][j] = F.rays[s[i]][j];
  return det(a);
}

// stellar subdivision at lattice vector v lying in cone s (coefficients lam >= 0)
void subdivide(Fan& F, const Vec& v, const vector<int>& tau) {
  int n = F.rays.size();
  F.rays.push_back(v);
  vector<Cone> nc;
  for (auto& s : F.cones) {
    bool contains = true;
    for (int i : tau) if (find(s.begin(), s.end(), i) == s.end()) { contains = false; break; }
    if (!contains) { nc.push_back(s); continue; }
    for (int i : tau) {
      Cone s2 = s;
      for (int j = 0; j < 3; j++) if (s2[j] == i) s2[j] = n;
      nc.push_back(s2);
    }
  }
  F.cones = nc;
}

// find a lattice point in the half-open parallelepiped of singular cone s
bool parPoint(const Fan& F, const Cone& s, Vec& v, vector<int>& tau, int mode) {
  vector<vector<ll>> a(3, vector<ll>(3));
  for (int i = 0; i < 3; i++) for (int j = 0; j < 3; j++) a[i][j] = F.rays[s[i]][j];
  ll d = det(a);
  ll ad = llabs(d);
  // adjugate: inverse * d
  vector<vector<ll>> adj(3, vector<ll>(3));
  for (int i = 0; i < 3; i++) for (int j = 0; j < 3; j++) {
    vector<vector<ll>> m;
    for (int r = 0; r < 3; r++) { if (r == j) continue; vector<ll> row; for (int k = 0; k < 3; k++) if (k != i) row.push_back(a[r][k]); m.push_back(row); }
    adj[i][j] = (((i + j) & 1) ? -1 : 1) * det(m);
  }
  // enumerate lambda = (k0,k1,k2)/ad via v = sum lambda_i u_i lattice: iterate over v in box
  Vec lo{}, hi{};
  for (int j = 0; j < 3; j++) { lo[j] = 0; hi[j] = 0; for (int i = 0; i < 3; i++) { if (a[i][j] < 0) lo[j] += a[i][j]; else hi[j] += a[i][j]; } }
  vector<pair<ll, pair<Vec, vector<int>>>> cands;
  for (ll x = lo[0]; x <= hi[0]; x++) for (ll y = lo[1]; y <= hi[1]; y++) for (ll z = lo[2]; z <= hi[2]; z++) {
    // lambda_j * d = sum_i v_i adj[i][j]
    ll L[3]; bool ok = true; ll tot = 0;
    for (int j = 0; j < 3; j++) {
      L[j] = x * adj[0][j] + y * adj[1][j] + z * adj[2][j];
      if (d < 0) L[j] = -L[j];
      if (L[j] < 0 || L[j] >= ad) { ok = false; break; }
      tot += L[j];
    }
    if (!ok || tot == 0) continue;
    vector<int> t;
    for (int j = 0; j < 3; j++) if (L[j] > 0) t.push_back(s[j]);
    cands.push_back({tot, {Vec{x, y, z}, t}});
  }
  if (cands.empty()) return false;
  if (mode == 0) { sort(cands.begin(), cands.end(), [](auto& p, auto& q){ return p.first < q.first; }); }
  else shuffle(cands.begin(), cands.end(), rng);
  v = cands[0].second.first; tau = cands[0].second.second;
  return true;
}

int main(int argc, char** argv) {
  ll seed = argc > 1 ? atoll(argv[1]) : 1;
  ll iters = argc > 2 ? atoll(argv[2]) : 100;
  ll maxpts = argc > 3 ? atoll(argv[3]) : 2000;
  ll box = argc > 4 ? atoll(argv[4]) : 3;
  rng.seed(seed);
  ll tested = 0, rel = 0, skipped = 0;
  map<ll,ll> hist;
  for (ll it = 0; it < iters; it++) {
    // random lattice tetrahedron
    V3 P0{0,0,0}, P1, P2, P3;
    int fam = rnd(0, 1);
    if (fam == 0) {  // White's empty tetrahedra conv{0, e1, e3, (p,q,1)}
      ll q = rnd(2, box * 4), p = rnd(1, q - 1);
      while (__gcd(p, q) != 1) p = rnd(1, q - 1);
      P1 = {1,0,0}; P2 = {0,0,1}; P3 = {p,q,1};
    } else {
      P1 = {rnd(-box,box), rnd(-box,box), rnd(-box,box)};
      P2 = {rnd(-box,box), rnd(-box,box), rnd(-box,box)};
      P3 = {rnd(-box,box), rnd(-box,box), rnd(-box,box)};
    }
    V3 VV[4] = {P0, P1, P2, P3};
    if (dot(cross(sub(P1,P0), sub(P2,P0)), sub(P3,P0)) == 0) { skipped++; continue; }
    // inner facet normals n_i (facet opposite V_i)
    Fan F;
    for (int i = 0; i < 4; i++) {
      int a = (i+1)%4, b = (i+2)%4, c = (i+3)%4;
      V3 n = prim(cross(sub(VV[b],VV[a]), sub(VV[c],VV[a])));
      if (dot(n, sub(VV[i], VV[a])) < 0) n = {-n[0], -n[1], -n[2]};
      F.rays.push_back({n[0], n[1], n[2]});
    }
    for (int j = 0; j < 4; j++) { Cone s; int t = 0; for (int i = 0; i < 4; i++) if (i != j) s[t++] = i; F.cones.push_back(s); }
    int mode = rnd(0, 1);
    bool bad = false;
    for (int step = 0; step < 400; step++) {
      int si = -1;
      vector<int> order(F.cones.size()); iota(order.begin(), order.end(), 0);
      shuffle(order.begin(), order.end(), rng);
      for (int i : order) if (llabs(coneDet(F, F.cones[i])) > 1) { si = i; break; }
      if (si < 0) break;
      Vec v; vector<int> tau;
      if (!parPoint(F, F.cones[si], v, tau, mode)) { bad = true; break; }
      subdivide(F, v, tau);
      if (step == 399) bad = true;
    }
    if (bad) { skipped++; continue; }
    // optional extra random smooth blow-ups
    vector<Wall> W;
    if (!walls(F, W)) { skipped++; continue; }
    int extra = rnd(0, 3);
    for (int e = 0; e < extra; e++) {
      const Cone& s = F.cones[rnd(0, F.cones.size()-1)];
      int sz = rnd(2, 3);
      vector<int> idx(s.begin(), s.end()); shuffle(idx.begin(), idx.end(), rng);
      vector<int> tau(idx.begin(), idx.begin()+sz);
      Vec u{}; for (int i : tau) for (int j = 0; j < 3; j++) u[j] += F.rays[i][j];
      subdivide(F, u, tau);
    }
    if (!walls(F, W)) { skipped++; continue; }
    vector<ll> c;
    if (!findAmple(F, W, c)) { skipped++; continue; }
    // R support vector
    vector<ll> cR(F.rays.size());
    for (size_t r = 0; r < F.rays.size(); r++) {
      ll mn = LLONG_MAX;
      for (int i = 0; i < 4; i++) mn = min(mn, dot(VV[i], V3{F.rays[r][0], F.rays[r][1], F.rays[r][2]}));
      cR[r] = -mn;
    }
    if (!ample(W, cR, 0)) { printf("BUG R not nef\n"); continue; }
    vector<Vec> P;
    latpts(F, c, 1, P, maxpts);
    if ((ll)P.size() > maxpts) { skipped++; continue; }
    hist[min<ll>(P.size(), 2000) / 50 * 50]++;
    shuffle(P.begin(), P.end(), rng);
    Vec wit;
    tested++;
    if (relcheck(F, c, P, c, maxpts * 20, &wit) > 0) {
      printf("NONIDP witness %lld %lld %lld | %s\n", wit[0], wit[1], wit[2], fanstr(F, c).c_str()); fflush(stdout);
    }
    rel++;
    ll rr = relcheck(F, c, P, cR, maxpts * 20, &wit);
    if (rr > 0) {
      printf("NONREL_R witness %lld %lld %lld | %s | q", wit[0], wit[1], wit[2], fanstr(F, c).c_str());
      for (auto x : cR) printf(" %lld", x);
      printf("\n"); fflush(stdout);
    }
    if (getenv("DUMP") && it % atoi(getenv("DUMP")) == 0) {
      printf("INST npts=%zu rays=%zu | %s | q", P.size(), F.rays.size(), fanstr(F, c).c_str());
      for (auto x : cR) printf(" %lld", x);
      printf("\n");
    }
  }
  printf("DONE seed=%lld tested=%lld rel=%lld skipped=%lld\n", seed, tested, rel, skipped);
  for (auto& [k, v] : hist) printf("  pts %lld+: %lld\n", k, v);
}
