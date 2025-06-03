/*
   Copyright 2025 Bohdan R, Rau <ethanak@polip.com>

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

     http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/

#include <Arduino.h>
#include <freertos/queue.h>
#include "i18n.h"
#include VOXDEFS
#include <AudioGenerator.h>
#include <AudioOutputI2S.h>
#include "sonic_lite.h"
#include "common.h"

#define _external_dac 1

static AudioOutputI2S *player = NULL;
extern bool disablePauses;
static const short ulaw_decode[256] = {
    -32124, -31100, -30076, -29052, -28028, -27004, -25980, -24956,
    -23932, -22908, -21884, -20860, -19836, -18812, -17788, -16764,
    -15996, -15484, -14972, -14460, -13948, -13436, -12924, -12412,
    -11900, -11388, -10876, -10364,  -9852,  -9340,  -8828,  -8316,
     -7932,  -7676,  -7420,  -7164,  -6908,  -6652,  -6396,  -6140,
     -5884,  -5628,  -5372,  -5116,  -4860,  -4604,  -4348,  -4092,
     -3900,  -3772,  -3644,  -3516,  -3388,  -3260,  -3132,  -3004,
     -2876,  -2748,  -2620,  -2492,  -2364,  -2236,  -2108,  -1980,
     -1884,  -1820,  -1756,  -1692,  -1628,  -1564,  -1500,  -1436,
     -1372,  -1308,  -1244,  -1180,  -1116,  -1052,   -988,   -924,
      -876,   -844,   -812,   -780,   -748,   -716,   -684,   -652,
      -620,   -588,   -556,   -524,   -492,   -460,   -428,   -396,
      -372,   -356,   -340,   -324,   -308,   -292,   -276,   -260,
      -244,   -228,   -212,   -196,   -180,   -164,   -148,   -132,
      -120,   -112,   -104,    -96,    -88,    -80,    -72,    -64,
       -56,    -48,    -40,    -32,    -24,    -16,     -8,      0,
     32124,  31100,  30076,  29052,  28028,  27004,  25980,  24956,
     23932,  22908,  21884,  20860,  19836,  18812,  17788,  16764,
     15996,  15484,  14972,  14460,  13948,  13436,  12924,  12412,
     11900,  11388,  10876,  10364,   9852,   9340,   8828,   8316,
      7932,   7676,   7420,   7164,   6908,   6652,   6396,   6140,
      5884,   5628,   5372,   5116,   4860,   4604,   4348,   4092,
      3900,   3772,   3644,   3516,   3388,   3260,   3132,   3004,
      2876,   2748,   2620,   2492,   2364,   2236,   2108,   1980,
      1884,   1820,   1756,   1692,   1628,   1564,   1500,   1436,
      1372,   1308,   1244,   1180,   1116,   1052,    988,    924,
       876,    844,    812,    780,    748,    716,    684,    652,
       620,    588,    556,    524,    492,    460,    428,    396,
       372,    356,    340,    324,    308,    292,    276,    260,
       244,    228,    212,    196,    180,    164,    148,    132,
       120,    112,    104,     96,     88,     80,     72,     64,
	56,     48,     40,     32,     24,     16,      8,      0 };

class CorpSynthGenerator: public AudioGenerator
{
    public:
    CorpSynthGenerator(): _speed(0), _wcount(0), _voice(0) {};
    
    bool begin(int *words, int count, AudioOutputI2S *out);
    bool begin(AudioOutputI2S *out);
    bool setVoice(int voice);
    bool setSpeed(int speed);
    bool setVol(int volume);
    bool feed(int nword);
    bool stop();
    bool isRunning(void);
    bool loop(void);
    void reset(void);

    private:

    bool beginAudioOutput(AudioOutput * output);
    int readSonicBuffer(void);
    int getNextPause(void);
    int16_t getSample(void);
    int16_t getPlainSample(void);
    uint16_t _words[32];
    const uint8_t *_wordstart[32];
    int _wordlen[32];
    uint8_t _wcount;
    int8_t _wpos;
    uint8_t _voice;
    uint8_t _volume;
    uint8_t running;
    bool consumed;
    uint8_t flushing;
    uint8_t _lpfilter;
    int16_t fsample;
    int _wordcon;
    int currentPause, nextPause;
    const uint8_t *sonicBytes;
    const uint8_t *nextBytes;
    int sonicBytePos;
    int sonicByteLen;
    int nextBytePos;
    int nextByteLen;
    int sonicWordPos;
    int samplesOut;
    int sonicOutPos;
    float _speed;
    int16_t inBuffer[SONIC_INPUT_SAMPLES];
    int16_t outBuffer[SONIC_INPUT_SAMPLES];
};


static void initAudioOutput(void)
{
    if (_external_dac) {
        player = new AudioOutputI2S(0,AudioOutputI2S::EXTERNAL_I2S,8,AudioOutputI2S::APLL_ENABLE);
        player->SetPinout(_bclk_pin, _wclk_pin, _dout_pin);
        //player->SetGain(0.75);
        player->SetGain(1.0);
    }
    else {
        player = new AudioOutputI2S(0,AudioOutputI2S::INTERNAL_DAC,8,AudioOutputI2S::APLL_ENABLE);
        player->SetGain(1.0);
        
    }
}

bool CorpSynthGenerator::begin(AudioOutputI2S *out)
{
    sonicInit();
    sonicSetSpeed(_speed);
    running = false;
    sonicWordPos=-1;
    samplesOut=0;
    sonicOutPos = 0;
    consumed=true;
    flushing=0;
    fsample = 0;
    _lpfilter = lpfilter;
    return beginAudioOutput(out);
}

bool CorpSynthGenerator::beginAudioOutput(AudioOutput * output)
{
    output->SetRate(16000);
    output->SetBitsPerSample(16);
    output->SetChannels(1);
    if (!output->begin()) return false;
    this->output = output;
    running = true;
    return true;
}


bool CorpSynthGenerator::begin(int *words, int count, AudioOutputI2S *out)
{
    if (count > 32) count=32;
    int i;
    for (i=0; i<count;i++) {
        _words[i] = words[i];
        _wordstart[i] = getVoxData(words[i],_voice,&_wordlen[i]);
    }
    _wcount=count;
    return begin(out);
    
}

bool CorpSynthGenerator::setVoice(int voice)
{
    if (voice >= 0 && voice < VOICE_COUNT) {
        _voice = voice;
        _wordcon = disablePauses;
        
        return true;
    }
    return false;
}

bool CorpSynthGenerator::setVol(int volume)
{
    if (volume >= 0 && volume < 3) {
        _volume = volume;
        return true;
    }
    return false;
}

static float speeds[]={1.0, 1.2, 1.4,  1.65, 2.0, 2.3, 2.6};
bool CorpSynthGenerator::setSpeed(int speed)
{
    if (speed < 0 || speed >= 7) return false;
    _speed=speeds[speed];
    return true;
}


#define PAUSECNT 2000
int CorpSynthGenerator::getNextPause(void)
{
    if (!_wordcon) return PAUSECNT;
    if (sonicWordPos >= _wcount - 1) return 0;
    if (_words[sonicWordPos] >= vword_nonum) return PAUSECNT / 10;
    if (_words[sonicWordPos+1] >= vword_nonum) return PAUSECNT / 10 ;
    return 0;
}

int CorpSynthGenerator::readSonicBuffer(void)
{
    int i;
    static const int ivol[]={4,2,1};
    for (i=0; i< SONIC_INPUT_SAMPLES; i++) {
        if (sonicWordPos == -1 || sonicBytePos >= sonicByteLen) {
            sonicWordPos++;
            if (sonicWordPos >= _wcount) break;
            if (!sonicWordPos) {
                sonicBytePos = 0;
//            int tw=words[sonicWordPos] + voffsets[thisVoice];
            //sonicBytes = getVoxData(_words[sonicWordPos], _voice, &sonicByteLen);
                sonicBytes = _wordstart[sonicWordPos];
                sonicByteLen = _wordlen[sonicWordPos];
                currentPause = 0;
                nextPause = getNextPause();
            }
            else {
                currentPause = nextPause;
                nextPause = getNextPause();
                sonicBytePos = 0;
                sonicBytes = _wordstart[sonicWordPos];
                sonicByteLen = _wordlen[sonicWordPos];
            }
            if (sonicWordPos < _wcount - 1) {
                nextBytes = _wordstart[sonicWordPos+1];
                nextByteLen = _wordlen[sonicWordPos+1];
                nextBytePos = 0;
                
            }
            else {
                nextBytes = NULL;
            }
 //           sonicByteLen = vox_datalengt[tw];
        }
        int sample;
        if (currentPause > 0) {
            currentPause--;
            sample = 0;
        }
        else {
            sample = ulaw_decode[sonicBytes[sonicBytePos++]];
        }
        sample /= ivol[_volume];
        inBuffer[i] = sample;
    }
    return i;
}


bool CorpSynthGenerator::isRunning(void)
{
    return running;
}


bool CorpSynthGenerator::feed(int word)
{
    if (_wcount < 32){
        _words[_wcount++] = word;
        return true;
    }
    return false;
}

bool CorpSynthGenerator::stop()
{
    //if (!running) return true;
    running = false;
    if (player) {
        player->stop();
        delete player;
    }
    player = NULL;
    output = NULL;
    _wcount=0;
    return true;
}

int16_t CorpSynthGenerator::getPlainSample(void)
{
    for (;;) {
        if (sonicOutPos < samplesOut) return outBuffer[sonicOutPos++];
        samplesOut = sonicReadShortFromStream(outBuffer, SONIC_INPUT_SAMPLES);
        sonicOutPos = 0;
        if (samplesOut > 0) return outBuffer[sonicOutPos++];
        int n=readSonicBuffer();
        if (n == 0) {
            if (flushing > 10) {
                running=false;
                return 0;
            }
            flushing++;
            //sonicFlushStream();
            memset((void *)inBuffer,0,2*SONIC_INPUT_SAMPLES);
            sonicWriteShortToStream(inBuffer, SONIC_INPUT_SAMPLES);
        }
        else {
            sonicWriteShortToStream(inBuffer, n);
        }
        samplesOut = sonicReadShortFromStream(outBuffer, SONIC_INPUT_SAMPLES);
    }
    return 0;
}

int16_t CorpSynthGenerator::getSample(void)
{
    static int flts[] = {0,2,4,5};
    return fsample = (fsample * flts[_lpfilter] + getPlainSample() * (8 - flts[_lpfilter])) / 8;
}

bool CorpSynthGenerator::loop(void)
{
    static int16_t sample;
    while (running) {
        if (consumed) {
            sample = getSample();
            if (!running) break;
            consumed=false;
        }
        lastSample[AudioOutput::LEFTCHANNEL] =
        lastSample[AudioOutput::RIGHTCHANNEL] = sample;
        if (!output->ConsumeSample(lastSample))
            break;
        consumed=true;
    }
    output->loop();
    delay(1);
    return running;
}

// code

static CorpSynthGenerator csg;


void initCSG(void)
{
    csg.stop();
}

bool startCSG(int *words, int count)
{
    if (player) printf("Player exists\n");
    if (player) return false;
    initAudioOutput();
    return csg.begin(words, count, player);
}

bool startCSG(uint8_t *words, int count)
{
    if (player) printf("Player exists\n");
    if (player) return false;
    initAudioOutput();
    int wds[32],i;
    for (i=0;i<count;i++) wds[i] = words[i];
    return csg.begin(wds, count, player);
}

static xQueueHandle spkQueue;

enum {
    ACMD_STOP=0,
    ACMD_VOICE = 33,
    ACMD_SPEED = 34,
};

bool loopCSG(void)
{
    if (csg.loop()) return true;
    csg.stop();
    return false;
}

void stopSpeech(void)
{
    csg.stop();
}

void setSpeed(int n)
{
    csg.setSpeed(n);
}

void setVoice(int n)
{
    csg.setVoice(n);
}

static volatile bool _isSpeaking = false;
bool useBLE;
void audioTask(void *param)
{
    
    initKey();

#ifndef DISABLE_BLE
    bool ble;
    if (useBLE) startBlue(blename);
#endif
    //startBlue("Testerek");
    //initKey();
    struct speechData spd;
    bool haveData=false;
    for (;;) {
        // odbierz wiadomość z kolejki
        if (!haveData) {
#ifndef DISABLE_BLE

            if (!useBLE)
#endif
            xQueueReceive(spkQueue,&spd, portMAX_DELAY);
#ifndef DISABLE_BLE

            else {
                bleCtlLoop();
                if (xQueueReceive(spkQueue,&spd, 100) == pdFALSE) continue;
            }
#endif

        }
        else haveData = false;
        if (spd.wordCnt == 0) continue; // stop

#ifndef DISABLE_BLE
        
        if (useBLE && spd.text[0]) {
            ble = bleSay(spd.text);
        }
        else ble=false;
        if (ble) {
            for (;;) {
                int rc = bleSpkLoop();
                if (rc == BLES_SUCCESS) {
                    _isSpeaking = false;
                    break;
                }
                else if (rc != BLES_ERROR) {
                    delay(10);
                    continue;
                }
                break;
            }
            if (!_isSpeaking) continue;
        }
#endif        
        csg.setSpeed(spd.param & 15);
        //csg.setSpeed();
        csg.setVoice((spd.param >> 4) & 3);
        csg.setVol((spd.param >> 6) & 3);
        _isSpeaking = startCSG(spd.wordList, spd.wordCnt);
        if (!_isSpeaking) continue;
        for (;;) {
            int qrc=xQueueReceive(spkQueue,&spd, 0);
            if (qrc == pdTRUE) {
                csg.stop();
                _isSpeaking = false;
                haveData = true;
                break;
                
            }
#ifndef DISABLE_BLE
            if (useBLE) bleCtlLoop();
#endif
            _isSpeaking = loopCSG();
            if (!_isSpeaking) break;
        }
    }
}

bool isSpeaking(void)
{
    return _isSpeaking;
}
void qSay(int voice, int speed, int volume, int count, int *wordlist, const char *btext)
{
    struct speechData spd;
    spd.wordCnt = count;
    int i;
    for (i=0;i<count;i++) spd.wordList[i] = wordlist[i];
    if (btext) strcpy(spd.text, btext);
    else spd.text[0] = 0;
    spd.param = (volume <<6) | (voice << 4) | speed;
    _isSpeaking = true;
    xQueueSend(spkQueue, &spd,0);
}

static TaskHandle_t speechHandle;

void startSecondTask(void)
{
    spkQueue = xQueueCreate(5,sizeof(struct speechData));
    xTaskCreatePinnedToCore(audioTask,"speech",32000,NULL,3,&speechHandle,0);
}
