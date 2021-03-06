/**
 * @file umbrella.hpp
 * @author ubdussamad (ubdussamad@gmail.com)
 * @brief Parent Suite Header for Umbrella Firmware.
 * Part of Project Umbrella
 * @version V-1.5 REF 08NOV2020
 * @date 2020-11-8
 * @license This piece of firmware is in public domian.
 * @copyright Copyright (c) 2020, ubdussamad@gmail.com
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
// Pulse Oximeter Driver Headers
#include <SparkFun_Bio_Sensor_Hub_Library.h>
#endif

#if (ENABLE_OLED_)
#include <Arduino.h>
#if (!COMPILING)
#define U8X8_USE_PINS
#endif
#include <U8g2lib.h>
#endif

/* Block for mapping BLE Characteristic UUIDs to relevent macros.  */
#define DEVICE_INFO_SERVICE                    "180A" // SiG Standard
#define BATTERY_SERVICE_UUID                   "180F" // SiG Standard

#define HR_SERVICE_UUID                        "180D" // SiG Standard Heart Rate Service
#define HR_CHARACTERISTIC_UUID                 "2A37" // SiG Standard Heart Rate Measurement Service
#define HR_CP_CHARACTERISTIC_UUID              "2A39" // SiG Standard Heart rate Control Point

#define POX_SERVICE_UUID                       "1822" // SiG Standard, PulseOximeter Service UUID
#define POX_CHARACTERISTIC_UUID                "2A5F" // SiG Standard, PulseOximeter Characteristic UUID
#define POX_PLX_FEATURES                       "2A60" // SiG standard, PluseOx PLC features

#define BODY_TEMP_SERVICE_UUID                 "1809" // SiG Standard , Health Thermometer Service
#define BODY_TEMP_CHARACTERISTIC_UUID          "2A1C" // SiG Standard , Health Thermometer Chars..
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

Now Since we could only send an 8bit(1Byte or an octet) unsigned int array,
we have to split a 16bit(2bytes)  into 2 Consecutive octets in an array.

For example, say we have 8fe2 we represent it in an array as {8f,e2}.
This way of representing the larger octet (8f) first and the smaller octet
last is called Big Endian Order (MSO to LSO)

But, In case of BLE standard we send it in the Little-Endian order (LSO to MSO).
So, we represent the hex number 8fe2 as {e2, 8f}.

Thus, the array we send for our heart rate characteristic data will look like:

FLAGS (8BIT)   , Heart Rate(8BIT), Energy Exp(16BIT), RR (16BIT)
 { C2          , 50              , 11,   00         , 08,    2d }
   8BIT        , 8BIT            , LSO,  MSO        , LSO,   MSO

   To many, this'd look un intuitive and unnatural, but some systems do
   work this way so do we have to work this way too :(.
   
*/

/* Block for defining BLE properties macros. */
#define BLE_NOTIFY    BLECharacteristic::PROPERTY_NOTIFY
#define BLE_READ      BLECharacteristic::PROPERTY_READ
#define BLE_WRITE     BLECharacteristic::PROPERTY_WRITE
#define BLE_INDICATE  BLECharacteristic::PROPERTY_INDICATE

/**
 * @brief Block Containing Error codes
 * for the error code bit field.
 */
#define POX_ERROR_BIT 1<<8
#define GSR_ERROR_BIT 1<<7
#define GYRO_ERR_BIT  1<<6

/* Block for mapping pinout to relevant macros.*/
#define ADC0_CH1        36
#define GSR_PWR         18
#define LED_RED         32
#define LED_BLUE        33 /* Not functional due to Hardware Reasons */
#define I2C_SDA         21
#define I2C_SCL         22
#define FLASH_BTN       0
#define ADC_VOLTAGE     3.3
#define RESET_PIN_POX   4 /* TODO: Should be Changed. */
#define MFIO_PIN_POX    5 /* TODO: Should be Changed. */

/* Block for OTHER macros */
#define uS_TO_S_FACTOR 1000000ULL  /* Conversion factor for micro seconds to seconds */
#define ULOGO_WIDTH  30
#define ULOGO_HEIGHT 30

/*Block contaning graphics. */
static unsigned char uLogo[] PROGMEM = {
   0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0x3f, 0x00, 0x80, 0x03, 0x30, 0x00,
   0x80, 0x07, 0x78, 0x00, 0xc0, 0x07, 0xf8, 0x00, 0xe0, 0x07, 0xfc, 0x01,
   0xf0, 0x0f, 0xfc, 0x03, 0xfc, 0x0f, 0xfc, 0x07, 0xfe, 0x1f, 0xfe, 0x1f,
   0xfe, 0x1f, 0xfe, 0x1f, 0xfa, 0x3f, 0xff, 0x17, 0xc2, 0x3f, 0xff, 0x10,
   0x02, 0x3f, 0x3f, 0x10, 0x02, 0xfc, 0x0f, 0x10, 0x02, 0xe0, 0x01, 0x10,
   0x02, 0xe0, 0x01, 0x10, 0x02, 0xf8, 0x07, 0x10, 0x02, 0x3f, 0x3f, 0x10,
   0xc2, 0x3f, 0xff, 0x11, 0xfa, 0x3f, 0xff, 0x17, 0xfe, 0x1f, 0xfe, 0x1f,
   0xfe, 0x1f, 0xfe, 0x1f, 0xf8, 0x0f, 0xfc, 0x07, 0xf0, 0x0f, 0xfc, 0x03,
   0xe0, 0x07, 0xfc, 0x01, 0xc0, 0x07, 0xf8, 0x00, 0x80, 0x07, 0x78, 0x00,
   0x80, 0x03, 0x30, 0x00, 0x00, 0xff, 0x3f, 0x00, 0x00, 0x00, 0x00, 0x00
   };


/* Constructor Block for diffrent peripherals.  */
#if (ENABLE_GSR)
auto sensorGsr =  gsr(ADC0_CH1 , ADC_VOLTAGE , GSR_PWR );
#endif
#if (ENABLE_OLED)
U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2 ( U8G2_R2 , U8X8_PIN_NONE, I2C_SCL , I2C_SDA );
#endif
#if (ENABLE_HR)
// TODO: Revise pin number of reset pin, and MFIO pin.
SparkFun_Bio_Sensor_Hub bioHub(RESET_PIN_POX, MFIO_PIN_POX);
#endif




/* Namespace Contaning System-Wide Vars  */
namespace uSysVars {

/** UPDATE:
 *  V-1.5 REF 08NOV2020
 *  System Wide Error Reporting Bit Field.
 *  If a designated bit is 1, it means thats there was an error.
 *  MSB -> LSB
 *  POX_ERROR_BIT, GSR_ERROR_BIT, GYRO_ERR_BIT, N/A, N/A, N/A, N/A, N/A
 */
uint8_t errorCode = 0b00000000;

/* System wide counter. */
#if (ENABLE_SLEEPING)
RTC_DATA_ATTR int bootCount(0); // Counts the number of bootups.
#endif
long int sysNotifyCounter(0); // TODO: This might Persist even when the uP is in sleep

#if (ENABLE_BLINKING)
uint64_t flashCounter(0);
#endif
#if (ENABLE_CONNECTION_TIMEOUT_SLEEP)
uint32_t connCtr(0);
#endif

#if (ENABLE_HR)
/**
 * UPDATE:
 * V-1.5 REF 08NOV2020
 * The beatsCounter will count the number of
 * reported heart rate and spo2 insted of 
 * counting the actually beats in the previously
 * used sensor.
 */
bioData body;

int beatsCounter(0);
int bpmMeasure(0); // This might be removed in the next update.
#endif


/* This block contains Global system event flags. */
bool isConnected(false);
bool hrInitFailed(false);
bool freshRestart(true);
bool pulseOxState(true);

/* This block decalers global constructor handles for BLE characteristics/Services and Servers. */
BLEServer               *umbrellaServer;
uConnectionCallback     *svCb;
BLECharacteristic       *hrCharacteristic;
BLECharacteristic       *tempCharacteristic;
BLECharacteristic       *gyroCharacteristic;

}

class uConnectionCallback : public BLEServerCallbacks {
  void onConnect(BLEServer* pServer) {
    LOG("Device Connected!");

    #if (ENABLE_OLED)
    delay(5);
    u8g2.clearBuffer();
    u8g2.drawStr(20,20,"Connected.");   // write something to the internal memory.
    u8g2.sendBuffer();
    delay(5);
    #endif
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
      #if (ENABLE_OLED)
      delay(5);
      u8g2.clearBuffer();
      u8g2.drawStr(10,20,"Disconnected.");   // write something to the internal memory.
      u8g2.sendBuffer();
      delay(5);
      #endif
      short counter = 3;
      while ( counter-- ) {
        delay(80);
        digitalWrite(LED_RED , HIGH);
        delay(100);
        digitalWrite(LED_RED , LOW);
      }
  }
};

#if (ENABLE_HR)
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
 * 
 * Update (V-1.5 REF 08NOV2020)
 * 
 * The New MAX30101 Sensor with bio sensor hub might come with a similar
 * callback feature thus this isn't removed yet, although the later
 * stated issues are stil not fixed and should be worked on.
 * 
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
    uSysVars::bpmMeasure += pulseOx.getHeartRate();
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
  uSysVars::bpmMeasure /= BEATS_TO_COUNT_BEFORE_NOTIFYING - 1;
  uint8_t hrD[] = { 0b01100010, uSysVars::bpmMeasure };
  uint8_t spo2 = pulseOx.getSpO2();
  uint8_t poxD[] = { 0b00000000, spo2 , uSysVars::bpmMeasure };
  uSysVars::hrCharacteristic->setValue(hrD , 2);
  uSysVars::hrCharacteristic->notify();
  delay(5);
  uSysVars::poxCharacteristic->setValue(poxD,3);
  uSysVars::poxCharacteristic->notify();
  delay(5);
  double gsrD = sensorGsr.get_value();
  uSysVars::gsrCharacteristic->setValue(gsrD);
  uSysVars::gsrCharacteristic->notify();
  delay(5);
  pulseOx.shutdown(); // Shutting down the Pulse Oximeter after Reading data.

  LOG("Logging..");
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