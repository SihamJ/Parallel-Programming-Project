#!/usr/bin/env python3

import sys
import os
import subprocess
import time
import re

EXTENSION="txt"
INDIR="input/"
OUTDIR="out/"
# OUTDIR="best/"

# lit le score d'après le nom du fichier lié
def readscore(fname):
    try:
        # descend jusqu'au dernier lien
        while os.path.islink(fname):
            fname = os.path.join(os.path.dirname(fname), os.readlink(fname))
        s = int(re.split("\.",os.path.basename(fname))[1])
    except:
        return 0
    return s

# list of benchmarks : files ending with "."+EXTENSION in the "input/" directory
lbench = []
for f in os.listdir(INDIR):
        bname,n=re.subn(r"(.*)\."+EXTENSION+"$",r"\1",f)
        if n==1 :
                lbench += [bname]
lbench.sort()

# read args
if len(sys.argv)!=1:
    print("Usage: ", sys.argv[0], file=sys.stderr)
    sys.exit(1)

total = 0
for b in lbench:
    infile = INDIR+b+"."+EXTENSION

    best=OUTDIR + b + "_best.out"
    score = readscore(best)
    print(b+":", score)
    total += score

print("Total score:", total)
exit(0)
