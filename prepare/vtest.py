#!/usr/bin/env python3

import sys, subprocess,os, math, array, struct
args=sys.argv[1:]
arg=args.pop(0)

voice = arg
from t9n import voices
v = None
for lang in voices:
    for vo in voices[lang]['voices']:
        if vo['code'] == voice:
            v = vo
            break
    if v:
        break

if v is None:
    print ("Bad voice")
    exit(0)
#print(v)
vspeed = v.get('speed',1.0)
vrate = v.get('rate',24000)
vcon = v.get('contrast',None)
vfilter = v.get('filter',None)
vstrb = v.get('stripbeg',3)
vstre = v.get('stripend',3)
vthrb = v.get('begthr',0.1)
vthre = v.get('finthr',0.1)

piper = None
tts=None
isdy=None
if voice.endswith('.onnx'):
    pipdir=os.path.join(os.environ['HOME'],'Pobrane','Linux','piper')
    piper = os.path.join(pipdir,'piper')
elif voice.startswith('dir:'):
    isdy = voice[4:]
else:
    from rhvoice_wrapper import TTS
    tts=TTS(threads=1,quiet=True)
    tts.set_params(relative_rate=vspeed)
    


def getRaw(txt):
    global voice, piper, isdy
    if isdy is not None:
        fname = os.path.join(isdy,txt+'.raw')
 #       print(fname)
        return open(fname,'rb').read()
    elif piper is None:
        return tts.get(text=txt,voice=voice,format_='wav')
    else:
        p=subprocess.Popen([piper, '--model',os.path.join(pipdir,voice),
            '--output-raw'],
            stdin=subprocess.PIPE,
            stdout=subprocess.PIPE)
        res=p.communicate(txt.encode('utf-8'))
        if p.returncode != 0:
            raise Exception("ERROR")
        return res[0]


voxes = []
    
def vnormalize(v):
    if piper is not None:
        cmd=['sox','-t','raw','-e','signed-integer','-r',str(vrate),'-b','16',
            '-',
            '-t','raw','-e','signed-integer','-r','16000','-c','1','-r','16000','-','norm']
    elif isdy is not None:
        cmd=['sox','-t','raw','-e','signed-integer','-r',str(vrate),'-b','16','-','-t','raw','-e','signed-integer','-r','16000','-c','1','-r','16000','-','norm','-0.1']
    else:
        cmd=['sox','-t','wav','-','-t','raw','-e','signed-integer','-r','16000','-c','1','-r','16000','-','norm','-0.1']
    if vfilter is not None:
        cmd.append('lowpass')
        cmd.append('-2')
        cmd.append(str(vfilter)+'k')
    p=subprocess.Popen(cmd,
        stdin=subprocess.PIPE,
        stdout=subprocess.PIPE);
    res=p.communicate(v)
    return res[0]
    
    
for a in args:
    print('Phoneme',a)
    vox=array.array('h')
    vox.frombytes(vnormalize(getRaw(a)))
    #le=int(len(vox) / 2)
    #vox=list(struct.unpack('%dh' % le, vox))
    l=160 * ((len(vox)+159) // 160) - len(vox)
    if l > 0:
        vox.extend(l * [0])
    voxes.append(vox)
    
if tts:
    tts.join()

def energy(x):
    ene = sum(float(a * a) / (32767.0 * 32767.0) for a in x)
    return math.sqrt(ene / 160)

def trimvox(vox):
    fno = len(vox) // 160
    frames=[]
    for b in range(fno):
        s=vox[b*160:(b+1)*160]
        frames.append(energy(s))
    print("Frame energy:")
    print(frames)
    beg=0
    for i,n in enumerate(frames):
        if n >= vthrb:
            beg = i
            break
    if beg > vstrb:
        vox = vox[(beg-vstrb) * 160:]
        frames = frames[beg-vstrb:]
        beg = vstrb
    fin = len(frames) - 1
    while fin > beg+3:
        if frames[fin] > vthre:
            break
        fin -= 1
    
    if fin < len(frames) - vstre:
        frames=frames[:fin+vstre]
        vox = vox[:160 * (fin+vstre)]

    return vox

outvox = array.array('h')
for vox in voxes:
    outvox.extend(trimvox(vox))

def play16(vox):
    cmd = ['play','-t','raw','-e','signed-integer','-r','16000','-b','16',
    '-']
    if vcon:
        cmd.append('contrast')
        cmd.append(str(vcon))
    p=subprocess.Popen(cmd,stdin=subprocess.PIPE,stderr=subprocess.DEVNULL)
    p.communicate(vox.tobytes())

play16(outvox)

