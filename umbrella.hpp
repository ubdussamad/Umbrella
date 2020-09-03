/*
    File: umbrella.hpp
    Parent Suite Header for Umbrella Firmware.
    Part of Project Umbrella
    V-1.0 REF 20JUN2020
    Author: Ubdussamad <ubdussamad@gmail.com>
    This Piece of Software is NOT for public sharing.
    Copyright (c) 2020, ubdussamad@gmail.com
    All rights reserved.
*/
#ifndef UMBRELLA_H
#define UMBRELLA_H

#if (!COMPILING)
#include <Arduino.h>
#endif
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>


/* This block is optional and could be commented out during compilation. */
// #define U8X8_USE_PINS "HI"
// #define ARDUHAL_LOG_LEVEL_VERBOSE 20

#if (ENABLE_GSR)
#include "Gsr.hpp"
#endif

#if (ENABLE_HR)
#include "MAX30100_PulseOximeter.h"
#endif

#if (ENABLE_OLED)
#include <Wire.h>
#include <U8g2lib.h>
#endif

/* Block for mapping BLE Characteristic UUIDs to relevent macros.  */
#define DEVICE_INFO_SERIVCE                    "180A" // SiG Standard
#define BATTERY_SERVICE_UUID                   "180F" // SiG Standard

#define HR_SERVICE_UUID                        "180D" // SiG Standard Heart Rate Service
#define HR_CHARACTERISTIC_UUID                 "2A37" // SiG Standard Heart Rate Measurement Serivce
#define HR_CP_CHARACTERISTIC_UUID              "2A39" // SiG Standard Heart rate Control Point

#define BODY_TEMP_SERVICE_UUID                 "1809" // SiG Standard , Health Thermometer Service
#define BODY_TEMP_CHARACTERISTIC_UUID          "2A1C" // SiG Standard , Health Thermometer Charac..
#define BODY_TEMP_T_DESC_CHARACTERISTIC_UUID   "2A1F" // Temperature Celsius Descriptor
#define BODY_TEMP_T_DESIG_CHARACTERISTIC_UUID  "2A1D" // Temp type Descriptor

#define GSR_SENSOR_SERVICE_UUID                "4fafc201-1fb5-459e-8fcc-c5c9c331914b" // Custom UUID for GSR Sensor
#define GSR_SENSOR_CHARACTERISTIC_UUID         "248efce7-3ccc-48ee-ba7f-17a2e891266b" // Custom Characteristic UUID

/* ------------ NOTE --------------------
All the values of characteristic(s) which are defined 
by SiG are in the order of LSO to MSO.
Where LSO = Least Significant Octet and MSO = Most Significant Octet. 

For Example if a value having a width greater than 1byte (8bit)
will be represented as follows:

Lets say we have a Heart rate Charateristic value in the order:
FLAGS (8BIT), Heart Rate(8BIT), Energy Expended(16BIT) , RR (16BIT)

Lets Assume some values according to their sizes:

 FLAGS (8BIT), Heart Rate(8BIT), Energy Exp(16BIT), RR (16BIT)
 194         , 80 (BPM)        , 17 Kilo Joules   , 11528 milliseconds   | Decimal Values
 11000010    , 01010000        , 0000000000010001 , 0010110100001000     | Binary Values
 C2          , 50              , 0011             , 2d08                 | Hex Values

Now Since we could only send an 8bit unisgned int (1Bytes) array,
we have to split a 16bit(2bytes) octet into 2 Consecutive octets in an array.

For example, say we have 8fe2 we represent it in an array as {8f,e2}.
This way of represnting the larger octet (8f) first and the smaller octet
last is called Big Endian Order (MSO to LSO)

But, In case of BLE standard we send it in the Big-Endian order (LSO to MSO).
So, we represent the hex number 8fe2 as {e2, 8f}.

Thus the array we send for out heart rate characteristic data will look like:

FLAGS (8BIT)   , Heart Rate(8BIT), Energy Exp(16BIT), RR (16BIT)
 { C2          , 50              , 11,   00         , 08,    2d }
   8BIT        , 8BIT            , LSO,  MSO        , LSO,   MSO

   To many, this'd look unintuitive and unnatural, but some systems do
   work this way so do we have to work this way too :(.
   
*/

/* Block for defining BLE properties macros. */
#define BLE_NOTIFY    BLECharacteristic::PROPERTY_NOTIFY
#define BLE_READ      BLECharacteristic::PROPERTY_READ
#define BLE_WRITE     BLECharacteristic::PROPERTY_WRITE
#define BLE_INDICATE  BLECharacteristic::PROPERTY_INDICATE


/* Block for mapping pinout to relevant macros.*/
#define ADC0_CH1      36
#define GSR_PWR       18
#define LED_RED       32
#define LED_BLUE      33 /* Not functional due to Hardware Reasons */
#define I2C_SDA       21
#define I2C_SCL       22
#define FLASH_BTN     0
#define ADC_VOLTAGE   3.3


/* Constructor Block for diffrent peripherals.  */
#if (ENABLE_GSR)
auto sensorGsr =  gsr(ADC0_CH1 , ADC_VOLTAGE , GSR_PWR );
#endif
#if (ENABLE_OLED)
U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2 ( U8G2_R2 , U8X8_PIN_NONE, I2C_SCL , I2C_SDA );
#endif
#if (ENABLE_HR)
PulseOximeter pulseOx;
#endif


/* Namespace Contaning system wide Vars  */
namespace umbrellaSysVars {

/* System wide counter. */
long int sysNotifyCounter = 0;


/* This block contains Global system event flags. */
bool isConnected(false);
bool hrInitFailed(false);

/* This block decalers global constructor handles for BLE characteristics/Services and Servers. */
BLEServer *umbrellaServer;
BLECharacteristic *hrCharacteristic;
BLECharacteristic *gsrCharacteristic;
BLECharacteristic *TempCharacteristic;
BLECharacteristic *GyroCharacteristic;

}

#if (ENABLE_GSR)
class gsrCharCB : public BLECharacteristicCallbacks {
  void onRead(BLECharacteristic* pCharacteristic) {
    double value = sensorGsr.get_value();
    #ifdef GSR_LOGGING
    Serial.print("Gsr Value is:");
    Serial.println(value);
    #endif
    pCharacteristic->setValue(value);
    return;
  }
};
#endif

class ConnectionCallback : public BLEServerCallbacks {
  void onConnect ( BLEServer Server ) {
      umbrellaSysVars::isConnected = true;
      short counter = 3;
      while ( counter-- ) {
      delay(80);
      digitalWrite(LED_RED , HIGH);
      delay(100);
      digitalWrite(LED_RED , LOW);
      }
  }

  void onDisconnect ( BLEServer Server ) {
      umbrellaSysVars::isConnected = false;
      short counter = 3;
      while ( --counter ) {
        delay(80);
        digitalWrite(LED_RED , HIGH);
        delay(100);
        digitalWrite(LED_RED , LOW);
      }
  }
};

#endif  // UMBRELLA_H