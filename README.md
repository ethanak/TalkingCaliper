# TalkingCaliper

A multilingual speaking caliper attachment.

## Short description

![general view](/images/vfront.jpg)

Short movie: [YouTube video](https://youtu.be/fLb1sWjNS7g)

The device is an attachment for the popular Vorel caliper, it can also be used in all measuring devices of this company with a compatible interface (tire tread depth gauge, dial gauge, etc.). In the case of the caliper, it is mounted on the rear wall of the housing in a way that does not interfere with its normal use.

The attachment allows for convenient use of the caliper;

* by blind and visually impaired people
* in poor lighting conditions
* in situations where observing the display is difficult or impossible

In contrast to previous designs using the full TTS mechanism (and thus limited to one specific language), a corpus synthesizer was used here. Due to the relatively small number of messages, it is possible to pre-record them in virtually any language. The program code allows you to select (during compilation) one of four languages:

* English (RHVoice, two voices bdl and slt);
* Polish (RHVoice, two voices magda and natan)
* Spanish (RHVoice, one voice mateo)
* German (Piper-TTS, one voice thorsten)

It is also possible to use an Android phone and use the phone's built-in TTS system, but it is not very convenient (especially for the first group of users)

The device can work in the following modes:

* continuous reading - current measurement results are provided with a one-second break
* reading changes - only measurement results are provided if the value has changed or a reading has been forced.
* reading on demand - after pressing a key.

The units (millimeters or inches) depend on the settings of the caliper itself. Each first reading ends with the unit being provided, the next ones are the same numbers. Since the result in millimeters always contains two decimal digits, in inches three, the reading is always unambiguous. Additionally, it is possible to set the reading option in thousandths of an inch for results below one inch (the unit is mil), in order to shorten the voice message without losing its intelligibility.

## Operation

The device has only one button. The options are as follows

* single click - in the change reading mode or on demand, it reads the current result;
* hold - changes the reading mode;
* double click - reads the battery status;
* click and hold - enters the settings mode.

In the settings mode:

* single click - change value
* hold - exit the settings mode without saving and return to the previous settings
* double click - change the parameter
* click and hold - save.

The following parameters can be set:

* speech speed (seven options, from absurdly slow to very fast);
* volume (three options);
* voice (only if there is more than one);
* way of reading small values;
* separation of digits when reading.

Additionally, after connecting the charger, the charging mode is started, where the device does not say anything. You can exit it by holding the key.

## Program

The program is currently only adapted to the XIAO ESP32S3 board.
Used in program:

* sonic_lite from library [Sonic](https://github.com/waywardgeek/sonic/)
* Piece of code for reading caliper data from  [EspDRO](https://github.com/MGX3D/EspDRO)

### Requirements:

* Arduino IDE with ESP32 Board version 3.1 or later installed
* ESP8266Audio library installed

### IDE settings:

- Selected board: XIAO ESP32-S3
- CPU Frequency: 80 MHz
- Partition Scheme: Maximum APP (7.9 MB APP, No OTA/No FS)

### Program settings:

In the common.h file:

```c
//Uncomment if you don't have/don't need the BlueTooth application
//#DISABLE_BLE

//Uncomment if you don't need charging control
//#DISABLE_CHARGER
```

In the i18n.h file:

```c

//Uncomment for language Polish
//#define VOICE_IS_PL
```

All other VOICE_IS definitions should be commented out.

For other languages:

* for English comment out all VOICE_IS definitions
* for others uncomment only the appropriate definition.

## Electronics

### Required elements

* XIAO ESP32-S3
* MAX98357A module
* Small speaker (originally used 15x24x4 1W)
* LiPo battery min. 250 mAh
* 2 x Schottky diode (min. 0.5A)
* 2 x rectifier diode (the smallest, any parameters) to power the caliper
* tact switch button
* small power switch
* resistors, capacitors, mounting elements according to the diagram

### Test version

![simplified version diagram](/images/testsch.png)

This version is used only to test the program's cooperation with the caliper. It requires only three additional components beyond the main Xiao board and the amplifier (two capacitors and a button), but can be used both to evaluate the usability of the program and to work on introducing a new voice or language. The circuit can be assembled on a small (170 holes) breadboard.

![example breadboard layout](/images/miniboard.png)

In this version, the caliper must be powered by an internal battery. When compiling, uncomment

```c
#DISABLE_CHARGER
```

in the common.h file so that the program does not try to check the charger connection to a non-existent battery.

Since there is no battery, the command to check the battery status will generate incorrect readings, but the rest will work.

### Base version

![full version diagram](/images/fullsch.png)

The simplest one that is realistically possible to make a working device. It differs from the previous one in that the amplifier power is taken either directly from the battery or (in the case of a USB connection) from the Vin input. Additionally, a battery status meter is already connected.

As before, the charger connection detection must be disabled.

Additional elements: two Schottky diodes, two resistors, optional capacitor.

Instead of Schottky diodes, you can use rectifier diodes of min. 0.5A as a last resort,
but you have to take into account that the voltage supplying the I2S amplifier circuit
may drop below the minimum required value of 2.5V with a partially discharged battery.

### Caliper power supply from the attachment battery

If the attachment is not permanently attached to the caliper, this should not be used and the caliper should be powered from the internal battery. In the case of a permanent connection, this should be considered: as a rule, the time needed to recharge the battery is at least an order of magnitude shorter than the time needed to find a battery store and buy one. And as we know, batteries discharge at the least expected moment... In addition, the system (including the caliper) can then be powered by any charger (if the battery is discharged).

Cost: two rectifier diodes, two resistors, connecting all four wires to the caliper plug. Additionally, approx. 1 mA of current draw from the battery.

It should be impossible to insert a battery into the caliper!

### Charging sensor

Allows the device to automatically switch to silent mode (it is not advisable for the device to talk during charging).

Cost: two resistors.

Comment required

```c
#DISABLE_CHARGER
```

in common.h file

### Notes

To calibrate the battery status sensor, access to gnd and the +bat pad is required. While this is trivial before installing the attachment (+bat is available on the switch leads, gnd in several places) - after installation, this is no longer possible. Remember this and calibrate before permanently installing the attachment.

## Mechanical design

### Caliper plug

The project is a modified version of the plug from [EspDRO](https://github.com/MGX3D/EspDRO).
You can also use the original version.

The plug should be printed from TPU or similar elastomer [STL file](/connector/mini_connector.jpg),
preferably with a 0.1 mm layer. The way to attach the cable is shown in the pictures in the [Connector](/connector/) folder.

### Housing

Due to the possibility of using different elements and configurations, it is not possible to include STL files. The attached file [OpenSCAD](/caliper.scad) can be a basis for designing the housing

## Other voices and languages.

It is possible to implement other voices (e.g. using commercial
synthesizers), as well as other languages. More information in the folder
[voice preparation](/prepare).
