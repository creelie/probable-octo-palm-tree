// Search over smooth projective fans generated recursively:
// fan(d) = P^d | P^1-bundle over fan(d-1) with twists | P^k-bundle over P^1,
// each followed by random smooth blow-ups; then random blow-up cuts of an
// ample polytope.  Tests: kP IDP for k <= D-1 and (P, nef pullbacks).
#include "common.h"

struct RFan { int d; vector<vector<ll>> rays; vector<vector<int>> cones; };

RFan rsimplex(int d) {
  RFan F; F.d = d;
  for (int i = 0; i < d; i++) { vector<ll> e(d, 0); e[i] = 1; F.rays.push_back(e); }
  F.rays.push_back(vector<ll>(d, -1));
  for (int o = 0; o <= d; o++) { vector<int> c; for (int i = 0; i <= d; i++) if (i != o) c.push_back(i); F.cones.push_back(c); }
  return F;
}

void rblowup(RFan& F, const vector<int>& tau) {
  vector<ll> u(F.d, 0);
  for (int i : tau) for (int j = 0; j < F.d; j++) u[j] += F.rays[i][j];
  int n = F.rays.size(); F.rays.push_back(u);
  vector<vector<int>> nc;
  for (auto& s : F.cones) {
    bool ok = true;
    for (int i : tau) if (find(s.begin(), s.end(), i) == s.end()) { ok = false; break; }
    if (!ok) { nc.push_back(s); continue; }
    for (int i : tau) { auto s2 = s; for (auto& x : s2) if (x == i) x = n; nc.push_back(s2); }
  }
  F.cones = nc;
}

RFan rbundle(const RFan& B, ll A) {  // P^1-bundle over B
  RFan F; F.d = B.d + 1;
  int n = B.rays.size();
  for (auto& w : B.rays) { auto v = w; v.push_back(rnd(-A, A)); F.rays.push_back(v); }
  vector<ll> top(F.d, 0), bot(F.d, 0); top[B.d] = 1; bot[B.d] = -1;
  F.rays.push_back(top); F.rays.push_back(bot);
  for (auto& s : B.cones) { auto a = s; a.push_back(n); F.cones.push_back(a); auto b = s; b.push_back(n + 1); F.cones.push_back(b); }
  return F;
}

RFan rprojbundleP1(int k, int d, ll A) {  // P^k-bundle over P^1 inside dim d = k+1
  RFan F; F.d = d;
  RFan Fi = rsimplex(k);
  for (auto& w : Fi.rays) { auto v = w; v.push_back(0); F.rays.push_back(v); }
  vector<ll> top(d, 0); top[k] = 1; F.rays.push_back(top);
  vector<ll> bot(d, 0); for (int i = 0; i < k; i++) bot[i] = rnd(-A, A); bot[k] = -1; F.rays.push_back(bot);
  int n = Fi.rays.size();
  for (auto& s : Fi.cones) { auto a = s; a.push_back(n); F.cones.push_back(a); auto b = s; b.push_back(n + 1); F.cones.push_back(b); }
  return F;
}

RFan rproduct(const RFan& X, const RFan& Y) {
  RFan F; F.d = X.d + Y.d;
  for (auto& w : X.rays) { auto v = w; v.resize(F.d, 0); F.rays.push_back(v); }
  for (auto& w : Y.rays) { vector<ll> v(X.d, 0); v.insert(v.end(), w.begin(), w.end()); F.rays.push_back(v); }
  int nx = X.rays.size();
  for (auto& s : X.cones) for (auto& t : Y.cones) { auto a = s; for (int j : t) a.push_back(nx + j); F.cones.push_back(a); }
  return F;
}

RFan rrandom(int d, int depth) {
  RFan F;
  if (d == 1) { F.d = 1; F.rays = {{1}, {-1}}; F.cones = {{0}, {1}}; }
  else {
    int t = rnd(0, 5);
    if (t == 0) F = rsimplex(d);
    else if (t <= 2) F = rbundle(rrandom(d - 1, depth), rnd(0, 3));
    else if (t == 3) F = rprojbundleP1(d - 1, d, rnd(0, 3));
    else { int a = rnd(1, d - 1); F = rproduct(rrandom(a, depth), rrandom(d - a, depth)); }
  }
  int nb = rnd(0, depth);
  for (int b = 0; b < nb && d >= 2; b++) {
    auto& s = F.cones[rnd(0, F.cones.size() - 1)];
    int sz = rnd(2, d);
    vector<int> idx = s; shuffle(idx.begin(), idx.end(), rng);
    idx.resize(sz);
    rblowup(F, idx);
  }
  return F;
}

Fan toFan(const RFan& R) {
  Fan F;
  for (auto& r : R.rays) { Vec v; for (int i = 0; i < D; i++) v[i] = r[i]; F.rays.push_back(v); }
  for (auto& s : R.cones) { Cone c; for (int i = 0; i < D; i++) c[i] = s[i]; F.cones.push_back(c); }
  return F;
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
  ll DEPTH = getenv("DEPTH") ? atoll(getenv("DEPTH")) : 3;
  ll tested = 0, relTested = 0, skipped = 0;
  map<ll, ll> sizeHist;
  for (ll it = 0; it < iters; it++) {
    Fan F = toFan(rrandom(D, DEPTH));
    vector<Wall> W;
    if (!walls(F, W)) { skipped++; continue; }
    vector<ll> c;
    if (!findAmple(F, W, c)) { skipped++; continue; }
    { ll sc = rnd(1, SCALEMAX); for (auto& x : c) x *= sc; }
    vector<vector<ll>> nefs;
    nefs.push_back(c);
    int nb = rnd(0, NBMAX);
    for (int b = 0; b < nb; b++) {
      for (int attempt = 0; attempt < 30; attempt++) {
        const Cone& s = F.cones[rnd(0, F.cones.size() - 1)];
        int sz = rnd(2, D);
        vector<int> idx(s.begin(), s.end());
        shuffle(idx.begin(), idx.end(), rng);
        vector<int> tau(idx.begin(), idx.begin() + sz);
        ll t = rnd(1, TMAX);
        Fan F2 = F; vector<ll> c2 = c; auto n2 = nefs;
        blowup(F2, c2, n2, tau, t);
        vector<Wall> W2;
        if (!walls(F2, W2)) break;
        if (ample(W2, c2)) { F = F2; c = c2; nefs = n2; W = W2; if (rnd(0, 2) == 0) nefs.push_back(c); break; }
      }
    }
    vector<Vec> P;
    latpts(F, c, 1, P, maxpts);
    if ((ll)P.size() > maxpts || (ll)P.size() < MINPTS) { skipped++; continue; }
    sizeHist[P.size() / 50 * 50]++;
    if (getenv("DUMP") && it % atoi(getenv("DUMP")) == 0) printf("INST npts=%zu | %s\n", P.size(), fanstr(F, c).c_str());
    shuffle(P.begin(), P.end(), rng);
    for (ll k = 2; k <= D - 1; k++) {
      vector<ll> km(c.size());
      for (size_t i = 0; i < c.size(); i++) km[i] = (k - 1) * c[i];
      Vec wit;
      ll r = relcheck(F, c, P, km, maxpts * 200, &wit);
      tested++;
      if (r > 0) {
        printf("NONIDP k=%lld witness", k);
        for (int i = 0; i < D; i++) printf(" %lld", wit[i]);
        printf(" | %s\n", fanstr(F, c).c_str()); fflush(stdout);
      }
    }
    for (auto& q : nefs) {
      Vec wit;
      ll r = relcheck(F, c, P, q, maxpts * 200, &wit);
      relTested++;
      if (r > 0) {
        printf("NONREL witness");
        for (int i = 0; i < D; i++) printf(" %lld", wit[i]);
        printf(" | %s | q", fanstr(F, c).c_str());
        for (auto x : q) printf(" %lld", x);
        printf("\n"); fflush(stdout);
      }
    }
  }
  printf("DONE seed=%lld tested=%lld rel=%lld skipped=%lld\n", seed, tested, relTested, skipped);
  for (auto& [k, v] : sizeHist) printf("  pts %lld+: %lld\n", k, v);
}
