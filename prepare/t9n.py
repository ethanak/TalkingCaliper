#!/usr/bin/env python3


voices = {
    'en': {
        'voices': [
            {'code':'bdl','name':'male','speed':0.8,
                'stripbeg': 3,
                'stripfin': 3,
                'begthr': 0.12,
                'finthr': 0.007},
            {'code':'slt','name':'female','speed':0.8,
                'stripbeg': 3,
                'stripfin': 3,
                'begthr': 0.1,
                'finthr': 0.007},
            #{'code':'alan','name':'male','speed':0.8,'rate':24000,
            #    'stripbeg': 3,
            #    'stripfin': 3,
            #    'begthr': 0.12,
            #    'finthr': 0.04},
            {'code':'dir:jenny','name':'Jenny','rate':'16000',
                'begthr': 0.005,
                'finthr': 0.01,
            },
            {'code':'dir:eric','name':'Eric','rate':'16000',
                'begthr': 0.005,
                'finthr': 0.01,
            },

            ]
        },

    'pl':{
        'melody': False,
        'voices':[
            {'code':'magda','name':'magda','speed':1.1,'rate':24000,
        'stripbeg': 3,
        'stripfin': 3,
        'begthr': 0.10,
        'finthr': 0.03
            },
            {'code':'natan','name':'natan','speed':1.1,'rate':24000,
                'stripbeg': 3,
                'stripfin': 3,
                'begthr': 0.1,
                'finthr': 0.02
            },
        {'code':'dir:zofia','name':'zofia','rate':'16000',
                'begthr': 0.005,
                'finthr': 0.01,
            },
        {'code':'dir:marek','name':'marek','rate':'16000',
                'begthr': 0.005,
                'finthr': 0.01,
            },
            ]
        },

    'es':{
        'melody':False,
        'dir': True,
        'voices':[
            {'code':'Mateo','name':'mateo','speed':0.8, 'rate':16000, 'filter': 3,
                'stripbeg': 3,
                'stripfin': 3,
                'begthr': 0.01,
                'finthr': 0.01,
            },
            {'code':'dir:jorge','name':'Jorge','rate':16000,
                'begthr': 0.005,
                'finthr': 0.01
            },
            {'code':'dir:dalia','name':'Dalia','rate':16000,
                'begthr': 0.005,
                'finthr': 0.01
            },



            ]
        },

    'de':{
        
        'melody': True,
        'voices':[
            {'code':'de_DE-thorsten-high.onnx','name':'Thorsten','rate':22050,
                'stripbeg': 3,
                'stripfin': 3,
                'begthr': 0.0025,
                'finthr': 0.000001,
              'contrast': 2},

            {'code':'dir:killian','name':'Killian','rate':'16000',
                'begthr': 0.005,
                'finthr': 0.01,
            },
            {'code':'dir:katja','name':'Katja','rate':'16000',
                'begthr': 0.005,
                'finthr': 0.01,
            }
            
        ]},

    }

vocab={
  'pl': {
    'xmins':'minus','xminp':'minus?','xi':'ii?',
    'xinch':'cala','xmm':'milimetra','xmil':'mils','xvox':None,
    'xspeeds':'prędkość','xspeedp':'prędkość?',
    'xvtyps':'głos','xvtypp':'głos?','xvoff':'wyłącz urządzenie',
    'xsstil':'odczyt ciągły','xschan':'odczyt zmian','xsrequ':'odczyt na żądanie',
    'xnosig':'brak odczytu','xbatts':'bateria','xbattp':'bateria?',
    'xblu':'blutuf aktywny','xperc':'%',
    'xmelof':'melodia płaska','xmelon':'melodia uwydatniona',
    'xmilsof':'używaj zawsze cali','xmilson':'używaj mils poniżej cala',
    'xoverof':'oddzielaj, cyfry','xoveron':'połącz cyfry',
    'xcharge':'tryb ładowania',
    'xstored':'zapisano ustawienia',
    'xcancel':'anulowano zapis ustawień',
    'xvol0':'Głośność minimalna',
    'xvol1':'Głośność średnia',
    'xvol2':'Głośność pełna'
    },
  'de': {
    'xmins':'minus','xminp':'minus?','xi':'punkt',
    'xinch':'Zoll','xmm':'Millimeter','xmil':'mils','xvox':None,
    'xspeeds':'Sprechgeschwindigkeit.','xspeedp':'Sprechgeschwindigkeit?',
    'xvtyps':'Sprachname.','xvtypp':'Sprachname?','xvoff':'Gerät ausschalten',
    'xsstil':'alles aussprechen','xschan':'Änderungen aussprechen',
    'xsrequ':'sprechen auf Anfrage',
    'xnosig':'kein Signal','xbatts':'Batterie','xbattp':'Batterie?',
    'xblu':'blutuf aktiv','xperc':'%',
    'xmelof':'flache Melodie','xmelon':'erweiterte Melodie',
    'xmilsof':'immer Zoll verwenden','xmilson':'Verwenden Mils für kleine Werte',
    'xoverof':'separate Nummern','xoveron':'verbinde Nummern',
    'xcharge':'Lademodus',
    'xstored':'Einstellung gespeichert',
    'xcancel':'Einstellung abgebrochen',
    'xvol0':'Minimum Lautstärke',
    'xvol1':'Nominal Lautstärke',
    'xvol2':'Maximum Lautstärke'
  },
  'en': {
    'xmins':'minus','xminp':'minus?','xi':'point',
    'xinch':'inch','xmm':'millimeters','xmil':'mils','xvox':None,
    'xspeeds':'speed.','xspeedp':'speed?',
    'xvtyps':'voice.','xvtypp':'voice?','xvoff':'turn off device',
    'xsstil':'reading continuous','xschan':'reading changes','xsrequ':'reading on demand',
    'xnosig':'no signal','xbatts':'battery','xbattp':'battery?',
    'xblu':'bluetooth active','xperc':'%',
    'xmelof':'melody flat','xmelon':'melody extended',
    'xmilsof':'always use inches','xmilson':'use mils for small values',
    'xoverof':'separate numbers','xoveron':'connect numbers',
    'xcharge':'charging mode',
    'xstored':'settings saved',
    'xcancel':'setting cancelled',
    'xvol0':'volume minimal',
    'xvol1':'volume standard',
    'xvol2':'volume full'
    },
  'es': {
    'xmins':'menos','xminp':'menos?','xi':'punto',
    'xinch':'pulgadas','xmm':'milimetros','xmil':'mils','xvox':None,
    'xspeeds':'velocidad.','xspeedp':'velocidad?',
    'xvtyps':'voz.','xvtypp':'voz?','xvoff':'apague el dispositivo',
    'xsstil':'leer continuamente','xschan':'leer cambios','xsrequ':'leer a pedido',
    'xnosig':'sin señal','xbatts':'acumulador','xbattp':'acumulador?',
    'xblu':'blútuf activo','xperc':'%',
    'xmelof':'entonación plana','xmelon':'entonación extendida',
    'xmilsof':'Utilice siempre pulgadas','xmilson':'Utilice milésimas para valores pequeños',
    'xoverof':'separar números','xoveron':'conectar números',
    'xcharge':'modo de carga',
    'xstored':'configuración guardada',
    'xcancel':'configuración cancelada',
    'xvol0':'volumen mínimo',
    'xvol1':'volumen estándar',
    'xvol2':'volumen máximo'
    }
}

