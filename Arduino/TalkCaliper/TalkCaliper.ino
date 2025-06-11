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
#ifndef DISABLE_BLE
#include <WiFi.h>
#endif
#include <stdarg.h>
#include <driver/adc.h>
#include <esp_adc_cal.h>
#include "i18nstrings.h"
#include VOXDEFS
#include "common.h"
//#include "soc/rtc_wdt.h"

esp_adc_cal_characteristics_t db11_chars;


uint8_t lastInch=0xff;
uint32_t lastRead;
uint32_t lastSpoken;
uint8_t mainSpeed=0;
uint8_t mainVox=0;
uint8_t mainVol=0;
uint8_t lpfilter = 0;
static bool chargeMode, lastChargeMode;
static uint8_t chargeCnt;
static uint32_t lastChargeTimer = 0;
uint16_t calibV1, calibV2;
#ifndef DISABLE_BLE
char blename[32];
bool autoBLE;
#endif

enum {
    RDM_STILL = 0,
    RDM_CHAN,
    RDM_REQ,
    RDM_CHARGE,
    RDM_MAX
};

enum {
    MAMO_STD = 0,
    MAMO_PFS
};

#ifdef MOVIE_MODE
enum {
#if VOICE_COUNT > 1
    PFMO_VOX = 0,
#endif
    PFMO_OVER,
    PFMO_TEMPO,
    PFMO_VOL,
    PFMO_MILS,
#ifndef DISABLE_MELODY
    PFMO_MELO,
#endif
    PFMO_MAX
};

#else
enum {
    PFMO_TEMPO = 0,
    PFMO_VOL,
#if VOICE_COUNT > 1
    PFMO_VOX,
#endif
    PFMO_MILS,
#ifndef DISABLE_MELODY
    PFMO_MELO,
#endif
    PFMO_OVER,
    PFMO_MAX
};

#endif


const uint8_t readModeSpk[] = {
    vword_xsstil,
    vword_xschan,
    vword_xsrequ,
    vword_xcharge
};

const char * const readModeTxt[] = {
    STR_RDSTIL,
    STR_RDCHAN,
    STR_RDREQU,
    STR_RDCHRG
};

int wordBuffer[32],wordBufLen;
char blubuffer[256];
uint8_t readMode = RDM_STILL;
uint8_t mainMode = MAMO_STD;
uint8_t pfsMode = 0;
bool enableMelody, disablePauses, enableMils;
static bool tmpMelody, tmpPauses, tmpMils;


extern void speakLocal(int cnt, const char *txt=NULL);

#if defined(DISABLE_CHARGER)
#define chargerConnected 0
#else

#ifdef DIGITAL_VIN_PIN
#define chargerConnected digitalRead(VIN_PIN)
#else
#define chargerConnected (adc1_get_raw(VIN_CHANNEL) > 500)
#endif

#endif

void setup(void)
{
    Serial.begin(115200);
    setCpuFrequencyMhz(80);

#ifndef DISABLE_BLE
    WiFi.mode(WIFI_MODE_NULL);
#endif
    adc1_config_width(ADC_WIDTH_BIT_12);
#ifdef DIGITAL_VIN_PIN
    pinMode(VIN_PIN,INPUT);
#else
    adc1_config_channel_atten(VIN_CHANNEL,ADC_ATTEN_DB_11);
#endif
    adc1_config_channel_atten(VOLT_CHANNEL,ADC_ATTEN_DB_11);
    adc1_config_channel_atten(CLK_CHANNEL,ADC_ATTEN_DB_6);
    adc1_config_channel_atten(DATA_CHANNEL,ADC_ATTEN_DB_6);
    esp_adc_cal_characterize(
        ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_12,
        1100, &db11_chars);
    initPrefs();
    revertPrefs();
    //getCalibData();
    pinMode(KEY_PIN, INPUT_PULLUP);
#ifndef DISABLE_BLE
    delay(100);
    if (autoBLE) useBLE = true;
    else if (!digitalRead(KEY_PIN)) {
        delay(100);
        if (!digitalRead(KEY_PIN)) useBLE=true;
    }
#endif
    startSecondTask();
    delay(10);
    chargeMode = chargerConnected;
    readMode=(chargeMode) ? RDM_CHARGE : RDM_STILL;
    chargeCnt = 0;
    
    //initKey();
#ifndef DISABLE_BLE
    if (useBLE) {
        wordBuffer[0] = vword_xblu;
        wordBuffer[1] = readModeSpk[readMode];
        speakLocal(2);
    }
    else
#endif
     {
        wordBuffer[0] = readModeSpk[readMode];
        speakLocal(1);
    }
    lastChargeMode = chargeMode;
    lastRead=millis();
    lastChargeTimer = millis();
}

static uint32_t _readBattery()
{
    static uint32_t batsum=0;
    static uint8_t batstep=0;
    static uint32_t batmillis = 0;
    uint32_t curvolt;
    if (millis() - batmillis < 50) return 0;
    batmillis = millis();
    if (esp_adc_cal_get_voltage(ADC_CHANNEL_0,&db11_chars,&curvolt) != ESP_OK) return 0;
    if (batstep == 0) batsum = 0;
    batsum += curvolt;
    batstep = (batstep + 1) & 15;
    if (batstep == 0) return batsum / 16;
    return 0;
    
}

int batvolt = 0;

void readBattery()
{
    int t = _readBattery();
    if (t) batvolt = t;
}

void getStrResult(char *buf, long packet)
{
    static const char *units[]={
        " mm"," in"," ms"};
        
    long data = packet & 0xFFFFF;
    char *cb=buf;
    uint8_t neg = (packet & 0x100000) ? 1 : 0;
    uint8_t inch;
    if (packet & 0x800000) {
        data = data/2;
        inch = (enableMils && data < 1000) ? 2: 1;
        
    }
    else inch=0;
    if (neg) {
        strcpy(buf,"- ");
        buf+=2;
    }
    if (inch == 2) sprintf(buf,"%d ms", data);
    else {
        if (inch) sprintf(buf,"%d . %03d", data/1000, data%1000);
        else sprintf(buf,"%d . %02d", data/100, data%100);
        if (lastInch != inch) {
            strcat(buf, units[inch]);
        }
    }
    lastInch = inch;
}


uint32_t pfsTimer;
uint8_t tmpSpeed, tmpVox, tmpVol;
bool speaking = false;
void speakLocal(int cnt, const char *txt)
{
    qSay(mainVox, mainSpeed, mainVol, cnt, wordBuffer,txt);
    speaking = true;
}

static void vspeakTmp(int cnt, ...)
{
    int i;
    va_list ap;
    va_start(ap, cnt);
    for (i=0;i<cnt;i++) wordBuffer[i] = va_arg(ap, int);
    va_end(ap);
    qSay(tmpVox, tmpSpeed, tmpVol, cnt, wordBuffer);
    speaking = true;
}    

void announceRDM(void)
{
    wordBuffer[0] = readModeSpk[readMode];
    speakLocal(1,readModeTxt[readMode]);
}

void announcePfs(void)
{
    switch (pfsMode) {
        case PFMO_TEMPO:
        vspeakTmp(2, vword_xspeedp, (tmpSpeed+1) * NUMSTEP);
        break;

        case PFMO_VOL:
        vspeakTmp(1, vword_xvol0+tmpVol);
        break;

#if VOICE_COUNT > 1
        case PFMO_VOX:
        vspeakTmp(2,vword_xvtypp,vword_xvox);
        break;
#endif

        case PFMO_MILS:
        vspeakTmp(1, enableMils? vword_xmilson : vword_xmilsof);
        break;
        
        case PFMO_OVER:
        vspeakTmp(1, disablePauses? vword_xoveron : vword_xoverof);
        break;

#ifndef DISABLE_MELODY
        case PFMO_MELO:
        vspeakTmp(1, enableMelody ? vword_xmelon : vword_xmelof);
        break;
#endif
    }
}


void startMamoPfs(void)
{
    mainMode = MAMO_PFS;
    pfsMode = 0;
    tmpSpeed = mainSpeed;
    tmpVox = mainVox;
    tmpVol = mainVol;
    announcePfs();
    pfsTimer = millis();
    
}

void doMamoPfs(uint8_t cmd)
{
    if ((!cmd && (millis() - pfsTimer > 10000)) || cmd == KCMD_LONG) {
        mainMode = MAMO_STD;
#ifndef DISABLE_MELODY
        enableMelody = tmpMelody;
#endif
        disablePauses = tmpPauses;
        enableMils = tmpMils;
        wordBuffer[0] = vword_xcancel;
        speakLocal(1);
        return;
    }
    pfsTimer = millis();
    switch(cmd) {
        case KCMD_DBLE:
        pfsMode = (pfsMode + 1) % PFMO_MAX;
        announcePfs();
        return;
    
        case KCMD_DLONG:
        mainSpeed = tmpSpeed;
        mainVox = tmpVox;
        mainVol = tmpVol;
        storePrefs();
        mainMode = MAMO_STD;
        wordBuffer[0] = vword_xstored;
        speakLocal(1);
        
        //announceRDM();
        return;
        
        case KCMD_SGL:
        switch (pfsMode) {

#if VOICE_COUNT > 1
            case PFMO_VOX:
            tmpVox = (tmpVox+1) % VOICE_COUNT;
            break;
#endif
            case PFMO_VOL:
            tmpVol = (tmpVol+1) % 3;
            break;

            case PFMO_TEMPO:
            tmpSpeed = (tmpSpeed + 1) % 7;
            break;

#ifndef DISABLE_MELODY
            case PFMO_MELO:
            enableMelody = !enableMelody;
            break;
#endif
            case PFMO_MILS:
            enableMils = !enableMils;
            break;

            case PFMO_OVER:
            disablePauses = !disablePauses;
            break;
        }
        announcePfs();
        return;
    }
}

int lastSaidPacket=-1;
int lastKnownPacketValue = PACKET_NO_DATA;
int lastSpokenValue = PACKET_NO_DATA;

void sayPacket(int packet)
{
    char buf[32];
    getStrResult(buf, packet);
    lastSaidPacket=packet;
    lastSpokenValue = lastKnownPacketValue = packetValue(packet);
    mdits(buf);
    qSay(mainVox, mainSpeed,mainVol,  wordBufLen, wordBuffer,blubuffer);
    speaking = true;
}

static void spkOff()
{
    if (millis() - lastSpoken > 20000) {
        wordBuffer[0] = vword_xvoff;
        speakLocal(1);
    }
}

void spkBattery(void) {
    int level = batPercent((batvolt * calibV1) / calibV2);
    makebat(level);
    speakLocal(wordBufLen, blubuffer);
}

#ifndef DISABLE_CHARGER
void chargeCtl(void)
{
    if (millis() - lastChargeTimer < 100) return;
    lastChargeTimer = millis();
    bool cm = chargerConnected;
    if (cm != lastChargeMode) {
        chargeCnt = 0;
        lastChargeMode = cm;
        return;
    }
    if (chargeMode == cm) return;
    chargeCnt++;
    if (chargeCnt < 3) return;
    chargeMode = lastChargeMode = cm;
    if (!chargeMode) {
        if (readMode != RDM_CHARGE) return;
        readMode = RDM_STILL;        
    }
    else {
        if (readMode == RDM_CHARGE) return;
        readMode = RDM_CHARGE;
    }
    announceRDM();
}
#endif
bool canSayChange(long packet)
{
    if (lastKnownPacketValue == PACKET_NO_DATA) return true;
    int pv=packetValue(packet);
    if (abs(lastKnownPacketValue - pv) > 10) {
        lastKnownPacketValue = pv;
        return false;
    }
    if (abs(lastSpokenValue - pv) < 2) return false;
    return true;
}

static void avoidUglyMessage(void)
{
    static uint32_t ds=millis();
    static uint8_t md = 2;
    if (!md) return;
    if (millis() - ds >= 1000) {
        printf("\n"); md--;ds=millis();
    }
}


void loop()
{
    avoidUglyMessage();
    serLoop();
    readBattery();
#ifndef DISABLE_CHARGER
    chargeCtl();
#endif
    uint8_t cmd=getCmd();
    if (cmd) {
        qSay(0,0,0,0,NULL);
        qSay(0,0,0,0,NULL);
        lastSpoken = millis();
    }

    if (speaking && !isSpeaking()) {
        speaking = false;
        lastSpoken = millis();
    }
        
    if (mainMode == MAMO_PFS) {
        doMamoPfs(cmd);
        return;
    }
    if (cmd == KCMD_DBLE) {
        spkBattery();
        return;
    }
    if (cmd == KCMD_DLONG) {
        startMamoPfs();
        return;
    }
    
    if (cmd == KCMD_LONG) {
#ifdef DISABLE_CHARGER
        readMode = (readMode + 1) % RDM_MAX;
#else
        readMode = (readMode + 1) % (chargeMode ? RDM_MAX : RDM_CHARGE);
#endif
        announceRDM();
        return;
    }

    if (readMode == RDM_CHARGE) return;
    
    int packet=getPacket();
    if (packet >= 0) {
        lastRead = millis();
        if (!speaking) switch(readMode) {
            case RDM_CHAN:
            if (cmd == KCMD_SGL || (millis() - lastSpoken >= 1000 && lastSaidPacket != packet && canSayChange(packet))) sayPacket(packet);
            else spkOff();
            break;

            case RDM_REQ:
            if (cmd == KCMD_SGL) sayPacket(packet);
            else spkOff();
            break;

            default:
            if (millis() - lastSpoken >= 1000) {
                sayPacket(packet);
            }
            break;
        }
    }
    else if (!speaking) {
        if (millis() - lastRead > 10000) {
            lastSaidPacket = -1;
            wordBuffer[0] = vword_xnosig;
            lastRead=millis();
            speakLocal(1,STR_NOSIG);
        }
        else spkOff();
    }
    lastKnownPacketValue = packetValue(packet);
    
}
