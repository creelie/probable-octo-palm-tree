# Independent check of data/certificates.txt by exact polynomial arithmetic.
# Polynomials in the parameters p1, ..., p6 are dictionaries from exponent vectors to
# integers; a certificate is checked as polynomial identities, so it holds for every
# integer value of the parameters.
# Usage: julia check7.jl certificates.txt

const NV = 6
const Poly = Dict{NTuple{NV,Int},BigInt}
const ZERO = ntuple(_ -> 0, NV)

cst(c) = c == 0 ? Poly() : Poly(ZERO => BigInt(c))
function var(i)
    e = collect(ZERO); e[i] = 1
    Poly(Tuple(e) => BigInt(1))
end
function padd(ps...)
    r = Poly()
    for p in ps, (e, c) in p
        r[e] = get(r, e, BigInt(0)) + c
    end
    filter!(kv -> kv[2] != 0, r)
end
pscale(k, p) = filter!(kv -> kv[2] != 0, Poly(e => k * c for (e, c) in p))
function pmul(p, q)
    r = Poly()
    for (e1, c1) in p, (e2, c2) in q
        e = e1 .+ e2
        r[e] = get(r, e, BigInt(0)) + c1 * c2
    end
    filter!(kv -> kv[2] != 0, r)
end
psub(p, q) = padd(p, pscale(-1, q))
isconst(p) = all(e == ZERO for e in keys(p))
cval(p) = get(p, ZERO, BigInt(0))

const Vec = NTuple{3,Poly}
vec3(x, y, z) = (x isa Poly ? x : cst(x), y isa Poly ? y : cst(y), z isa Poly ? z : cst(z))
vadd(vs...) = Tuple(padd((v[a] for v in vs)...) for a in 1:3)
vsub(u, v) = Tuple(psub(u[a], v[a]) for a in 1:3)
det3(u, v, w) = padd(pmul(u[1], psub(pmul(v[2], w[3]), pmul(v[3], w[2]))),
                     pscale(-1, pmul(u[2], psub(pmul(v[1], w[3]), pmul(v[3], w[1])))),
                     pmul(u[3], psub(pmul(v[1], w[2]), pmul(v[2], w[1]))))
crossp(u, v) = (psub(pmul(u[2], v[3]), pmul(u[3], v[2])),
                psub(pmul(u[3], v[1]), pmul(u[1], v[3])),
                psub(pmul(u[1], v[2]), pmul(u[2], v[1])))
dotc(m, v) = padd((pscale(m[a], v[a]) for a in 1:3)...)

mutable struct Fan
    rays::Vector{Vec}
    cones::Vector{NTuple{3,Int}}   # 0-based ray indices, sorted
    name::String
end
srt(t) = Tuple(sort(collect(t)))
normalize!(F) = (F.cones = sort([srt(c) for c in F.cones]); F)
faces2(F) = sort(unique([(c[i], c[j]) for c in F.cones for (i, j) in ((1, 2), (1, 3), (2, 3))]))
function blowpoint(Y, c)
    i, j, k = c; n = length(Y.rays)
    w = vadd(Y.rays[i+1], Y.rays[j+1], Y.rays[k+1])
    cones = [x for x in Y.cones if x != c]
    append!(cones, [(n, j, k), (i, n, k), (i, j, n)])
    normalize!(Fan(vcat(Y.rays, [w]), cones, Y.name * " pt($i,$j,$k)"))
end
function blowcurve(Y, f)
    i, j = f; n = length(Y.rays)
    w = vadd(Y.rays[i+1], Y.rays[j+1])
    cones = NTuple{3,Int}[]
    for x in Y.cones
        if i in x && j in x
            k = first(t for t in x if t != i && t != j)
            push!(cones, (n, j, k), (i, n, k))
        else
            push!(cones, x)
        end
    end
    normalize!(Fan(vcat(Y.rays, [w]), cones, Y.name * " cv($i,$j)"))
end
blowups(Y) = vcat([blowpoint(Y, c) for c in Y.cones], [blowcurve(Y, f) for f in faces2(Y)])

const P1 = var(1); const P2v = var(2)
basecones = [(0, 1, 3), (0, 1, 4), (1, 2, 3), (1, 2, 4), (0, 2, 3), (0, 2, 4)]
fanZ() = normalize!(Fan([vec3(1, 0, 0), vec3(0, 1, 0), vec3(-1, -1, P1), vec3(0, 0, 1), vec3(0, 0, -1)], copy(basecones), "Z"))
fanW() = normalize!(Fan([vec3(1, 0, 0), vec3(0, 1, 0), vec3(-1, -1, 0), vec3(0, 0, 1), vec3(P1, P2v, -1)], copy(basecones), "W"))
const BASE_FA = [(cst(1), cst(0)), (cst(0), cst(1)), (cst(-1), P1), (cst(0), cst(-1))]
function bundle(base, name)
    k = length(base)
    rays = [vec3(base[i][1], base[i][2], var(1 + i)) for i in 1:k]
    push!(rays, vec3(0, 0, 1), vec3(0, 0, -1))
    cones = NTuple{3,Int}[]
    for i in 0:k-1
        push!(cones, (i, mod(i + 1, k), k), (i, mod(i + 1, k), k + 1))
    end
    normalize!(Fan(rays, cones, name))
end
function fanG(j)
    b = BASE_FA; q = b[mod(j + 1, 4)+1]
    new = (padd(b[j+1][1], q[1]), padd(b[j+1][2], q[2]))
    bundle(vcat(b[1:j+1], [new], b[j+2:end]), "G$j")
end
fanC() = normalize!(Fan([vec3(1, 0, 0), vec3(0, 1, 0), vec3(0, 0, 1), vec3(0, -1, P1), vec3(0, 0, -1), vec3(0, 1, -1), vec3(-1, 2, -1)],
    [(0, 1, 2), (0, 2, 3), (0, 3, 4), (0, 4, 5), (0, 5, 6), (0, 1, 6), (1, 2, 6), (2, 3, 6), (3, 4, 6), (4, 5, 6)], "C"))

function smooth(X)
    (length(X.rays) == 7 && length(X.cones) == 10) || return false
    cnt = Dict{Tuple{Int,Int},Int}()
    for c in X.cones, (i, j) in ((1, 2), (1, 3), (2, 3))
        cnt[(c[i], c[j])] = get(cnt, (c[i], c[j]), 0) + 1
    end
    all(==(2), values(cnt)) || return false
    for c in X.cones
        d = det3(X.rays[c[1]+1], X.rays[c[2]+1], X.rays[c[3]+1])
        (isconst(d) && abs(cval(d)) == 1) || return false
    end
    true
end
function checkS(X, u)
    U = vec3(u...)
    for c in X.cones
        R = [X.rays[i+1] for i in c]
        d = cval(det3(R...))
        xs = BigInt[]
        for t in 1:3
            M = copy(R); M[t] = U
            x = pscale(d, det3(M...))
            (isconst(x) && abs(cval(x)) <= 1) || return false
            push!(xs, cval(x))
        end
        s = vadd((Tuple(pscale(xs[t], R[t][a]) for a in 1:3) for t in 1:3)...)
        all(isempty(psub(s[a], cst(u[a]))) for a in 1:3) || return false
    end
    n = length(X.rays)
    nb = [Set{Int}() for _ in 1:n]
    for i in 1:n, j in i+1:n
        d = det3(U, X.rays[i], X.rays[j])
        if !isconst(d) || abs(cval(d)) >= 2
            push!(nb[i], j); push!(nb[j], i)
        end
    end
    all(length(intersect(nb[i], nb[j])) <= 1 for i in 1:n for j in i+1:n)
end
function heights(X, m)
    h = [dotc(m, r) for r in X.rays]
    all(isconst, h) ? [cval(x) for x in h] : nothing
end
function checkP(X, m)
    h = heights(X, m)
    (h === nothing || any(abs(x) > 1 for x in h)) && return false
    k = findfirst(!=(0), m)
    for f in faces2(X), g in faces2(X)
        (h[f[1]+1] == 1 && h[f[2]+1] == 1 && h[g[1]+1] == -1 && h[g[2]+1] == -1) || continue
        c = crossp(vsub(X.rays[f[1]+1], X.rays[f[2]+1]), vsub(X.rays[g[1]+1], X.rays[g[2]+1]))
        all(isconst, c) || return false
        cv = [cval(t) for t in c]
        cv[k] % m[k] == 0 || return false
        d = div(cv[k], m[k])
        (all(cv[a] == d * m[a] for a in 1:3) && abs(d) <= 1) || return false
    end
    true
end
function checkP2(X, m)
    h = heights(X, m)
    h === nothing && return false
    all(-1 <= x <= 2 for x in h) && count(==(-1), h) == 1
end

function main(path)
    table = Dict{String,Tuple{String,Vector{Int}}}()
    nrows = 0
    for line in eachline(path)
        t = split(line)
        length(t) < 5 && continue
        table[join(t[1:end-4], " ")] = (String(t[end-3]), parse.(Int, t[end-2:end]))
        nrows += 1
    end
    fans = Fan[]
    for M in (fanZ(), fanW()), Y in blowups(M)
        append!(fans, blowups(Y))
    end
    append!(fans, blowups(bundle(BASE_FA, "F")))
    append!(fans, [fanG(j) for j in 0:3])
    push!(fans, fanC())
    good = 0
    seen = Set{String}()
    for X in fans
        if !haskey(table, X.name) || X.name in seen
            println(stderr, "missing or repeated ", X.name); continue
        end
        push!(seen, X.name)
        kind, v = table[X.name]
        ok = smooth(X) && (kind == "S" ? checkS(X, v) : kind == "P" ? checkP(X, v) : kind == "P2" ? checkP2(X, v) : false)
        ok || println(stderr, "certificate fails: ", X.name)
        good += ok
    end
    println("Julia checker: $good of $(length(fans)) fans verified, $nrows table rows")
    exit(good == length(fans) == nrows == length(table) ? 0 : 1)
end

main(ARGS[1])
