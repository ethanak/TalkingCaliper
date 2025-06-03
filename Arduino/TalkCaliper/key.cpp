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
#include <freertos/queue.h>


enum {
    KSTA_IGNORE = 0,
    KSTA_PRESSED,
    KSTA_RELEASED,
    KSTA_DPRESSED
};

static hw_timer_t * timer = NULL;

static uint32_t keyITimer;
static uint8_t currentKey, lastKey;
uint32_t pfs_doubleMsec=200, pfs_longMsec=400;


static uint8_t keyState = KSTA_IGNORE;


static IRAM_ATTR uint8_t getIKey(void)
{
    uint32_t ms = millis();
    uint8_t rc = 0;
    if (ms - keyITimer < 30) return 0; // debounce
    currentKey = digitalRead(KEY_PIN);
    if (currentKey == lastKey) {
        if (keyState == KSTA_IGNORE) return 0;
        if (!currentKey) { //pressed
            if (ms - keyITimer >= pfs_longMsec) {
                rc = (keyState == KSTA_PRESSED) ? KCMD_LONG : KCMD_DLONG;
                keyState = KSTA_IGNORE;
            }
        } else { // released
            if (keyState == KSTA_RELEASED && ms - keyITimer >= pfs_doubleMsec) {
                rc=KCMD_SGL;
                keyState = KSTA_IGNORE;
            }
        }
        return rc;
    }
    lastKey = currentKey;
    keyITimer = ms;
    if (!currentKey) { // fell
        keyState = (keyState == KSTA_RELEASED) ? KSTA_DPRESSED : KSTA_PRESSED;
        return 0;
    }
    // rose
    
    if (keyState == KSTA_PRESSED) {
        keyState = KSTA_RELEASED;
    }
    else {
        if (keyState == KSTA_DPRESSED) rc = KCMD_DBLE;
        keyState = KSTA_IGNORE;
    }
    return rc;
}

static xQueueHandle kbdQueue;

static void IRAM_ATTR keyboardInt()
{
    uint8_t t = getIKey();
    if (!t) return;
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xQueueSendFromISR(kbdQueue,&t,&xHigherPriorityTaskWoken);
}

uint8_t getCmd(void)
{
    uint8_t rc = 0;
    BaseType_t brc=xQueueReceive(kbdQueue, (void *)&rc, 0);
    return (brc == pdTRUE) ? rc : 0;
}


bool initKey(void)
{
    kbdQueue = xQueueCreate(10,1);
    lastKey=digitalRead(KEY_PIN);
    timer = timerBegin(1000000);
    timerAttachInterrupt(timer, &keyboardInt);
    keyITimer = millis();
    keyState = KSTA_IGNORE;
    timerAlarm(timer,10000, true, 0);
    timerStart(timer);
    return lastKey == 0;
}
