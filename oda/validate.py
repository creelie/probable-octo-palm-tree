import sys, re, itertools
import numpy as np
from fractions import Fraction

def parse(line):
    rays=[tuple(map(int,x.split(','))) for x in re.findall(r'\(([-\d,]+)\)', line.split(' c ')[0])]
    c=list(map(int,line.split(' c ')[1].split(' cones ')[0].split()))
    cones=[tuple(map(int,x.split('.'))) for x in line.split(' cones ')[1].split()]
    return np.array(rays),np.array(c),cones

def latpts_H(A,b):
    # points m with A m >= -b ; bounding box by brute force from vertices of all D-subsets
    D=A.shape[1]
    verts=[]
    for S in itertools.combinations(range(len(A)),D):
        M=A[list(S)]
        if abs(round(np.linalg.det(M)))==0: continue
        x=np.linalg.solve(M,-b[list(S)])
        if np.all(A@x >= -b-1e-9): verts.append(x)
    V=np.array(verts)
    lo=np.floor(V.min(0)+1e-9).astype(int); hi=np.ceil(V.max(0)-1e-9).astype(int)
    grids=np.meshgrid(*[np.arange(l,h+1) for l,h in zip(lo,hi)],indexing='ij')
    pts=np.stack([g.ravel() for g in grids],1)
    ok=np.all(pts@A.T >= -b,1)
    return pts[ok]

def idp2(A,b):
    P=latpts_H(A,b); P2=latpts_H(A,2*b)
    S=set()
    for i in range(len(P)):
        for j in range(i,len(P)):
            S.add(tuple(P[i]+P[j]))
    missing=[tuple(m) for m in P2 if tuple(m) not in S]
    return len(P),len(P2),missing

if __name__=='__main__' and sys.argv[1]=='reeve':
    # Reeve tetrahedron conv{0,e1,e2,(1,1,r)} in H-form computed from facets
    r=int(sys.argv[2])
    V=np.array([[0,0,0],[1,0,0],[0,1,0],[1,1,r]])
    A=[];b=[]
    for S in itertools.combinations(range(4),3):
        p=V[list(S)]; n=np.cross(p[1]-p[0],p[2]-p[0]); o=[k for k in range(4) if k not in S][0]
        if n@(V[o]-p[0])<0: n=-n
        g=np.gcd.reduce(np.abs(n)); n=n//g
        A.append(n); b.append(-(n@p[0]))
    print(idp2(np.array(A),np.array(b)))
elif __name__=='__main__':
    for line in open(sys.argv[1]):
        A,c,cones=parse(line)
        smooth=all(abs(round(np.linalg.det(A[list(s)])))==1 for s in cones)
        npts,n2,miss=idp2(A,c)
        print(line.split('|')[0].strip(), 'py_npts',npts,'smooth',smooth,'2P',n2,'missing',len(miss))
