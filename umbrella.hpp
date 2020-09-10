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

#if (ENABLE_HR || ENABLE_OLED || ENABLE_GYRO)
#include <Wire.h>
#endif

#if (ENABLE_HR)
#pragma once
#include "MAX30100_PulseOximeter.h"
#endif

#if (ENABLE_OLED)
#if (!COMPILING)
#define U8X8_USE_PINS
#endif
#include <U8g2lib.h>
#endif

/* Block for mapping BLE Characteristic UUIDs to relevent macros.  */
#define DEVICE_INFO_SERIVCE                    "180A" // SiG Standard
#define BATTERY_SERVICE_UUID                   "180F" // SiG Standard

#define HR_SERVICE_UUID                        "180D" // SiG Standard Heart Rate Service
#define HR_CHARACTERISTIC_UUID                 "2A37" // SiG Standard Heart Rate Measurement Serivce
#define HR_CP_CHARACTERISTIC_UUID              "2A39" // SiG Standard Heart rate Control Point

#define POX_SERVICE_UUID                       "1822" // SiG Standard, PulseOximeter Service UUID
#define POX_CHARACTERISTIC_UUID                "2A5F" // SiG Standard, PulseOximeter Characteristic UUID
#define POX_PLX_FEATURES                       "2A60" // SiG standard, PluseOx PLC fetures

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

For Example, a value having a width greater than 1byte (8bit)
will be represented as follows:

Lets say we have a Heart rate Charateristic value in the order:
FLAGS (8BIT), Heart Rate(8BIT), Energy Expended(16BIT) , RR (16BIT)

Lets Assume some values according to their sizes:

 FLAGS (8BIT), Heart Rate(8BIT), Energy Exp(16BIT), RR (16BIT)
 194         , 80 (BPM)        , 17 Kilo Joules   , 11528 milliseconds   | Decimal Values
 11000010    , 01010000        , 0000000000010001 , 0010110100001000     | Binary Values
 C2          , 50              , 0011             , 2d08                 | Hex Values

Now Since we could only send an 8bit(1Byte or an octet) unisgned int array,
we have to split a 16bit(2bytes)  into 2 Consecutive octets in an array.

For example, say we have 8fe2 we represent it in an array as {8f,e2}.
This way of represnting the larger octet (8f) first and the smaller octet
last is called Big Endian Order (MSO to LSO)

But, In case of BLE standard we send it in the Little-Endian order (LSO to MSO).
So, we represent the hex number 8fe2 as {e2, 8f}.

Thus, the array we send for our heart rate characteristic data will look like:

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

/* Block for OTHER macros */
#define uS_TO_S_FACTOR 1000000ULL  /* Conversion factor for micro seconds to seconds */ 


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




/* Namespace Contaning System-Wide Vars  */
namespace uSysVars {

/* System wide counter. */
#if (ENABLE_SLEEPING)
RTC_DATA_ATTR int bootCount(0); // Counts the number of bootups.
#endif
long int sysNotifyCounter(0); // TODO: This might Persist even when the uP is in sleep
short beatsCounter(0);
#if (ENABLE_BLINKING)
uint64_t flashCounter(0);
#endif


/* This block contains Global system event flags. */
bool isConnected(false);
bool hrInitFailed(false);
bool freshRestart(true);
bool pulseOxState(true);

/* This block decalers global constructor handles for BLE characteristics/Services and Servers. */
BLEServer           *umbrellaServer;
// uConnectionCallback *svCb;
BLECharacteristic   *hrCharacteristic;
BLECharacteristic   *poxCharacteristic;
BLECharacteristic   *gsrCharacteristic;
BLECharacteristic   *tempCharacteristic;
BLECharacteristic   *gyroCharacteristic;

}

class uConnectionCallback : public BLEServerCallbacks {
  void onConnect(BLEServer* pServer) {
    LOG("Device Connected!");
    uSysVars::isConnected = true;
    short counter = 3;
    while ( counter-- ) {
    delay(80);
    digitalWrite(LED_RED , HIGH);
    delay(100);
    digitalWrite(LED_RED , LOW);
    }
      
  }

  void onDisconnect ( BLEServer Server ) {
      uSysVars::isConnected = false;
      LOG("Device Disconnected.!");
      short counter = 3;
      while ( counter-- ) {
        delay(80);
        digitalWrite(LED_RED , HIGH);
        delay(100);
        digitalWrite(LED_RED , LOW);
      }
  }
};

#if (1)//ENABLE_HR)
/**
 * @brief This is a method which will be called when the
 * PulseOximeter detects a heart beat, this routine
 * will call all the other sensors to collect data and notify.
 * Finally, the whole chip will be shutdown and woken up after
 * a set interval from start.
 * 
 * There is an ISSUE that if the heart rate sensor is disabled
 * the whole system won't notify since everything depends on
 * the beat detected callback.
 * 
 * This issue should be fixed.
 */
void onBeatDetectedCb() { 
  /** TODO: This should trigger notify funcs for all services,
   * as it's the last thing to gather data.
   * 
   * If applicable, it should also turn everything (Sensors)
   * off before going into powerdow mode.
   * 
   * It should also put the whole processor to sleep after finally
   * sending all the data.
   * 
   * Also, trigger all the notify after a couple of beat detections maybe,
   * thus it'll have a callback counter which counts how many times the
   * beat has been detected. DONE!!
   */

  LOG("Beat Detected! CB Triggred.\n");

  if (uSysVars::beatsCounter != BEATS_TO_COUNT_BEFORE_NOTIFYING) {
    uSysVars::beatsCounter++;
    LOG("Beat Halted, count: "+String(uSysVars::beatsCounter)+" times.\n");
    return;
  }
  else if (!uSysVars::isConnected) {
    /** If we have enough reading but nothing is connected
     * to the device then dont turn the device off but wait
     *  for something to connect.
     * */
    LOG("Beat Count is enough but device not connected!");
    uSysVars::beatsCounter = BEATS_TO_COUNT_BEFORE_NOTIFYING - 1;
    return;
  }
  else {
    uSysVars::beatsCounter = 0;
    #if (ENABLE_SLEEPING)
    ++uSysVars::bootCount; // Increment the boot count as we're going to sleep.
    #endif
  }


  LOG("Beat Detected 4 times. Rate is: ");
  float hr = pulseOx.getHeartRate();
  uint8_t hrD[] = { 0b01100010, hr };
  LOG(hrD[1]);
  uint8_t spo2 = pulseOx.getSpO2();
  uint8_t poxD[] = { 0b00000000, spo2 , hr };

  uSysVars::hrCharacteristic->setValue(hrD , 2);
  LOG("CAlling HR notify!");
  uSysVars::hrCharacteristic->notify();
  delay(80);

  uSysVars::poxCharacteristic->setValue(poxD,3);
  LOG("CAlling POX notify!");
  uSysVars::poxCharacteristic->notify();
  delay(80);

  double gsrD = sensorGsr.get_value();
  /** After fetching value, the GSR sensor is automatically
   *  shutdown by its handler class.
   */
  uSysVars::gsrCharacteristic->setValue(gsrD);
  LOG("CAlling GSR notify!");
  uSysVars::gsrCharacteristic->notify();
  delay(80);

  pulseOx.shutdown(); // Shutting down the Pulse Oximeter after Reading data.

  LOG("Waiting for things to cool down!");
  delay(3000); // This delay is to let things cool down before shutting everything down.


  #if (ENABLE_SLEEPING)
  LOG("\n\n\n--------\n\nGoing to sleep!---------\n\n\n");
  Serial.flush();
  esp_deep_sleep_start();
  #endif
}
#endif

#if (ENABLE_SLEEPING_TOOLS)
void print_wakeup_reason(){
  esp_sleep_wakeup_cause_t wakeup_reason;

  wakeup_reason = esp_sleep_get_wakeup_cause();

  switch(wakeup_reason)
  {
    case ESP_SLEEP_WAKEUP_EXT0 : LOG("Wakeup caused by external signal using RTC_IO"); break;
    case ESP_SLEEP_WAKEUP_EXT1 : LOG("Wakeup caused by external signal using RTC_CNTL"); break;
    case ESP_SLEEP_WAKEUP_TIMER : LOG("Wakeup caused by timer"); break;
    case ESP_SLEEP_WAKEUP_TOUCHPAD : LOG("Wakeup caused by touchpad"); break;
    case ESP_SLEEP_WAKEUP_ULP : LOG("Wakeup caused by ULP program"); break;
    default : Serial.printf("Wakeup was not caused by deep sleep: %d\n",wakeup_reason); break;
  }
}
#endif





#if (ENABLE_GSR)
class gsrCharCB : public BLECharacteristicCallbacks {
  void onRead(BLECharacteristic* pCharacteristic) {
    double value = sensorGsr.get_value();
    #ifdef GSR_LOGGING
    LOG("Gsr Value is:");
    LOG(value);
    #endif
    pCharacteristic->setValue(value);
    return;
  }
};
#endif



#endif  // UMBRELLA_H