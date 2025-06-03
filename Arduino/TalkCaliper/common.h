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

#ifndef XICOMMON_H
#define XICOMMON_H

// settings

// uncomment to disable bluetooth
//#define DISABLE_BLE


// uncomment to disable charge control
//#define DISABLE_CHARGER

// pins
#define KEY_PIN 9

#define _bclk_pin 7
#define _wclk_pin 8
#define _dout_pin 44


//#define DIGITAL_VIN_PIN

#ifdef DIGITAL_VIN_PIN
#define VIN_PIN 4
#else
#define VIN_CHANNEL ADC1_CHANNEL_3
#endif

#define VOLT_CHANNEL ADC1_CHANNEL_0
#define VOLT_GCHAN ADC_CHANNEL_0
#define CLK_CHANNEL ADC1_CHANNEL_2
#define DATA_CHANNEL ADC1_CHANNEL_1

// keys

enum {
    KCMD_SGL = 1,
    KCMD_LONG,
    KCMD_DBLE,
    KCMD_DLONG,
    KCMD_SEC
};

extern uint8_t getCmd(void);
extern bool initKey(void);

// audio
extern const uint8_t * getVoxData(int wordNum, int voice, int *len);
extern void initCSG(void);
extern bool startCSG(int *words, int count);
extern void setSpeed(int n);
extern void setVoice(int n);
extern void stopSpeech(void);
extern bool loopCSG(void);

extern void startSecondTask(void);
extern void qSay(int voice, int speed, int volume,
    int count, int *wordlist, const char *btext = NULL);
extern bool isSpeaking(void);


struct speechData {
    uint8_t wordCnt; // aka command
    uint8_t param;
    uint8_t wordList[32];
    char text[128];
};

// ble
#ifndef DISABLE_BLE
enum {
    BLES_READY=0,
    BLES_SPEAKING,
    BLES_SUCCESS,
    BLES_ERROR
};

extern void startBlue(const char *name);
extern uint8_t bleSpkLoop(void);
extern void bleCtlLoop(void);
extern bool bleSay(const char *text);
extern bool useBLE;
#endif

// misc
extern bool mdits(const char *arg);
extern void makebat(int perc);
extern void melody(int fra, int til, int lo);
extern long getPacket() ;
extern uint8_t batPercent(uint16_t voltage);
extern void serLoop(void);

// prefs
extern void initPrefs(void);
extern void getCalibData(void);
extern void storeCalibData(void);
extern void revertPrefs(void);
extern void storePrefs(void);
extern void storeAutoBle(void);
extern void storeDble(void);
extern void storeLong(void);
extern void storeBleName(void);
extern void storeFilter(void);
#define PRERR_NO_DATA -1
#define PRERR_OTHER -2

// globals
extern bool enableMelody, disablePauses, enableMils;
extern uint16_t calibV1, calibV2;
extern uint8_t mainSpeed, mainVox, mainVol,lpfilter;
extern uint32_t pfs_doubleMsec, pfs_longMsec;

#ifndef DISABLE_BLE
extern char blename[32];
extern bool autoBLE;
#endif

extern int batvolt;

extern int packetValue(long packet);

#define PACKET_NO_DATA 0x1000001

#endif
