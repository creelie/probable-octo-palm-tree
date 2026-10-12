import sys, re, numpy as np
sys.path.insert(0,'.')
from validate import parse, latpts_H
for line in open(sys.argv[1]):
    main,q=line.rsplit(' | q ',1)
    A,c,cones=parse(main.split(' | ',1)[1])
    q=np.array(list(map(int,q.split())))
    P=latpts_H(A,c); Q=latpts_H(A,q); PQ=latpts_H(A,c+q)
    S={tuple(p+x) for p in P for x in Q}
    miss=[tuple(m) for m in PQ if tuple(m) not in S]
    smooth=all(abs(round(np.linalg.det(A[list(s)])))==1 for s in cones)
    print(main.split('|')[0].strip(),'py:',len(P),'Q',len(Q),'P+Q',len(PQ),'missing',len(miss),'smooth',smooth)
