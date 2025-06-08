#!/usr/bin/env python3


import sys, os, subprocess, struct, math, array
from t9n import voices, vocab
labelsy=['xmins','xminp','xi','xinch','xmm','xmil','xvox',
    'xspeeds','xspeedp','xvtyps','xvtypp','xvoff',
    'xsstil','xschan','xsrequ','xnosig','xbatts','xbattp',
    'xblu','xperc','xmelof','xmelon','xmilsof','xmilson',
    'xoverof','xoveron','xcharge','xstored','xcancel',
    'xvol0','xvol1','xvol2']
    

jedno="0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 30 40 50 60 70 80 90 100 200 300 400 500 600 700 800 900".split()
extrasy = []
args=sys.argv[1:]
lang = args.pop(0)
ful = True
opens = True
melnable = True
if lang not in voices:
    print("Bad language")
    exit(1)
while len(args) > 0:
    param = args.pop(0)
    if param == 'small':
        ful = False
    elif param == 'cs':
        opens = False
    elif param == 'dism':
        melnable = False
    else:
        print("Usage: %s <language> [small] [cs] [dism]" % sys.argv[0])
        exit(1)

if not opens and not ful:
    print("Only full mode for dir: voices")
    exit(1)

if ful:
    jedno = list(str(x) for x in range(100)) + ("100 200 300 400 500 600 700 800 900".split())

if lang == 'es':
    if ful:
        extrasy = ['y']
    else:
        extrasy = 'ý,21,22,23,24,25,26,27,28,29,30 y,40 y,50 y,60 y,70 y,80 y,90 y' . split(',')
elif lang == 'de':
    extrasy=['und']
    jedno[1] = 'ein'
    
class mkpcm(object):
    piper_path = '/home/ethanak/Pobrane/Linux/piper/piper'
    piper_dir = '/home/ethanak/Pobrane/Linux/piper'
    
    def __init__(self, lang):
        global voices, vocab, ful, cs, melnable
        self.lang = lang
        self.vob=vocab[lang]
        self.melody = voices[lang].get('melody',False) if melnable else False
        self.voices = []
        for v in voices[lang]['voices']:
            vt = v['code'].startswith('dir:')
            if vt == opens:
                continue
            self.voices.append(v)
        if len(self.voices) == 0:
            print("No voices")
            exit(1)
        if ful and len(self.voices) > 1:
            self.melody = False
        if self.melody:
            for a in self.voices:
                if a['code'].startswith('dir:') or a['code'].endswith('.onnx'):
                    self.melody = False
                    print("Melody disabled")
                    break
        self.datas=[]
        self.datal=[]
        self.datab=[]
        self.totlen=0
        self.piper = None
        self.tts = None
        self.indir = None

        self.ovname = 'vox_%s.h' % lang
        self.odname = 'voxdefs_%s.h' % lang
            
    def initTTS(self):
        if self.voice.startswith('dir:'):
            self.indir = True
            self.vdir = self.voice[4:]
            self.piper = None
        elif self.voice.endswith('.onnx'):
            self.piper = self.__class__.piper_path
            self.pipdir = self.__class__.piper_dir
            self.indir = None
        else:
            self.piper = None
            self.indir = None
            if self.tts is None:
                from rhvoice_wrapper import TTS
                self.tts=TTS(threads=1,quiet=True)
            self.tts.set_params(relative_rate=self.vspeed)
            
            
            
    def setVoice(self, voice):
        self.voice = voice['code']
        self.vspeed = voice.get('speed',1.0)
        print(voice, voice.get('name'))
        self.vname = voice.get('name')
        self.vstrb = voice.get('stripbeg',3)
        self.vstre = voice.get('stripend',3)
        self.vthrb = voice.get('begthr',0.1)
        self.vthre = voice.get('finthr',0.1)
        self.vrate = voice.get('rate',24000)
        self.vcont = voice.get('contrast', None)
        self.vfilter = voice.get('filter',None)
        self.initTTS()
        #if self.tts:
        #    self.tts.set_params(relative_rate=self.vspeed)
        #elif self.indir:
        #    self.vdir = self.voice[4:]
        
    #@staticmethod
    #def applyContrast(vox,level):
    #    level = level / 100.0
    #    mpi = math.pi / (2 * 32768.0)
    #    for i, v in enumerate(vox):
    #        #d = M_PI_2 * buffer[i] / 32768.0;
    #        d = mpi * vox[i]
            #buffer[i] = sin(d + contrast_level * sin(d * 4)) * 32767;
    
    #        vox[i] = int(math.sin(d + level * math.sin(d*4)) * 32767)
    
    def vnormalize(self,v):
        if self.indir:
            cmd=['sox','-t','raw','-e','signed-integer','-r',str(self.vrate),'-b','16','-','-t','raw','-e','signed-integer','-r','16000','-c','1','-r','16000','-','norm','-0.1']
        elif self.piper is not None:
            cmd=['sox','-t','raw','-e','signed-integer','-r',str(self.vrate),'-b','16',
                '-',
                '-t','raw','-e','signed-integer','-r','16000','-c','1','-r','16000','-','norm','-0.1']
        else:
            cmd=['sox','-t','wav','--ignore-length','-','-t','raw','-e','signed-integer','-r','16000','-c','1','-r','16000','-','norm','-0.1']
    
        #cmd=['sox','-t','raw','-e','signed-integer','-c', '1','-r',str(self.vrate),'-b','16',
        #    '-',
        #    '-t','raw','-e','signed-integer','-r','16000','-c','1','-','norm','-0.1']
        if self.vfilter is not None:
            cmd.append('lowpass')
            cmd.append('-2')
            cmd.append(str(self.vfilter)+'k')
        p=subprocess.Popen(cmd,
            stdin=subprocess.PIPE,
            stdout=subprocess.PIPE);
        res=p.communicate(v)
        return res[0]

    @staticmethod
    def energy(x):
        ene = sum(float(a * a) / (32767.0 * 32767.0) for a in x)
        return math.sqrt(ene / 160)
    
    def trimvox(self,vox):
        #print(type(vox))
        fno = len(vox) // 160
        frames=[]
        for b in range(fno):
            s=vox[b*160:(b+1)*160]
            frames.append(self.energy(s))
        #print(frames)
        beg=0
        for i,n in enumerate(frames):
            if n >= self.vthrb:
                beg = i
                break
        #beg=0
        if beg > self.vstrb:
            vox = vox[(beg-self.vstrb) * 160:]
            frames = frames[beg-self.vstrb:]
            beg = self.vstrb
    
            
    
        fin = len(frames) - 1
        while fin > beg+3:
            if frames[fin] > self.vthre:
                break
            fin -= 1

        if fin < len(frames) - self.vstre:
            frames=frames[:fin+self.vstre]
            vox = vox[:160 * (fin+self.vstre)]
        return vox

    def getraw(self,txt, lab=None):
        if self.indir:
            if lab == 'ein':
                lab = '1'
            fname = os.path.join(self.vdir,lab+'.raw')
            if not os.path.exists(fname) and lab.endswith('p'):
                fname = os.path.join(self.vdir,lab[:-1]+'s.raw')
            print(fname)
            return open(fname,'rb').read()  
        elif self.piper:
            p=subprocess.Popen([self.piper, '--model',os.path.join(self.pipdir,self.voice),
            '--output-raw'],
            stdin=subprocess.PIPE,
            stdout=subprocess.PIPE)
            res=p.communicate(txt.encode('utf-8'))
            if p.returncode != 0:
                raise Exception("ERROR")
            return res[0]
        else:
            return self.tts.get(text=txt,voice=self.voice,format_='wav')
            
    def mkwave(self, txt, lab=None):
        vox = array.array('h')
        vox.frombytes(self.vnormalize(self.getraw(txt,lab)))
        vox=self.trimvox(vox).tobytes()
        cmd=['sox','-t','raw','-e','signed-integer','-r','16000','-b','16',
            '-','-t','raw','-e','u-law','-r','16000','-','norm','-0.1']
        if self.vcont is not None:
            cmd.append('contrast')
            cmd.append(str(self.vcont))
        p=subprocess.Popen(cmd,
                stdin=subprocess.PIPE,
                stdout=subprocess.PIPE)
                #stderr=subprocess.DEVNULL)
        res=p.communicate(vox)
        if p.returncode != 0:
            raise Exception("ERROR")
        return res[0]
        
    def mkwave_old(self, txt, dtrim = True):
        vox =  self.tts.get(text=txt,voice=self.voice,format_='pcm')
        le=int(len(vox) / 2)
        vox=struct.unpack('%dh' % le, vox)
        if dtrim:
            for i in range(2,le):
                if abs(vox[i]) > 128:
                    break
            beg=max(i-200,2)
        #    print(beg,i)
            for i in range(le-10,beg,-1):
                if abs(vox[i]) > 128:
                    break
            fin = min(i+200,le-10)
            #print(fin,i,le)
        else:
            beg=100
            fin=len(vox) - 100
        vox=vox[beg:fin]
        if self.vcon is not None:
            self.applyContrast(vox,self.vcon)
        le=len(vox)
        vox=struct.pack('%dh' % le, *vox)
        p=subprocess.Popen(['sox','-t','raw','-e','signed-integer','-r','24000','-b','16',
            '-','-t','raw','-e','u-law','-r','16000','-','norm','-0.1'],
                stdin=subprocess.PIPE,
                stdout=subprocess.PIPE)
                #stderr=subprocess.DEVNULL)
        res=p.communicate(vox)
        if p.returncode != 0:
            raise Exception("ERROR")
        return res[0]

    def addwave(self,   txt, lab=None):
        wave = self.mkwave(txt,lab)
        self.datas.append(self.totlen)
        le=len(wave)
        self.datal.append(le)
        self.datab.extend(wave)
        self.totlen += le

    def runvox(self, v):
        global jedno, labelsy, extrasy
        self.setVoice(v)
        for jed in jedno:
            self.addwave(jed, jed)
            if self.melody:
                self.addwave(jed+'?', jed)
        
        if len(extrasy) > 0:
            for jed in extrasy:
                self.addwave(jed, jed)
                if self.melody:
                    self.addwave(jed+'?', jed)
            
        if not self.defsok:
            self.notnum = len(self.datas)
        for t in labelsy:
            tx=self.vob[t]
            if self.indir and tx == 'ii?':
                tx = 'i'
            if tx is None:
                tx = self.vname
            if not self.defsok:
                if self.melody or tx == 'ii?' or not tx.endswith('?'):
                    self.defs[t] = len(self.datas)
                else:
                    print(t, tx)
                    self.defs[t] = self.defs[t[:-1] + 's']
            if self.melody or tx == 'ii?' or not tx.endswith('?'):
                self.addwave(tx, t)
            
    def run(self):
        self.defsok = False
        self.defs = {}
        self.defsno = 0
        self.vbeg = []
        for v in self.voices:
            self.vbeg.append(len(self.datas))
            self.runvox(v)
            self.defsno = len(self.datas)
            self.defsok = True
        self.storevoxdefs()
        self.storevox()

    def storetab(self,name,data, outf):
        print("static const int vox_%s[]={" % name, file=outf,end='\n    ')
        dout=[]
        while len(data) > 0:
            dout.append(','.join('%d' % x for x in data[:8]))
            data=data[8:]
        dout=',\n    '.join(dout)
        print(dout,file=outf)
        print("};",file=outf)

    def storedat(self,name,data,outf):
        print("static const uint8_t vox_%s[]={" % name, file=outf,end='\n    ')
        for i in range(0,len(data),16):
            if i > 0:
                print(",\n    ",file=outf, end='')
            print(",".join('%d' % x for x in data[i:i+16]),end='',file=outf)
        print("};",file=outf)


    def storevox(self):
        outf=open(self.ovname,'w')
        print("/* autogenerated - do not edit */",file=outf)
        self.storetab('datastart',self.datas,outf)
        self.storetab('datalengt',self.datal,outf)
        self.storedat('data',self.datab,outf)
        print("Data size: %d" % len(self.datab))
        s="static int voffsets[]={" + (', '.join(str(x) for x in self.vbeg)) + '};'
        print(s, file=outf)
        #s="static int overlaps[]={" + (', '.join(str(x['params']['overlap']) for x in self.voices))+'};'
        #print(s)
        #print(s, file=outf)
        outf.close()

    def storevoxdefs(self):
        global labelsy
        outd=open(self.odname,'w')
        print("/* autogenerated - do not edit */",file=outd)
        print("#ifndef VOXDEFS_H",file=outd)
        print("#define VOXDEFS_H",file=outd)
        if not self.melody:
            print("#define DISABLE_MELODY", file=outd)
            print("#define NUMSTEP 1", file=outd)
            nst=1
        else:
            print("#define NUMSTEP 2", file=outd)
            nst=2
        for x in labelsy:
            print("#define vword_%s %s" % (x,self.defs[x]), file=outd)
        if ful:
            print("#define vword_100 100", file=outd)
            print("#define vword_extras 109", file = outd)
            print("#define FULL_100 1", file = outd)
        else:
            print("#define vword_20 20",  file=outd)
            print("#define vword_100 28", file=outd)
            print("#define vword_extras 37", file = outd)
        if self.lang == 'es':
            print("#define vword_yy vword_extras", file = outd)
            print("#define vword_21 (vword_yy + 1)", file = outd)
            print("#define vword_30y (vword_21 + 9)", file = outd)
        elif self.lang == 'de':
            print("#define vword_xunds vword_extras", file = outd)
        print("#define vword_nonum %d" % self.notnum, file=outd)
        print("#define VOICE_COUNT %d" % len(self.voices), file=outd)
            
        print("#endif", file=outd)
        outd.close()
        


        

    
genr = mkpcm(lang)
try:
    genr.run()
except:
    import traceback
    traceback.print_exc()
    if genr.tts:
        genr.tts.join()
    exit(1)
if genr.tts:
    genr.tts.join()
exit(0)
