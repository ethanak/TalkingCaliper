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

static char serbuf[64], *ser;
static int serpos=0;

static const char *getToken(void)
{
    while(*ser && isspace(*ser)) ser++;
    if (!*ser) return NULL;
    char *rc=ser;
    while (*ser && !isspace(*ser)) ser++;
    if (*ser) *ser++=0;
    return rc;
}

static int getLastInt(int mini, int maxi)
{
    int rc;
    while(*ser && isspace(*ser)) ser++;
    if (!*ser) return PRERR_NO_DATA;
    rc=strtol(ser, &ser, 10);
    while(*ser && isspace(*ser)) ser++;
    if (*ser) goto garbage;
    if (rc >= mini && rc <=maxi) return rc;
garbage:
    printf(STR_INTRANGE, mini, maxi);
    return PRERR_OTHER;
}

struct sercmd {
    const char *name;
    const char *descr;
    const char *longdes;
    void (*fun)(void);
};

static void doHelp(void);

static void doCal(void)
{
    char *c=ser;
    if (!batvolt) {
        printf(STR_UNFIN);
        return;
    }
    int v = getLastInt(3000, 4400);
    if (v == PRERR_NO_DATA) {
        printf("batvolt = %d, cal %d/%d\n",batvolt, calibV1, calibV2);
        return;
    }
    if (v < 0) return;
    calibV1=v;
    calibV2=batvolt;
    storeCalibData();
    printf(STR_SAVED);
    
        
}

#ifndef DISABLE_BLE

static void doBle(void)
{
    const char *c = getToken();
    if (!c || !*c) {
        printf("BLE %s\n", autoBLE ? STR_BALWAYS : STR_BDEMAND);
        return;
    }
    if (*c == STR_Y) autoBLE = true;
    else if (*c == STR_N) autoBLE = false;
    else {
        printf(STR_YORN);
        return;
    }
    storeAutoBle();
    printf(STR_SAVED);
}
static void doBlename(void)
{
    while(*ser && isspace(*ser)) ser++;
    if (!*ser) {
        printf(STR_CURBLE, blename);
        return;
    }
    char blbuf[32],*c=blbuf;
    while (*ser) {
        if (c-blbuf >= 31) break;
        if (isspace(*ser)) {
            while(*ser && isspace(*ser)) ser++;
            if (*ser) *c++=' ';
            else break;
            continue;
        }
        if (!isalnum(*ser) && !strchr("().-",*ser)) break;
        *c++=*ser++;
    }
    if (*ser || c-blbuf < 5) {
        printf(STR_BADNAME);
        return;
    }
    strcpy(blename,blbuf);
    storeBleName();
    printf(STR_SAVED);
}


#endif
static void doLng(void)
{
    int ti=getLastInt(300,600);
    if (ti == PRERR_NO_DATA) {
        printf(STR_PFLONG, pfs_longMsec);
        return;
    }
    if (ti < 0) return;
    pfs_longMsec = ti;
    storeLong();
    printf(STR_SAVED);
            
}

static const char *fltstr[] = {
    STR_FLTR0,
    STR_FLTR1,
    STR_FLTR2,
    STR_FLTR3
};
    
static void doFlt(void)
{
    int fl = getLastInt(0,3);
    if (fl == PRERR_NO_DATA) {
        printf(STR_FLTR, fltstr[lpfilter]);
        return;
    }
    if (fl < 0) return;
    lpfilter = fl;
    storeFilter();
    printf(STR_SAVED);
}

static void doDbl(void)
{
    int ti=getLastInt(200,600);
    if (ti == PRERR_NO_DATA) {
        printf(STR_PFDBLE, pfs_doubleMsec);
        return;
    }
    if (ti < 0) return;
    pfs_doubleMsec = ti;
    storeDble();
    printf(STR_SAVED);
}

static const struct sercmd sercmd[]={
    {"cal",STR_HCAL,STR_HLCAL,doCal},
#ifndef DISABLE_BLE
    {"ble",STR_HBLE,STR_HLBLE,doBle},
    {"name",STR_HBNAME,STR_HLBNAME,doBlename},
#endif
    {"long",STR_HLNG,STR_HLLNG,doLng},
    {"dbl",STR_HDBL,STR_HLDBL,doDbl},
    {"fltr",STR_HFLT,STR_HLFLT, doFlt},
    {"help",STR_HHLP,STR_HLHLP,doHelp},
    {NULL,NULL,NULL}
};

static void doHelp(void)
{
    const char *c=getToken();
    int i;
    if (c && *c) {
        for (i=0;sercmd[i].name;i++) if (!strcmp(c,sercmd[i].name)) break;
        if (sercmd[i].name) {
            printf("%s - %s\n",sercmd[i].name, sercmd[i].longdes);
            return;
        }
           
    }
    for (i=0;sercmd[i].name;i++) {
        printf("%s - %s\n",sercmd[i].name, sercmd[i].descr);
    }
    printf(STR_HSTR1);
    printf(STR_HSTR2);
}



static void serCmd(void)
{
    serbuf[serpos]=0;
    ser=serbuf;
    const char *cmd=getToken();
    if (!cmd) return;
    int i;
    for (i=0;sercmd[i].name;i++) if (!strcmp(sercmd[i].name,cmd)) {
        sercmd[i].fun();
        return;
    }
    *ser=0;
    doHelp();
}


void serLoop(void)
{
    while(Serial.available()) {
        int znak = Serial.read();
        if (znak == '\r' || znak == '\n') {
            if (!serpos) continue;
            serCmd();
            serpos = 0;
        }
        if (serpos < 63) serbuf[serpos++] = znak;
    }
}
