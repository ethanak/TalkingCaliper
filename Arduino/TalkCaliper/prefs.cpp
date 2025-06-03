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
#include <Preferences.h>
#include "i18n.h"
#include "common.h"
#include VOXDEFS
Preferences prefs;

void initPrefs(void)
{
    prefs.begin("rhspeak",false);
}


void getCalibData(void)
{
    uint16_t v1,v2;
    v1=prefs.getUShort("voltr1",0);
    v2=prefs.getUShort("voltr2",0);
    if (v1 && v2) {
        calibV1 = v1;
        calibV2 = v2;
    }
    else {
        calibV1 = 2030;
        calibV2 = 4170;
    }
}

void storeCalibData(void)
{
    prefs.putUShort("voltr1",calibV1);
    prefs.putUShort("voltr2",calibV2);
}

void revertPrefs(void)
{
    mainSpeed = prefs.getUChar("speed",0);
    if (mainSpeed > 9) mainSpeed=0;
#if VOICE_COUNT > 1
    mainVox = prefs.getUChar("vox",0);
    if (mainVox >= VOICE_COUNT)
#else
        mainVox=0;
#endif
#ifndef DISABLE_MELODY
    enableMelody = prefs.getBool("melo", true);
#else
    enableMelody = false;
#endif
    disablePauses = prefs.getBool("olap", false);
    enableMils = prefs.getBool("mils", false);
    mainVol = prefs.getUChar("vol",1);
#ifndef DISABLE_BLE
    autoBLE = prefs.getBool("able",false);
    prefs.getString("bname",blename,23);
    if (!blename[0]) strcpy(blename,"Caliper");
#endif
    lpfilter=prefs.getUChar("fltr",0);
    if (lpfilter > 3) lpfilter = 0;
    pfs_doubleMsec = prefs.getUShort("kdbl",200);
    pfs_longMsec = prefs.getUShort("klng",400);
    getCalibData();
}

void storeFilter(void)
{
    prefs.putUChar("fltr",lpfilter);
}

void storeDble(void)
{
    prefs.putUShort("kdbl",pfs_doubleMsec);
}

void storeLong(void)
{
    prefs.putUShort("klng",pfs_longMsec);
}


void storePrefs(void)
{
    prefs.putUChar("speed",mainSpeed);
#if VOICE_COUNT > 1
    prefs.putUChar("vox",mainVox);
#endif
    prefs.putUChar("vol",mainVol);
#ifndef DISABLE_MELODY
    prefs.putBool("melo", enableMelody);
#endif
    prefs.putBool("olap", disablePauses);
    prefs.putBool("mils", enableMils);
}

#ifndef DISABLE_BLE
void storeAutoBle(void)
{
    prefs.putBool("able",autoBLE);
}

void storeBleName(void)
{
    prefs.putString("bname",blename);
}

#endif
