// Independent check of data/certificates.txt, by evaluation on a second grid.
// Every quantity checked is a polynomial of total degree at most 3 in the parameters,
// hence of degree at most 3 in each of them; it is the constant c if and only if it
// equals c at every point of {-2,-1,1,3}^k for the k parameters of its family.
// Usage: check7 certificates.txt
#include <algorithm>
#include <array>
#include <cstdio>
#include <fstream>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <vector>

constexpr int NV = 6;
using Aff = std::array<long, NV + 1>;  // c + sum a[1+v] p_v
using Ray = std::array<Aff, 3>;
using Cone = std::array<int, 3>;

struct Fan {
  std::vector<Ray> rays;
  std::vector<Cone> cones;
  int nvars = 0;
  std::string name;
  void normalize() {
    for (auto &c : cones) std::sort(c.begin(), c.end());
    std::sort(cones.begin(), cones.end());
  }
  std::vector<std::pair<int, int>> faces2() const {
    std::set<std::pair<int, int>> s;
    for (auto &c : cones)
      for (int a = 0; a < 3; a++)
        for (int b = a + 1; b < 3; b++) s.insert({c[a], c[b]});
    return {s.begin(), s.end()};
  }
};

static Aff cst(long c) { Aff a{}; a[0] = c; return a; }
static Ray ray(long x, long y, long z) { return {cst(x), cst(y), cst(z)}; }
static Ray add(const Ray &u, const Ray &v) {
  Ray r;
  for (int a = 0; a < 3; a++)
    for (int k = 0; k <= NV; k++) r[a][k] = u[a][k] + v[a][k];
  return r;
}

static Fan blowPoint(const Fan &Y, const Cone &c) {
  Fan X = Y;
  int n = (int)Y.rays.size(), i = c[0], j = c[1], k = c[2];
  X.rays.push_back(add(add(Y.rays[i], Y.rays[j]), Y.rays[k]));
  X.cones.clear();
  for (auto &d : Y.cones) if (d != c) X.cones.push_back(d);
  X.cones.push_back({n, j, k}); X.cones.push_back({i, n, k}); X.cones.push_back({i, j, n});
  X.normalize();
  X.name = Y.name + " pt(" + std::to_string(i) + "," + std::to_string(j) + "," + std::to_string(k) + ")";
  return X;
}
static Fan blowCurve(const Fan &Y, int i, int j) {
  Fan X = Y;
  int n = (int)Y.rays.size();
  X.rays.push_back(add(Y.rays[i], Y.rays[j]));
  X.cones.clear();
  for (auto &d : Y.cones) {
    bool hi = std::count(d.begin(), d.end(), i), hj = std::count(d.begin(), d.end(), j);
    if (hi && hj) {
      int k = -1;
      for (int t : d) if (t != i && t != j) k = t;
      X.cones.push_back({n, j, k}); X.cones.push_back({i, n, k});
    } else X.cones.push_back(d);
  }
  X.normalize();
  X.name = Y.name + " cv(" + std::to_string(i) + "," + std::to_string(j) + ")";
  return X;
}
static std::vector<Fan> blowups(const Fan &Y) {
  std::vector<Fan> out;
  for (auto &c : Y.cones) out.push_back(blowPoint(Y, c));
  for (auto &f : Y.faces2()) out.push_back(blowCurve(Y, f.first, f.second));
  return out;
}

static Fan fanZW(bool w) {
  Fan F;
  F.name = w ? "W" : "Z";
  F.nvars = w ? 2 : 1;
  F.rays = {ray(1, 0, 0), ray(0, 1, 0), ray(-1, -1, 0), ray(0, 0, 1), ray(0, 0, -1)};
  if (w) { F.rays[4][0][1] = 1; F.rays[4][1][2] = 1; }
  else F.rays[2][2][1] = 1;
  for (auto p : std::vector<std::pair<int, int>>{{0, 1}, {1, 2}, {0, 2}}) {
    F.cones.push_back({p.first, p.second, 3});
    F.cones.push_back({p.first, p.second, 4});
  }
  F.normalize();
  return F;
}
// P^1-bundle over the base with rays (x_i, y_i + ya_i * a); ray i gets the twist p_{1+i}
static Fan bundle(std::vector<std::array<long, 3>> base, const std::string &name) {
  Fan F;
  int k = (int)base.size();
  F.name = name; F.nvars = k + 1;
  for (int i = 0; i < k; i++) {
    Ray r = ray(base[i][0], base[i][1], 0);
    r[1][1] = base[i][2];
    r[2][2 + i] = 1;
    F.rays.push_back(r);
  }
  F.rays.push_back(ray(0, 0, 1)); F.rays.push_back(ray(0, 0, -1));
  for (int i = 0; i < k; i++) {
    F.cones.push_back({i, (i + 1) % k, k});
    F.cones.push_back({i, (i + 1) % k, k + 1});
  }
  F.normalize();
  return F;
}
static std::vector<std::array<long, 3>> baseFa() { return {{1, 0, 0}, {0, 1, 0}, {-1, 0, 1}, {0, -1, 0}}; }
static Fan fanG(int j) {
  auto b = baseFa();
  std::vector<std::array<long, 3>> g;
  for (int i = 0; i < 4; i++) {
    g.push_back(b[i]);
    if (i == j) {
      auto &q = b[(i + 1) % 4];
      g.push_back({b[i][0] + q[0], b[i][1] + q[1], b[i][2] + q[2]});
    }
  }
  return bundle(g, "G" + std::to_string(j));
}
static Fan fanC() {
  Fan F;
  F.name = "C"; F.nvars = 1;
  F.rays = {ray(1, 0, 0), ray(0, 1, 0), ray(0, 0, 1), ray(0, -1, 0), ray(0, 0, -1), ray(0, 1, -1), ray(-1, 2, -1)};
  F.rays[3][2][1] = 1;
  F.cones = {{0, 1, 2}, {0, 2, 3}, {0, 3, 4}, {0, 4, 5}, {0, 5, 6}, {0, 1, 6}, {1, 2, 6}, {2, 3, 6}, {3, 4, 6}, {4, 5, 6}};
  F.normalize();
  return F;
}

using V = std::array<long, 3>;
static const long GRID[4] = {-2, -1, 1, 3};
static std::vector<std::vector<V>> evaluations(const Fan &F) {
  int np = 1;
  for (int v = 0; v < F.nvars; v++) np *= 4;
  std::vector<std::vector<V>> out;
  for (int q = 0; q < np; q++) {
    long p[NV] = {0, 0, 0, 0, 0, 0};
    int t = q;
    for (int v = 0; v < F.nvars; v++) { p[v] = GRID[t % 4]; t /= 4; }
    std::vector<V> R;
    for (auto &r : F.rays) {
      V x;
      for (int a = 0; a < 3; a++) {
        long s = r[a][0];
        for (int v = 0; v < NV; v++) s += r[a][1 + v] * p[v];
        x[a] = s;
      }
      R.push_back(x);
    }
    out.push_back(R);
  }
  return out;
}
static long det(const V &u, const V &v, const V &w) {
  return u[0] * (v[1] * w[2] - v[2] * w[1]) - u[1] * (v[0] * w[2] - v[2] * w[0]) + u[2] * (v[0] * w[1] - v[1] * w[0]);
}
static V crs(const V &u, const V &v) { return {u[1] * v[2] - u[2] * v[1], u[2] * v[0] - u[0] * v[2], u[0] * v[1] - u[1] * v[0]}; }

// a quantity given on each grid point; returns true and sets c iff it is constant
template <class Fn> static bool constant(const std::vector<std::vector<V>> &E, Fn f, long &c) {
  c = f(E[0]);
  for (auto &R : E) if (f(R) != c) return false;
  return true;
}

static bool smooth(const Fan &F, const std::vector<std::vector<V>> &E) {
  if (F.rays.size() != 7 || F.cones.size() != 10) return false;
  std::map<std::pair<int, int>, int> cnt;
  for (auto &c : F.cones)
    for (int a = 0; a < 3; a++)
      for (int b = a + 1; b < 3; b++) cnt[{c[a], c[b]}]++;
  for (auto &kv : cnt) if (kv.second != 2) return false;
  for (auto &c : F.cones) {
    long d;
    if (!constant(E, [&](const std::vector<V> &R) { return det(R[c[0]], R[c[1]], R[c[2]]); }, d)) return false;
    if (d != 1 && d != -1) return false;
  }
  return true;
}
static bool checkS(const Fan &F, const std::vector<std::vector<V>> &E, const V &u) {
  for (auto &c : F.cones)
    for (int t = 0; t < 3; t++) {
      long x;
      auto coord = [&](const std::vector<V> &R) {
        std::array<V, 3> M = {R[c[0]], R[c[1]], R[c[2]]};
        long d = det(M[0], M[1], M[2]);
        M[t] = u;
        return d * det(M[0], M[1], M[2]);
      };
      if (!constant(E, coord, x) || x < -1 || x > 1) return false;
    }
  int n = (int)F.rays.size();
  std::vector<std::set<int>> nb(n);
  for (int i = 0; i < n; i++)
    for (int j = i + 1; j < n; j++) {
      long d;
      bool c = constant(E, [&](const std::vector<V> &R) { return det(u, R[i], R[j]); }, d);
      if (!c || d >= 2 || d <= -2) { nb[i].insert(j); nb[j].insert(i); }
    }
  for (int i = 0; i < n; i++)
    for (int j = i + 1; j < n; j++) {
      int com = 0;
      for (int k : nb[i]) com += nb[j].count(k);
      if (com >= 2) return false;
    }
  return true;
}
static bool heights(const Fan &F, const std::vector<std::vector<V>> &E, const V &m, std::vector<long> &h) {
  h.assign(F.rays.size(), 0);
  for (size_t i = 0; i < F.rays.size(); i++)
    if (!constant(E, [&](const std::vector<V> &R) { return m[0] * R[i][0] + m[1] * R[i][1] + m[2] * R[i][2]; }, h[i])) return false;
  return true;
}
static bool checkP(const Fan &F, const std::vector<std::vector<V>> &E, const V &m) {
  std::vector<long> h;
  if (!heights(F, E, m, h)) return false;
  for (long x : h) if (x < -1 || x > 1) return false;
  int k = m[0] ? 0 : (m[1] ? 1 : 2);
  for (auto &f : F.faces2()) {
    if (h[f.first] != 1 || h[f.second] != 1) continue;
    for (auto &g : F.faces2()) {
      if (h[g.first] != -1 || h[g.second] != -1) continue;
      V c0;
      for (int a = 0; a < 3; a++) {
        auto comp = [&](const std::vector<V> &R) {
          V da, db;
          for (int t = 0; t < 3; t++) { da[t] = R[f.first][t] - R[f.second][t]; db[t] = R[g.first][t] - R[g.second][t]; }
          return crs(da, db)[a];
        };
        if (!constant(E, comp, c0[a])) return false;
      }
      if (c0[k] % m[k]) return false;
      long d = c0[k] / m[k];
      for (int a = 0; a < 3; a++) if (c0[a] != d * m[a]) return false;
      if (d < -1 || d > 1) return false;
    }
  }
  return true;
}
static bool checkP2(const Fan &F, const std::vector<std::vector<V>> &E, const V &m) {
  std::vector<long> h;
  if (!heights(F, E, m, h)) return false;
  int neg = 0;
  for (long x : h) {
    if (x > 2 || x < -1) return false;
    neg += (x == -1);
  }
  return neg == 1;
}

int main(int argc, char **argv) {
  if (argc < 2) { std::fprintf(stderr, "usage: %s certificates.txt\n", argv[0]); return 2; }
  std::ifstream in(argv[1]);
  if (!in) { std::perror(argv[1]); return 2; }
  std::map<std::string, std::pair<std::string, V>> table;
  std::string line;
  int rowsRead = 0;
  while (std::getline(in, line)) {
    std::istringstream ss(line);
    std::vector<std::string> t;
    std::string w;
    while (ss >> w) t.push_back(w);
    if (t.size() < 5) continue;
    std::string nm;
    for (size_t i = 0; i + 4 < t.size(); i++) nm += (i ? " " : "") + t[i];
    V v = {std::stol(t[t.size() - 3]), std::stol(t[t.size() - 2]), std::stol(t[t.size() - 1])};
    table[nm] = {t[t.size() - 4], v};
    rowsRead++;
  }
  std::vector<Fan> all;
  for (bool w : {false, true})
    for (auto &Y : blowups(fanZW(w)))
      for (auto &X : blowups(Y)) all.push_back(X);
  for (auto &X : blowups(bundle(baseFa(), "F"))) all.push_back(X);
  for (int j = 0; j < 4; j++) all.push_back(fanG(j));
  all.push_back(fanC());
  int good = 0;
  std::set<std::string> seen;
  for (auto &X : all) {
    auto it = table.find(X.name);
    if (it == table.end() || !seen.insert(X.name).second) { std::fprintf(stderr, "missing or repeated %s\n", X.name.c_str()); continue; }
    auto E = evaluations(X);
    bool ok = smooth(X, E);
    const auto &kind = it->second.first;
    const V &v = it->second.second;
    ok = ok && (kind == "S" ? checkS(X, E, v) : kind == "P" ? checkP(X, E, v) : kind == "P2" ? checkP2(X, E, v) : false);
    if (!ok) std::fprintf(stderr, "certificate fails: %s\n", X.name.c_str());
    good += ok;
  }
  std::printf("C++ checker: %d of %zu fans verified, %d table rows\n", good, all.size(), rowsRead);
  return (good == (int)all.size() && rowsRead == (int)all.size() && (int)table.size() == rowsRead) ? 0 : 1;
}
