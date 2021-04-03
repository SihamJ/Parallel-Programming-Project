#!/usr/bin/env python3

# pour batcher en mpi :
# mpirun -n __ -hostfile host__ ./batch_vin.py d 1000000000

import sys
import os
import subprocess
import time
import re
import random

# argv[1] = b        = benchmark name
# argv[2] = show = print the score if it is greater than this walue

EXTENSION="txt"

# list of benchmarks : files ending with "."+EXTENSION in the "input/" directory
lbench = []
for f in os.listdir("input/"):
        bname,n=re.subn(r"(.*)\."+EXTENSION+"$",r"\1",f)
        if n==1 :
                lbench += [bname]
lbench.sort()

# read args
b = None
if len(sys.argv)>1:
    for bn in lbench:
        if re.match(sys.argv[1],bn):
            b = bn
if b==None:
    # print( "Usage: ", sys.argv[0], lbench, "[<min score to print>]")
    # sys.exit(1)
    b = random.choice(lbench)
    sys.argv.insert(1, b)

print("benching:", b, "on", os.uname().nodename)

if len(sys.argv)>=3:
    try:
        show = int(sys.argv[2])
    except:
        show = 0
else:
    show = 0

# use "/tmp/HC/<benchname>/" for speed (final '/' needed !)
tmpdir="/tmp/HC/"+b+"/"
subprocess.check_call( ["mkdir", "-p", tmpdir] )

infile = "input/"+b+"."+EXTENSION
# must be in the "out/" directory !
seed="out/" + b + "_best.out"
# hostname_pid
pid = os.uname().nodename + "_" + str(os.getpid()) + "_"


def lock_score(f):
#   print(".",end="",flush=True)
    try:
        if os.access(f, os.R_OK):
            return False
        fd = open(f,"w")
        fd.write(pid)
        fd.close()
        os.sync()
        # check that I wrote correctly my id
        fd = open(f,"r")
        w = fd.read()
        fd.close()
        if w != pid:
            return False
    except:
        try:
            fd.close()
        except:
            pass
        return False
    # si tout s'est bien passé, j'ai le lock
    return True

def unlock_score(f):
#    print("unlocking.")
    try:
        os.unlink(f)
    except:
        pass

# recalcule le score du fichier best out existant
def computescore(fname):
    scorefile = fname+".score"
    if not os.access(fname, os.R_OK):
        return 0
    # si le fichier score n'existe pas, ou que sa date de modification est plus ancienne que le fichier out
    if not os.access(scorefile, os.R_OK) or os.stat(scorefile).st_mtime < os.stat(fname).st_mtime:
        if lock_score(scorefile+".lock"):
            # supprime le fichier score et le régénère
            try:
                os.unlink(scorefile)
            except:
                pass
            try:
                print( "./checker", infile, fname )
                subprocess.check_call( ["./checker", infile, fname] )
            except:
                os.unlink(fname)
                unlock_score(scorefile+".lock")
                return 0
        else:
            # another process is recomputing the score, just wait until he finishes and forget mine.
            while not lock_score(scorefile+".lock"):
                time.sleep(0.01)
        unlock_score(scorefile+".lock")
    # ouvre le fichier de score, lit le score et le renvoie (+ ferme le fichier)
    try:
        fd = open(scorefile,"r")
    except:
        return 0
    try:
        s = int(fd.read())
    except:
        s = 0
    fd.close()
    return s

# lit le score d'après le nom du fichier lié
def readscore(fname):
    try:
        if os.path.islink(fname):
            fname = os.readlink(fname)
        s = int(re.split("\.",fname)[1])
    except:
        return 0
    return s

###############################################################################
best = computescore(seed)
print( "Current best score :", best )

f = 0
nbfail = 0
while True:
    f += 1
    unique_postfix = pid + str(f) + ".out"
    outfile = tmpdir + unique_postfix

    cmd = ["./solver", infile, outfile]
    if best>0:
        cmd += [seed]
    try:
        status = subprocess.call( cmd, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL )
        if status<0:
            # user controlled exit
            print( " ...signaled! exiting." )
            break
        elif status>0:
            print( " ... FAILED" )
            print( '(command: "'+" ".join(cmd)+'" exit code:', status, ")")
            nbfail += 1
            if nbfail > 10:
                print("repeated failures, exiting.")
                break
        else:
            nbfail = 0
    except:
        print( " ...stopped!" )
        break
    try:
        fd = open(outfile+".score","r")
        score = int(fd.read())
        fd.close()
    except:
        score = 0

    print( "\r"+outfile+" :", score, end="    ", flush=True)
    oldbest = best
    best = readscore(seed)
    if oldbest != best:
        print( "(new best = " + str(best) + ")", end="", flush=True)
    if score > best:
        # copy source
        bestfname = b + "." + str(score) + "." + unique_postfix
        os.system( "cp " + outfile + " " + "out/"+bestfname )

        r = readscore(seed)    # check if the best did not change meanwhile, and if it did, just forget it...
        if score > r:
            # link the new .out file
            os.system( "ln -s -f "+ bestfname +" "+ seed)
            best = score
            print( " <- new best !" )
    elif score > show or oldbest != best:
        # aesthetic newline
        print()
