/* Independent check of data/certificates.txt.
 * Rays are affine in at most six integer parameters, so every quantity checked below
 * (cone determinants, coordinates, the determinants det(u, r_i, r_j), the cross
 * products of Proposition 7 of the paper and the heights) is a polynomial of total degree at most 3.
 * Such a polynomial has degree at most 3 in each variable, so it is the constant c
 * if and only if it takes the value c at every point of the grid {0,1,2,3}^k of the
 * parameters it involves.  This program evaluates on that grid.
 * Usage: check7 certificates.txt */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NV 6
#define MAXR 8
#define MAXC 12
typedef struct { long r[MAXR][3][NV + 1]; int n; int c[MAXC][3]; int nc; int nvars; char name[96]; } Fan;

static void setray(Fan *F, int i, long x, long y, long z) {
    memset(F->r[i], 0, sizeof F->r[i]);
    F->r[i][0][0] = x; F->r[i][1][0] = y; F->r[i][2][0] = z;
}
static void sort3(int *a) { int t;
    if (a[0] > a[1]) { t = a[0]; a[0] = a[1]; a[1] = t; }
    if (a[1] > a[2]) { t = a[1]; a[1] = a[2]; a[2] = t; }
    if (a[0] > a[1]) { t = a[0]; a[0] = a[1]; a[1] = t; } }
static void addcone(Fan *F, int i, int j, int k) { int *c = F->c[F->nc++]; c[0] = i; c[1] = j; c[2] = k; sort3(c); }
static int cmpc(const void *a, const void *b) {
    const int *x = a, *y = b;
    for (int i = 0; i < 3; i++)
        if (x[i] != y[i]) return x[i] - y[i];
    return 0;
}
static void sortcones(Fan *F) { qsort(F->c, F->nc, sizeof F->c[0], cmpc); }

static int faces2(const Fan *F, int f[][2]) {
    int n = 0;
    for (int i = 0; i < F->nc; i++)
        for (int a = 0; a < 3; a++) for (int b = a + 1; b < 3; b++) {
            int x = F->c[i][a], y = F->c[i][b], seen = 0;
            for (int t = 0; t < n; t++) if (f[t][0] == x && f[t][1] == y) seen = 1;
            if (!seen) { f[n][0] = x; f[n][1] = y; n++; }
        }
    /* sort lexicographically */
    for (int i = 0; i < n; i++) for (int j = i + 1; j < n; j++)
        if (f[j][0] < f[i][0] || (f[j][0] == f[i][0] && f[j][1] < f[i][1])) {
            int t0 = f[i][0], t1 = f[i][1]; f[i][0] = f[j][0]; f[i][1] = f[j][1]; f[j][0] = t0; f[j][1] = t1; }
    return n;
}
static void blow_point(const Fan *Y, int ci, Fan *X) {
    *X = *Y; int i = Y->c[ci][0], j = Y->c[ci][1], k = Y->c[ci][2], n = Y->n;
    for (int a = 0; a < 3; a++) for (int v = 0; v <= NV; v++)
        X->r[n][a][v] = Y->r[i][a][v] + Y->r[j][a][v] + Y->r[k][a][v];
    X->n = n + 1; X->nc = 0;
    for (int t = 0; t < Y->nc; t++) if (t != ci) addcone(X, Y->c[t][0], Y->c[t][1], Y->c[t][2]);
    addcone(X, n, j, k); addcone(X, i, n, k); addcone(X, i, j, n); sortcones(X);
    snprintf(X->name, sizeof X->name, "%.64s pt(%d,%d,%d)", Y->name, i, j, k);
}
static void blow_curve(const Fan *Y, int i, int j, Fan *X) {
    *X = *Y; int n = Y->n;
    for (int a = 0; a < 3; a++) for (int v = 0; v <= NV; v++)
        X->r[n][a][v] = Y->r[i][a][v] + Y->r[j][a][v];
    X->n = n + 1; X->nc = 0;
    for (int t = 0; t < Y->nc; t++) {
        const int *c = Y->c[t]; int hi = 0, hj = 0, k = -1;
        for (int a = 0; a < 3; a++) { if (c[a] == i) hi = 1; else if (c[a] == j) hj = 1; else k = c[a]; }
        if (hi && hj) { addcone(X, n, j, k); addcone(X, i, n, k); }
        else addcone(X, c[0], c[1], c[2]);
    }
    sortcones(X);
    snprintf(X->name, sizeof X->name, "%.64s cv(%d,%d)", Y->name, i, j);
}
static int blowups(const Fan *Y, Fan *out) {
    int m = 0, f[64][2], nf = faces2(Y, f);
    for (int c = 0; c < Y->nc; c++) blow_point(Y, c, &out[m++]);
    for (int t = 0; t < nf; t++) blow_curve(Y, f[t][0], f[t][1], &out[m++]);
    return m;
}
/* minimal fans */
static void fanZ(Fan *F) { memset(F, 0, sizeof *F); F->n = 5; F->nvars = 1; strcpy(F->name, "Z");
    setray(F, 0, 1, 0, 0); setray(F, 1, 0, 1, 0); setray(F, 2, -1, -1, 0); F->r[2][2][1] = 1;
    setray(F, 3, 0, 0, 1); setray(F, 4, 0, 0, -1);
    int p[3][2] = {{0, 1}, {1, 2}, {0, 2}};
    for (int t = 0; t < 3; t++) { addcone(F, p[t][0], p[t][1], 3); addcone(F, p[t][0], p[t][1], 4); }
    sortcones(F); }
static void fanW(Fan *F) { memset(F, 0, sizeof *F); F->n = 5; F->nvars = 2; strcpy(F->name, "W");
    setray(F, 0, 1, 0, 0); setray(F, 1, 0, 1, 0); setray(F, 2, -1, -1, 0);
    setray(F, 3, 0, 0, 1); setray(F, 4, 0, 0, -1); F->r[4][0][1] = 1; F->r[4][1][2] = 1;
    int p[3][2] = {{0, 1}, {1, 2}, {0, 2}};
    for (int t = 0; t < 3; t++) { addcone(F, p[t][0], p[t][1], 3); addcone(F, p[t][0], p[t][1], 4); }
    sortcones(F); }
/* P^1-bundle over a base with k rays (x_i, y_i), y_i = y0[i] + a * ya[i]; twist of ray i is p_{1+i} */
static void bundle(Fan *F, int k, const long *x, const long *y0, const long *ya, const char *name) {
    memset(F, 0, sizeof *F); F->n = k + 2; F->nvars = k + 1; strcpy(F->name, name);
    for (int i = 0; i < k; i++) { setray(F, i, x[i], y0[i], 0); F->r[i][1][1] = ya[i]; F->r[i][2][2 + i] = 1; }
    setray(F, k, 0, 0, 1); setray(F, k + 1, 0, 0, -1);
    for (int i = 0; i < k; i++) { addcone(F, i, (i + 1) % k, k); addcone(F, i, (i + 1) % k, k + 1); }
    sortcones(F); }
static void fanF(Fan *F) { long x[4] = {1, 0, -1, 0}, y0[4] = {0, 1, 0, -1}, ya[4] = {0, 0, 1, 0}; bundle(F, 4, x, y0, ya, "F"); }
static void fanG(Fan *F, int j) {
    long x[4] = {1, 0, -1, 0}, y0[4] = {0, 1, 0, -1}, ya[4] = {0, 0, 1, 0}, X5[5], Y5[5], A5[5]; int m = 0;
    for (int i = 0; i < 4; i++) {
        X5[m] = x[i]; Y5[m] = y0[i]; A5[m] = ya[i]; m++;
        if (i == j) { int q = (i + 1) % 4; X5[m] = x[i] + x[q]; Y5[m] = y0[i] + y0[q]; A5[m] = ya[i] + ya[q]; m++; }
    }
    char nm[8]; snprintf(nm, sizeof nm, "G%d", j); bundle(F, 5, X5, Y5, A5, nm); }
static void fanC(Fan *F) { memset(F, 0, sizeof *F); F->n = 7; F->nvars = 1; strcpy(F->name, "C");
    setray(F, 0, 1, 0, 0); setray(F, 1, 0, 1, 0); setray(F, 2, 0, 0, 1); setray(F, 3, 0, -1, 0); F->r[3][2][1] = 1;
    setray(F, 4, 0, 0, -1); setray(F, 5, 0, 1, -1); setray(F, 6, -1, 2, -1);
    int c[10][3] = {{0,1,2},{0,2,3},{0,3,4},{0,4,5},{0,5,6},{0,1,6},{1,2,6},{2,3,6},{3,4,6},{4,5,6}};
    for (int t = 0; t < 10; t++) addcone(F, c[t][0], c[t][1], c[t][2]);
    sortcones(F); }

/* grid evaluation */
static long GRIDV[4] = {0, 1, 2, 3};
static int npts(const Fan *F) { int p = 1; for (int v = 0; v < F->nvars; v++) p *= 4; return p; }
static void point(const Fan *F, int idx, long *p) {
    for (int v = 0; v < NV; v++) p[v] = 0;
    for (int v = 0; v < F->nvars; v++) { p[v] = GRIDV[idx % 4]; idx /= 4; } }
static void evalrays(const Fan *F, const long *p, long R[][3]) {
    for (int i = 0; i < F->n; i++) for (int a = 0; a < 3; a++) {
        long s = F->r[i][a][0];
        for (int v = 0; v < NV; v++) s += F->r[i][a][1 + v] * p[v];
        R[i][a] = s; } }
static long det3(const long *u, const long *v, const long *w) {
    return u[0] * (v[1] * w[2] - v[2] * w[1]) - u[1] * (v[0] * w[2] - v[2] * w[0]) + u[2] * (v[0] * w[1] - v[1] * w[0]); }

/* value[k][pt] tables: a quantity is constant iff all grid values agree */
static int smooth(const Fan *F) {
    if (F->n != 7 || F->nc != 10) return 0;
    int f[64][2], nf = faces2(F, f);
    for (int t = 0; t < nf; t++) { int cnt = 0;
        for (int c = 0; c < F->nc; c++) { int hi = 0, hj = 0;
            for (int a = 0; a < 3; a++) { if (F->c[c][a] == f[t][0]) hi = 1; if (F->c[c][a] == f[t][1]) hj = 1; }
            cnt += hi && hj; }
        if (cnt != 2) return 0; }
    long p[NV], R[MAXR][3];
    for (int c = 0; c < F->nc; c++) { long d0 = 0;
        for (int q = 0; q < npts(F); q++) { point(F, q, p); evalrays(F, p, R);
            long d = det3(R[F->c[c][0]], R[F->c[c][1]], R[F->c[c][2]]);
            if (q == 0) d0 = d; else if (d != d0) return 0; }
        if (d0 != 1 && d0 != -1) return 0; }
    return 1; }

static int checkS(const Fan *F, const long *u) {
    long p[NV], R[MAXR][3];
    for (int c = 0; c < F->nc; c++) {
        long x0[3] = {0, 0, 0};
        for (int q = 0; q < npts(F); q++) { point(F, q, p); evalrays(F, p, R);
            const long *a = R[F->c[c][0]], *b = R[F->c[c][1]], *e = R[F->c[c][2]];
            long d = det3(a, b, e), x[3] = {d * det3(u, b, e), d * det3(a, u, e), d * det3(a, b, u)};
            for (int t = 0; t < 3; t++) {
                if (x[t] < -1 || x[t] > 1) return 0;
                if (q == 0) x0[t] = x[t]; else if (x[t] != x0[t]) return 0; }
            for (int t = 0; t < 3; t++) if (x[0] * a[t] + x[1] * b[t] + x[2] * e[t] != u[t]) return 0; } }
    int n = F->n, E[MAXR][MAXR];
    for (int i = 0; i < n; i++) for (int j = 0; j < n; j++) E[i][j] = 0;
    for (int i = 0; i < n; i++) for (int j = i + 1; j < n; j++) { long d0 = 0; int cst = 1;
        for (int q = 0; q < npts(F); q++) { point(F, q, p); evalrays(F, p, R);
            long d = det3(u, R[i], R[j]); if (q == 0) d0 = d; else if (d != d0) cst = 0; }
        if (!cst || d0 >= 2 || d0 <= -2) E[i][j] = E[j][i] = 1; }
    for (int i = 0; i < n; i++) for (int j = i + 1; j < n; j++) { int com = 0;
        for (int k = 0; k < n; k++) com += E[i][k] && E[j][k];
        if (com >= 2) return 0; }
    return 1; }

static int heights(const Fan *F, const long *m, long *h) {
    long p[NV], R[MAXR][3];
    for (int q = 0; q < npts(F); q++) { point(F, q, p); evalrays(F, p, R);
        for (int i = 0; i < F->n; i++) { long s = m[0] * R[i][0] + m[1] * R[i][1] + m[2] * R[i][2];
            if (q == 0) h[i] = s; else if (s != h[i]) return 0; } }
    return 1; }
static int checkP(const Fan *F, const long *m) {
    long h[MAXR]; if (!heights(F, m, h)) return 0;
    for (int i = 0; i < F->n; i++) if (h[i] < -1 || h[i] > 1) return 0;
    int f[64][2], nf = faces2(F, f), k = m[0] ? 0 : (m[1] ? 1 : 2);
    long p[NV], R[MAXR][3];
    for (int s = 0; s < nf; s++) { if (h[f[s][0]] != 1 || h[f[s][1]] != 1) continue;
        for (int t = 0; t < nf; t++) { if (h[f[t][0]] != -1 || h[f[t][1]] != -1) continue;
            long c0[3] = {0, 0, 0};
            for (int q = 0; q < npts(F); q++) { point(F, q, p); evalrays(F, p, R);
                long da[3], db[3], c[3];
                for (int a = 0; a < 3; a++) { da[a] = R[f[s][0]][a] - R[f[s][1]][a]; db[a] = R[f[t][0]][a] - R[f[t][1]][a]; }
                c[0] = da[1] * db[2] - da[2] * db[1]; c[1] = da[2] * db[0] - da[0] * db[2]; c[2] = da[0] * db[1] - da[1] * db[0];
                for (int a = 0; a < 3; a++) { if (q == 0) c0[a] = c[a]; else if (c[a] != c0[a]) return 0; } }
            if (c0[k] % m[k]) return 0;
            long d = c0[k] / m[k];
            for (int a = 0; a < 3; a++) if (c0[a] != d * m[a]) return 0;
            if (d < -1 || d > 1) return 0; } }
    return 1; }
static int checkP2(const Fan *F, const long *m) {
    long h[MAXR]; if (!heights(F, m, h)) return 0;
    int neg = 0;
    for (int i = 0; i < F->n; i++) { if (h[i] > 2 || h[i] < -1) return 0; if (h[i] == -1) neg++; }
    return neg == 1; }

typedef struct { char name[96]; char kind[4]; long v[3]; int used; } Row;
static Row rows[1000]; static int nrows = 0;

static int verify(const Fan *X) {
    for (int i = 0; i < nrows; i++) if (!strcmp(rows[i].name, X->name)) {
        if (rows[i].used) { fprintf(stderr, "duplicate %s\n", X->name); return 0; }
        rows[i].used = 1;
        if (!smooth(X)) { fprintf(stderr, "not smooth: %s\n", X->name); return 0; }
        int ok = !strcmp(rows[i].kind, "S") ? checkS(X, rows[i].v) : !strcmp(rows[i].kind, "P") ? checkP(X, rows[i].v)
               : !strcmp(rows[i].kind, "P2") ? checkP2(X, rows[i].v) : 0;
        if (!ok) fprintf(stderr, "certificate fails: %s\n", X->name);
        return ok; }
    fprintf(stderr, "no row for %s\n", X->name); return 0; }

int main(int argc, char **argv) {
    if (argc < 2) { fprintf(stderr, "usage: %s certificates.txt\n", argv[0]); return 2; }
    FILE *fp = fopen(argv[1], "r"); if (!fp) { perror(argv[1]); return 2; }
    char line[256];
    while (fgets(line, sizeof line, fp)) {
        char *tok[8]; int nt = 0; char *s = strtok(line, " \n");
        while (s && nt < 8) { tok[nt++] = s; s = strtok(NULL, " \n"); }
        if (nt < 5) continue;
        Row *r = &rows[nrows++]; r->name[0] = 0;
        for (int i = 0; i < nt - 4; i++) { if (i) strcat(r->name, " "); strcat(r->name, tok[i]); }
        strcpy(r->kind, tok[nt - 4]); for (int i = 0; i < 3; i++) r->v[i] = atol(tok[nt - 3 + i]); r->used = 0; }
    fclose(fp);
    static Fan A[64], B[64]; Fan M; int good = 0, total = 0;
    for (int fam = 0; fam < 2; fam++) {
        if (fam == 0) fanZ(&M); else fanW(&M);
        int na = blowups(&M, A);
        for (int i = 0; i < na; i++) { int nb = blowups(&A[i], B);
            for (int j = 0; j < nb; j++) { total++; good += verify(&B[j]); } } }
    fanF(&M); { int na = blowups(&M, A); for (int i = 0; i < na; i++) { total++; good += verify(&A[i]); } }
    for (int j = 0; j < 4; j++) { fanG(&M, j); total++; good += verify(&M); }
    fanC(&M); total++; good += verify(&M);
    int unused = 0; for (int i = 0; i < nrows; i++) if (!rows[i].used) { unused++; fprintf(stderr, "unused row %s\n", rows[i].name); }
    printf("C checker: %d of %d fans verified, %d table rows, %d unused\n", good, total, nrows, unused);
    return (good == total && total == nrows && unused == 0) ? 0 : 1;
}
