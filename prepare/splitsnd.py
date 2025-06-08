#!/usr/bin/env python3

import os, subprocess, struct, math, sys, array

EFinal = 0.00007 # final frame energy
EStart = 0.0005  # start frame energt
ECount = 7       # minimum silence frames
EInit  = EStart  # first audio frame energy

def helpme():
    print("Usage: %s <audiofile> <outdir> [<language>]" % sys.argv[0])
    print("  audiofile must be .wav or .mp3")
    print("  outdir must exist")
    print("  optional language parameter may be 'es' or 'de' for extra words")
    exit(0)
    
try:
    vpath=sys.argv[1]
    opath=sys.argv[2]
    lang  = None
    if len(sys.argv) > 3:
        lang = sys.argv[3]
        if lang not in ('es','de'):
            raise Exception()
except:
    helpme()
vtyp = vpath[-3:].lower()
if vtyp not in ('wav','mp3'):
    helpme();

labels=list(str(x) for x in range(100))
labels.extend('100 200 300 400 500 600 700 800 900'.split())
labels.extend(['xmins','xinch','xmm','xmil','xvox',
    'xspeeds','xvtyps','xvoff',
    'xsstil','xschan','xsrequ','xnosig','xbatts',
    'xblu','xperc','xmelof','xmelon','xmilsof','xmilson',
    'xoverof','xoveron','xcharge','xstored','xcancel',
    'xvol0','xvol1','xvol2','xi'])
if lang == 'es':
    labels.append('yy')
elif lang == 'de':
    labels.append('und')


audio = array.array('h')
audio.frombytes(subprocess.check_output(
        ['sox','-t',vtyp,vpath,'-t','raw','-r','16000','-']))

def energy(x):
    ene = sum(float(a * a) / (32767.0 * 32767.0) for a in x)
    return math.sqrt(ene / 160)

fno = len(audio) // 160
frames=[]
for b in range(fno):
    s=audio[b*160:(b+1)*160]
    frames.append(energy(s))

for beg in range(fno):
    if frames[beg] > EInit:
        #print("FA", beg, frames[beg])
        break
if beg < 0:
    beg = 0
if beg > 0:
    frames = frames[beg:]
    fno -= beg
    audio = audio[beg * 160:]
    
beg = 3
nr = 0
#print(labels)
while beg < fno:
    fst = beg
    n=0
    while beg < fno-1:
        if frames[beg] < EFinal:
            n += 1
            if n > ECount:
                break
        else:
            n = 0
        beg += 1
    print(labels[nr],fst, beg, fno)
    fname = os.path.join(opath,'%s.raw' % labels[nr])
    open(fname,'wb').write(audio[fst*160:(beg+2) * 160])
    
    nr += 1
    while beg < fno:
        if frames[beg] > EStart:
            break
        beg += 1
    fst = beg-3

if nr < len(labels):
    raise Exception("Not all labels was used")
print("Created %d audio parts" % nr)
