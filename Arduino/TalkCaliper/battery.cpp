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

static const uint16_t chartv[]={
    3270,3610,3690,3710,3730,
    3750,3770,3790,3800,3820,
    3840,3850,3870,3910,3950,
    3980,4020,4080,4110,4150,
    4200};

uint8_t batPercent(uint16_t voltage)
{
    if (voltage >= 4200) return 100;
    if (voltage <= chartv[0]) return 0;
    int i;
    for (i=0; i<20; i++) {
        if (voltage < chartv[i+1]) break;
    }
    return  (5 * (voltage - chartv[i])) / (chartv[i+1] - chartv[i]) + 5 * i;
}

    
