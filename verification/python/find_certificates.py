"""Find a certificate for every fan of the classification and write data/certificates.txt."""
import os, sys
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from fans7 import all_fans, smooth, CHECK, directions

def main(out):
    D = directions(2)
    lines = []
    for X in all_fans():
        assert smooth(X), X.name
        cert = None
        for kind in ("S", "P", "P2"):
            for v in D:
                if CHECK[kind](X, v):
                    cert = (kind, v)
                    break
            if cert:
                break
        assert cert, "no certificate for " + X.name
        lines.append("%s %s %d %d %d" % (X.name, cert[0], *cert[1]))
    with open(out, "w") as f:
        f.write("\n".join(lines) + "\n")
    print(len(lines), "certificates written to", out)

if __name__ == "__main__":
    main(sys.argv[1])
