#include <Arduino.h>
#include <driver/adc.h>
#include <esp_adc_cal.h>
#include "common.h"

// Original code: https://github.com/MGX3D/EspDRO

// timeout for a packet read 
#define PACKET_READ_TIMEOUT 150

// Packet format: [0 .. 19]=data, 20=sign, [21..22]=unused?, 23=inch
#define PACKET_BITS       24

// ADC threshold for 1.5V SPCsignals (at 6dB/11-bit, high comes to around 1570 in analogRead() )
// or 390 ad 6dB/9-bit raw read
//#define ADC_TRESHOLD 800
#define ADC_TRESHOLD 1600

// timeout in milliseconds for a bit read ($TODO - change to micros() )
#define BIT_READ_TIMEOUT 50


static int ipos,vdl[32];
static int getBit() {
    int data;

    int readTimeout = millis(); //+ BIT_READ_TIMEOUT;
    while (adc1_get_raw(CLK_CHANNEL) > ADC_TRESHOLD) {
      if (millis() - readTimeout > BIT_READ_TIMEOUT)
        return -1;
    }
    while (adc1_get_raw(CLK_CHANNEL) < ADC_TRESHOLD) {
      if (millis() - readTimeout > BIT_READ_TIMEOUT)
        return -1;
    }
    int nda = adc1_get_raw(DATA_CHANNEL);
    vdl[ipos] = nda;
    ipos = (ipos+1) & 31;
    data = (nda > ADC_TRESHOLD)?1:0;
    
    return data;
}

// read one full packet
long getPacket() 
{
    long packet = 0;
    int readTimeout = millis();// + PACKET_READ_TIMEOUT;

    int bitIndex = 0;
    while (bitIndex < PACKET_BITS) {
      int bit = getBit();
      if (bit < 0 ) {
        // bit read timeout: reset packet or bail out
        if (millis() - readTimeout > PACKET_READ_TIMEOUT) {
          // packet timeout
          return -1;
        }
        
        bitIndex = 0;
        packet = 0;
        continue;
      }

      packet |= (bit & 1)<<bitIndex;
      
      bitIndex++;
    }
    
    return packet;
}

int packetValue(long packet)
{
    if (packet < 0) return PACKET_NO_DATA;
    long data = packet & 0xFFFFF;
    uint8_t neg = (packet & 0x100000) ? 1 : 0;
    if (packet & 0x800000) data = data / 2;
    if (packet & 0x100000) data = -data;
    return data;
}
