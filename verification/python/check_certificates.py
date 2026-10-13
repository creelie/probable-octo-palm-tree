"""Check data/certificates.txt: every fan of the classification has a row, and every row's
certificate holds as a polynomial identity in the parameters.
Usage: python3 check_certificates.py certificates.txt"""
import os, sys
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from fans7 import all_fans, smooth, CHECK

def main(path):
    table = {}
    nrows = 0
    for line in open(path):
        t = line.split()
        if len(t) < 5:
            continue
        table[" ".join(t[:-4])] = (t[-4], tuple(int(x) for x in t[-3:]))
        nrows += 1
    fans = all_fans()
    good = 0
    seen = set()
    for X in fans:
        if X.name not in table or X.name in seen:
            print("missing or repeated", X.name, file=sys.stderr)
            continue
        seen.add(X.name)
        kind, v = table[X.name]
        ok = smooth(X) and kind in CHECK and CHECK[kind](X, v)
        if not ok:
            print("certificate fails:", X.name, file=sys.stderr)
        good += ok
    print("Python checker: %d of %d fans verified, %d table rows" % (good, len(fans), nrows))
    return 0 if good == len(fans) == nrows == len(table) else 1

if __name__ == "__main__":
    sys.exit(main(sys.argv[1]))
