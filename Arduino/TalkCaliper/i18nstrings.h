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

#ifndef I18NSTRINGS_H
#define I18NSTRINGS_H

#include "i18n.h"

#if defined(VOICE_IS_PL)

// numbers
#define STR_ZERO "zero"
#define STR_MINUS "minus"
#define STR_MILS "mils"
#define STR_MM "milimetra"
#define STR_INCH "cala"
#define STR_IDOT "i"

#define STR_INTRANGE "Podaj liczbę z zakresu %d..%d\n"
#define STR_UNFIN "Nie ukończono pomiaru, spróbuj za chwilę\n"
#define STR_SAVED "Zapisane\n"
#define STR_BALWAYS "zawsze"
#define STR_BDEMAND "na żądanie"
#define STR_FLTR "Filtr: %s\n"
#define STR_FLTR0 "wyłączony"
#define STR_FLTR1 "słaby"
#define STR_FLTR2 "średni"
#define STR_FLTR3 "silny"
#define STR_Y 't'
#define STR_N 'n'
#define STR_YORN "Podaj 't' albo 'n'\n"
#define STR_PFLONG "Czas długiego wciśnięcia %d msec\n"
#define STR_PFDBLE "Odstęp drugiego wciśnięcia %d msec\n"
#define STR_HCAL "Kalibracja"
#define STR_HBLE "Włączenie BLE"
#define STR_HLNG "Czas długigo wciśnięcia"
#define STR_HDBL "Odstęp podwójnego kliknięcia"
#define STR_HHLP "Pomoc"
#define STR_HLCAL "Podaj aktualną wartość napięcia w mV"
#define STR_HLBLE "Wpisz t jeśli BLE ma być zawsze włączony, n jeśli nie"
#define STR_HLLNG "Podaj czas długiego wciśnięcia w msec (300..600)"
#define STR_HLDBL "Podaj odstęp dla podwójnego przyciśnięcia w msec (200..600)"
#define STR_HLHLP "Z parametrem: pomoc dla konkretnego polecenia"
#define STR_HFLT "Filtr dolnoprzepustowy"
#define STR_HLFLT "Podaj moc filtra 0 to 3"
#define STR_HSTR1 "Polecenie bez parametrów podaje aktualne wartości\n"
#define STR_HSTR2 "Wpisz \"help polecenie\" aby uzyskać pomoc dla polecenia\n"
#define STR_HBNAME "Nazwa BLE"
#define STR_HLBNAME "Podaj nazwę pod którą będzie widziane urządzenie"   
#define STR_CURBLE "Aktualna nazwa: %s\n"
#define STR_BADNAME "Błędna nazwa\n"

#define STR_RDSTIL "odczyt ciągły"
#define STR_RDCHAN "odczyt zmian"
#define STR_RDREQU "odczyt na żądanie"
#define STR_RDCHRG "tryb ładowania"
#define STR_NOSIG "brak odczytu"
#define STR_BATTERY "Bateria: %d procent"

//misc
#define STR_BLELANG "*pl\n"

// spanish here
#elif defined VOICE_IS_ES

//numbers - wymawiane przez apkę na komórce
#define STR_ZERO "cero"
#define STR_MINUS "menos"
// jednosylabovy - ma być krótko, a elektronik to zrozumie
// milésimas de pulgada, a nawet samo milésimas jest za długie
#define STR_MILS "mils"
#define STR_MM "milimetros"
#define STR_INCH "pulgadas"
#define STR_IDOT "punto"

// todo: translate into Spanish

#define STR_INTRANGE "Introduzca un número entre %d y %d\n"
#define STR_UNFIN "Medición no completada, inténtelo de nuevo\n"
#define STR_SAVED "Guardado\n"
#define STR_BALWAYS "siempre"
#define STR_BDEMAND "a pedido"
#define STR_FLTR "Low pass filter: %s\n"
#define STR_FLTR0 "off"
#define STR_FLTR1 "weak"
#define STR_FLTR2 "medium"
#define STR_FLTR3 "strong"

#define STR_Y 's'
#define STR_N 'n'
#define STR_YORN "Introduzca 's' o 'n'\n"
#define STR_PFLONG "Tiempo de pulsación larga %d ms\n"
#define STR_PFDBLE "Retardo de doble clic %d ms\n"
#define STR_HCAL "Calibración"
#define STR_HBLE "Bluetooth al iniciar"
#define STR_HLNG "Tiempo de pulsación larga"
#define STR_HDBL "Retardo de doble clic"
#define STR_HFLT "Low pass filter"
#define STR_HLFLT "Enter filter strength 0 to 3"
#define STR_HHLP "Ayuda"
#define STR_HLCAL "Introduzca el voltaje real en mV "
#define STR_HLBLE "Ingrese 's' si BLE debe estar siempre activado, o 'n' si no "
#define STR_HLLNG "Introduzca el tiempo de pulsación larga en ms  (300..600)"
#define STR_HLDBL "Introduzca el retardo del doble clic en ms  (200..600)"
#define STR_HFLT "Low pass filter"
#define STR_HLFLT "Enter filter strength 0 to 3"
#define STR_HLHLP "Con parámetro: ayuda para un comando específico"
#define STR_HSTR1 "El comando sin parámetro muestra el valor real\n"
#define STR_HSTR2 "Ingrese \"help comando\" para obtener ayuda con un comando específico\n"
#define STR_HBNAME "nombre de Bluetooth"
#define STR_HLBNAME "Introduzca el nombre visible de este dispositivo"
#define STR_BADNAME "Nombre incorrecto\n"   
#define STR_CURBLE "Nombre actual: %s\n"

// wymawiane przez apkę na komórce
// powinny być zgodne ze słownikiem w Pythonie

#define STR_RDSTIL "leer continuamente"
#define STR_RDCHAN "leer cambios"
#define STR_RDREQU "leer a pedido"
#define STR_RDCHRG "modo de carga"
#define STR_NOSIG "sin señal"
#define STR_BATTERY "Acumulador: %d por ciento"

//nie ruszać!
#define STR_BLELANG "*es\n"

#elif defined VOICE_IS_DE

//numbers - wymawiane przez apkę na komórce
#define STR_ZERO "0"
#define STR_MINUS "minus"
// jednosylabovy - ma być krótko, a elektronik to zrozumie
// milésimas de pulgada, a nawet samo milésimas jest za długie
#define STR_MILS "mils"
#define STR_MM "Millimeter"
#define STR_INCH "Zoll"
#define STR_IDOT "punkt"

// todo: translate into Spanish

#define STR_INTRANGE "Enter number between %d and %d\n"
#define STR_UNFIN "measurement not completed, try again\n"
#define STR_SAVED "Saved\n"
#define STR_BALWAYS "always"
#define STR_BDEMAND "on demand"
#define STR_FLTR "Low pass filter: %s\n"
#define STR_FLTR0 "off"
#define STR_FLTR1 "weak"
#define STR_FLTR2 "medium"
#define STR_FLTR3 "strong"

#define STR_Y 'y'
#define STR_N 'n'
#define STR_YORN "Enter 'y' or 'n'\n"
#define STR_PFLONG "Long press time %d msec\n"
#define STR_PFDBLE "Double click delay %d msec\n"
#define STR_HCAL "Calibration"
#define STR_HBLE "BLE on start"
#define STR_HLNG "Long press time"
#define STR_HDBL "Double click delay"
#define STR_HFLT "Low pass filter"
#define STR_HLFLT "Enter filter strength 0 to 3"
#define STR_HHLP "Help"
#define STR_HLCAL "Enter actual voltage in mV"
#define STR_HLBLE "Enter 'y' if BL E must be always on, or 'n' if not"
#define STR_HLLNG "Enter long press time in msec (300..600)"
#define STR_HLDBL "Enter double click delay in msec (200..600)"
#define STR_HFLT "Low pass filter"
#define STR_HLFLT "Enter filter strength 0 to 3"
#define STR_HLHLP "With parameter: help for specific command"
#define STR_HSTR1 "Command without parameter shows actual value\n"
#define STR_HSTR2 "Enter \"help command\" for specific command help\n"
#define STR_HBNAME "Bluetooth name"
#define STR_HLBNAME "Enter visible name of this device"
#define STR_BADNAME "Bad name\n"   
#define STR_CURBLE "Current name: %s\n"

// wymawiane przez apkę na komórce
// powinny być zgodne ze słownikiem w Pythonie

#define STR_RDSTIL "alles aussprechen"
#define STR_RDCHAN "Änderungen aussprechen"
#define STR_RDREQU "sprechen auf Anfrage"
#define STR_RDCHRG "Lademodus"
#define STR_NOSIG "kein Signal"
#define STR_BATTERY "Batterie: %d %%"

//nie ruszać!
#define STR_BLELANG "*de\n"

#else

//numbers 
#define STR_ZERO "zero"
#define STR_MINUS "minus"
#define STR_MILS "mils"
#define STR_MM "millimeters"
#define STR_INCH "inch"
#define STR_IDOT "point"

#define STR_INTRANGE "Enter number between %d and %d\n"
#define STR_UNFIN "measurement not completed, try again\n"
#define STR_SAVED "Saved\n"
#define STR_BALWAYS "always"
#define STR_BDEMAND "on demand"
#define STR_FLTR "Low pass filter: %s\n"
#define STR_FLTR0 "off"
#define STR_FLTR1 "weak"
#define STR_FLTR2 "medium"
#define STR_FLTR3 "strong"
#define STR_Y 'y'
#define STR_N 'n'
#define STR_YORN "Enter 'y' or 'n'\n"
#define STR_PFLONG "Long press time %d msec\n"
#define STR_PFDBLE "Double click delay %d msec\n"
#define STR_HCAL "Calibration"
#define STR_HBLE "BLE on start"
#define STR_HLNG "Long press time"
#define STR_HDBL "Double click delay"
#define STR_HHLP "Help"
#define STR_HLCAL "Enter actual voltage in mV"
#define STR_HLBLE "Enter 'y' if BLE must be always on, or 'n' if not"
#define STR_HLLNG "Enter lontg press time in msec (300..600)"
#define STR_HLDBL "Enter double click delay in msec (200..600)"
#define STR_HFLT "Low pass filter"
#define STR_HLFLT "Enter filter strength 0 to 3"
#define STR_HLHLP "With parameter: help for specific command"
#define STR_HSTR1 "Command without parameter shows actual value\n"
#define STR_HSTR2 "Enter \"help command\" for specific command help\n"
#define STR_HBNAME "Bluetooth name"
#define STR_HLBNAME "Enter visible name of this device"
#define STR_BADNAME "Bad name\n"   
#define STR_CURBLE "Current name: %s\n"


#define STR_RDSTIL "reading continuous"
#define STR_RDCHAN "reading changes"
#define STR_RDREQU "reading on demand"
#define STR_RDCHRG "charging mode"
#define STR_NOSIG "no signal"
#define STR_BATTERY "Battery: %d percent"

//misc
#define STR_BLELANG "*en\n"
#endif


#endif
