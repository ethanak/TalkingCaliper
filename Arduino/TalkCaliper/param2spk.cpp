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
#include "common.h"
#include "i18nstrings.h"
#include VOXDEFS

extern int wordBuffer[32],wordBufLen;
#ifndef DISABLE_BLE
extern char blubuffer[256];
char *bluptr;

void blustr(const char *str)
{
    if (bluptr != blubuffer) *bluptr++=' ';
    strcpy(bluptr,str);
    bluptr+=strlen(bluptr);
}

void blunr(int *dts, int dnr)
{
    if (bluptr != blubuffer) *bluptr++=' ';
    while (dnr--) *bluptr++=*dts++ + '0';
    *bluptr=0;
}

#endif

static bool mkdit(const char ** arg, bool noble=false)
{
    int i,dts[3],dnr;
    for (dnr=0;;dnr++,(*arg)++) {
        if (!*arg || !isdigit(**arg)) break;
        if (dnr >= 3) return false;
        dts[dnr] = (**arg) - '0';
    }
    if (!dnr) return false;
    i=0;
    while (i < dnr && !dts[i]) {
#ifndef DISABLE_BLE
        if (!noble) blustr(STR_ZERO);
#endif
        wordBuffer[wordBufLen++] = 0;
        i++;
        continue;
    }
    if (i >= dnr) return true;
#ifndef DISABLE_BLE
    if (!noble) blunr(dts+i, dnr-i);
#endif
    if (dnr-i == 3) {
        wordBuffer[wordBufLen++] =  NUMSTEP * (dts[i] + vword_100 -1);
        i++;
        if (dts[1] == 0 && dts[2] == 0) return true;
    }
    if (dnr-i == 2 && dts[i] == 0) i++;
    if (dnr-i == 2) {
        if (dts[i] == 1) {
            wordBuffer[wordBufLen++] = NUMSTEP * (dts[i+1] + 10);
            return true;
        }
// >= 20
#if defined FULL_100
        int n = dts[i] * 10 + dts[i+1];
        if (disablePauses) {
            wordBuffer[wordBufLen++] = NUMSTEP * n;
            return true;
        }
        if (n < 20) {
            wordBuffer[wordBufLen++] = NUMSTEP * n;
            return true;
        }
                
            
#if defined(VOICE_IS_DE)
        if (dts[i+1]) {
            wordBuffer[wordBufLen++] = NUMSTEP * dts[i+1];
            wordBuffer[wordBufLen++] = NUMSTEP * vword_xunds;
        }
        wordBuffer[wordBufLen++] = NUMSTEP * (dts[i] * 10);
        return true;
#elif defined(VOICE_IS_ES)
        wordBuffer[wordBufLen++] = NUMSTEP * (10 * dts[i]) ;
        if (dts[i+1] != 0) {
            wordBuffer[wordBufLen++] = NUMSTEP * vword_yy;
        }
        i += 1;
#else        
        wordBuffer[wordBufLen++] = NUMSTEP * (10 * dts[i]) ;
        i += 1;
#endif

#else // only simple numbers

#if defined(VOICE_IS_DE)

        if (dts[i+1]) {
            wordBuffer[wordBufLen++] = NUMSTEP * dts[i+1];
            wordBuffer[wordBufLen++] = NUMSTEP * vword_xunds;
        }
        wordBuffer[wordBufLen++] = NUMSTEP * (dts[i] -2 + vword_20);
        return true
#elif defined(VOICE_IS_ES)
        if (dts[i+1] == 0) {
            wordBuffer[wordBufLen++] = NUMSTEP * (dts[i] -2 + vword_20);
        }
        else if (dts[i] == 2) {
            wordBuffer[wordBufLen++] = NUMSTEP * (dts[i+1] - 1 + vword_21);
            i += 1;
        }
        else {
            if (!disablePauses) {
                wordBuffer[wordBufLen++] = NUMSTEP * (dts[i] + vword_30y - 3);
            }
            else {
                wordBuffer[wordBufLen++] = NUMSTEP * (dts[i] + vword_20 - 2);
                wordBuffer[wordBufLen++] = NUMSTEP * vword_yy;
            }
        }
        i += 1;
#else
        wordBuffer[wordBufLen++] = NUMSTEP * (dts[i] -2 + vword_20);
        i++;
#endif // voice_is
#endif // full 100
    }
    if (dnr-i == 1 && dts[i] != 0) {
        wordBuffer[wordBufLen++] = NUMSTEP * dts[i];
    }
    return true;
} 

#ifndef DISABLE_MELODY
void melody(int fra, int til, int lo)
{
    extern bool enableMelody;
    if (!enableMelody) return;
    if (!lo) til --;
    for (;til>=fra;til -=2) {
        wordBuffer[til] += 1;
    }
}

#endif

void makebat(int perc)
{
    char bts[8];
#ifndef DISABLE_BLE
    bluptr = blubuffer;
    sprintf(blubuffer,STR_BATTERY, perc);
#endif

    wordBuffer[0] = vword_xbatts;
    wordBufLen = 1;
    sprintf(bts, "%d", perc);
    const char *c=bts;
    mkdit(&c, true);
    wordBuffer[wordBufLen++] = vword_xperc;
    
}

bool mdits(const char *arg)
{
    wordBufLen=0;
#ifndef DISABLE_BLE
    bluptr = blubuffer;
#endif
    int mel=0;
    if (*arg == '-') {
        arg++;
#ifndef DISABLE_BLE
        blustr(STR_MINUS);
#endif
        wordBuffer[wordBufLen++] = vword_xmins;
    }
    while (*arg && isspace(*arg)) arg++;
    if (!mkdit (&arg)) return false;
    while (*arg && isspace(*arg)) arg++;
    if (!*arg) {
#ifndef DISABLE_MELODY
        melody(mel,wordBufLen-1,0);
#endif
        return true;
    }
    if (!strcmp(arg,"ms")) {
#ifndef DISABLE_BLE
        blustr(STR_MILS);
#endif

#ifndef DISABLE_MELODY
        melody(mel,wordBufLen-1,1);
#endif
        wordBuffer[wordBufLen++] = vword_xmil;
        return true;
    }
    if (*arg != '.') return false;
#ifndef DISABLE_BLE
    blustr(STR_IDOT);
#endif
    arg++;
#ifndef DISABLE_MELODY
    melody(mel,wordBufLen-1,1);
#endif
    wordBuffer[wordBufLen++] = vword_xi;
    while (*arg && isspace(*arg)) arg++;
    mel = wordBufLen;
    if (!mkdit(&arg)) return false;
    while (*arg && isspace(*arg)) arg++;
    if (!*arg) {
#ifndef DISABLE_MELODY
        melody(mel,wordBufLen-1,0);
#endif
        return true;
    }
    if (!strncmp(arg,"mm",2)) {
        wordBuffer[wordBufLen++] = vword_xmm;
#ifndef DISABLE_BLE
        blustr(STR_MM);
#endif
    }
    else if (!strncmp(arg,"in",2)) {
        wordBuffer[wordBufLen++] = vword_xinch;
#ifndef DISABLE_BLE
        blustr(STR_INCH);
#endif
    }
    else return false;
#ifndef DISABLE_MELODY
    melody(mel, wordBufLen-1,0);
#endif
    return true;
}

