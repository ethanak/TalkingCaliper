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

#ifndef DISABLE_BLE
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <ctype.h>
#include "common.h"
#include "i18nstrings.h"

#define SERVICE_UUID           "6E400001-B5A3-F393-E0A9-E50E24DCCA9E"  // UART service UUID
#define CHARACTERISTIC_UUID_RX "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_TX "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"

static BLEServer *pServer = NULL;
static BLECharacteristic *pTxCharacteristic;
static bool deviceConnected = false;
static bool oldDeviceConnected = false;
static uint32_t connTime;
static bool bleSpeaking=false;
static uint8_t bleuCtl=0;
static uint8_t txString[256];
static int msgPos;
static uint8_t mustWrite;
static uint8_t mStatus;
static uint32_t bleTimer;
bool devReady;
bool mustSetLang;


class MyServerCallbacks : public BLEServerCallbacks {
    void onConnect(BLEServer *pServer) {
        deviceConnected = true;
        printf("Connected\n");
        connTime = millis();
    //nyString();
    
    };

    void onDisconnect(BLEServer *pServer) {
        deviceConnected = false;
        bleSpeaking=false;
        mStatus = BLES_ERROR;
        printf("Disconnected\n");
    }
};


class MyCallbacks : public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
        String rxValue = pCharacteristic->getValue();
        int l=rxValue.length();
        char rxbuf[16];
        if (l > 0) {
            const char *c=rxValue.c_str();
            //printf("Received Value: %s\n",c);
            if (c[0] == 'f' && (c[1] == bleuCtl|| c[1] == '?')) {
                bleSpeaking = false;
                mStatus = BLES_SUCCESS;
                //printf("Finished speak\n");
                return;
            }
        }
    }
};


void startBlue(const char *name)
{
    BLEDevice::init(name);
    // Create the BLE Server
    pServer = BLEDevice::createServer();
    pServer->setCallbacks(new MyServerCallbacks());
    // Create the BLE Service
    BLEService *pService = pServer->createService(SERVICE_UUID);
    // Create a BLE Characteristic
    pTxCharacteristic = pService->createCharacteristic(CHARACTERISTIC_UUID_TX, BLECharacteristic::PROPERTY_NOTIFY);
    pTxCharacteristic->addDescriptor(new BLE2902());    
    BLECharacteristic *pRxCharacteristic = pService->createCharacteristic(CHARACTERISTIC_UUID_RX, BLECharacteristic::PROPERTY_WRITE);
    pRxCharacteristic->setCallbacks(new MyCallbacks());
    pService->start();
    bleuCtl=0;
    pServer->getAdvertising()->start();
    mStatus = BLES_READY;
}

static int computeMsgLen(void)
{
    const uint8_t *c=(const uint8_t *)txString + msgPos;
    int n;
    for (n=0;n<20 && c[n];n++) {
        uint8_t z = c[n];
        if ((z & 0xc0) == 0xc0 && n>=17) break;
    }
    return n;
}


bool bleSay(const char *text)
{
    if (!devReady) return false;
    if (bleuCtl <'a' || bleuCtl >='z') bleuCtl='a';
    else bleuCtl++;
    txString[0]=bleuCtl;
    uint8_t *d=txString+1;
    bool spa=false;
    while (*text && isspace(*text)) text++;
    for (;*text;text++) {
        if (isspace(*text)) spa=true;
        else {
            if (spa) {
                *d++=' ';
                spa=false;
            }
            *d++=*text;
        }
    }
    *d++='\n';
    *d=0;
    msgPos=0;
    mustWrite = 1;
    mStatus = BLES_SPEAKING;
    bleSpeaking = true;
    return true;
}

uint8_t bleSpkLoop(void)
{
    bleCtlLoop();
    if (mustSetLang && devReady) {
        pTxCharacteristic->setValue((uint8_t*)STR_BLELANG, 4);
        pTxCharacteristic->notify();
        delay(10);  // bluetooth stack will go into congestion, if too many packets are sent
        mustSetLang = false;
        //printf("Sent BLA %s\n", STR_BLELANG);
    }
    if (mustWrite) {
        if (!devReady) {
            bleSpeaking = false;
            return mStatus = BLES_ERROR;
        }
        int n=computeMsgLen();
        mStatus = BLES_SPEAKING;
        pTxCharacteristic->setValue(txString+msgPos, n);
        pTxCharacteristic->notify();
        delay(10);  // bluetooth stack will go into congestion, if too many packets are sent
        msgPos+=n;
        if (!txString[msgPos]) {
            mustWrite=0;
            msgPos=0;
        }
        bleTimer = millis();
    }
    else if (mStatus == BLES_SPEAKING && millis() - bleTimer > 8000) {
        mStatus = BLES_ERROR;
    }
    return mStatus;
}

void bleCtlLoop(void)
{
    if (deviceConnected != oldDeviceConnected) {
        devReady = false;
        oldDeviceConnected = deviceConnected;
    }
    else if (deviceConnected && !devReady && millis() - connTime > 500) {
        pServer->startAdvertising();
        devReady = true;
        mustSetLang = true;
    }
}
#endif
