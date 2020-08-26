/*
    File: Umbrella.ino
    Parent Suite for Umbrella Firmware.
    Part of Project Umbrella
    V-1.0 REF 20JUN2020
    Author: Ubdussamad <ubdussamad@gmail.com>
    This Piece of Software is NOT for public sharing.
    Copyright (c) 2020, ubdussamad@gmail.com
    All rights reserved.
*/



#define COMPILING 0
#define ENABLE_GSR 0
#define ENABLE_LCD 0
#define LOG Serial.println

#if (!COMPILING)
#include <Arduino.h>
#endif


#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>

// #if !defined(GSR_H)
// #define GSR_H
// #include "Gsr.h"
// #endif // GSR_H


// This block is optional and could be commented out during compilation.
// #define U8X8_USE_PINS "HI"
// #define ARDUHAL_LOG_LEVEL_VERBOSE 20

#if (ENABLE_GSR)
#if !defined(GSR_H)
#define GSR_H
#include "Gsr.h"
#endif // GSR_H
#endif

#if (ENABLE_LCD)
#include <Wire.h>
#include <U8g2lib.h>
#endif

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


#define ADC0_CH1      36
#define GSR_PWR       18
#define LED_RED       32
#define LED_BLUE      33 /* Not functional due to Hardware Reasons */
#define I2C_SDA       21
#define I2C_SCL       22
#define FLASH_BTN     0
#define ADC_VOLTAGE   3.5

#if (ENABLE_GSR)
gsr sensorGsr(ADC0_CH1 , GSR_PWR , ADC_VOLTAGE );
#endif

#if (ENABLE_LCD)
U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2 ( U8G2_R2 , U8X8_PIN_NONE, I2C_SCL , I2C_SDA );
#endif

class GenericCharacteristicCallback : public BLECharacteristicCallbacks {
  
  void onRead(BLECharacteristic pCharacteristic) {

    digitalWrite(LED_RED , HIGH);
    delay(50);
    digitalWrite(LED_RED , LOW);
    return;

  }
  
  void onWrite(BLECharacteristic pCharacteristic) {
  
  return;
  
  }
};

class ConnectionCallback : public BLEServerCallbacks {
  void onConnect ( BLEServer Server ) {

    short counter = 0x03;
    while ( counter ) {
      delay(50);
      digitalWrite(LED_RED , HIGH);
      delay(50);
      digitalWrite(LED_RED , LOW);
    }
  }

  void onDisconnect ( BLEServer Server ) {

    short counter = 0x03;
    while ( counter ) {
      delay(50);
      digitalWrite(LED_RED , HIGH);
      delay(50);
      digitalWrite(LED_RED , LOW);
    }
  }
};


BLECharacteristic *hrCharacteristic;

void setup () {

  Serial.begin(115200);

  Serial.println("Initializing.");

  pinMode( LED_BLUE, OUTPUT);
  pinMode( LED_RED , OUTPUT);
  pinMode( I2C_SDA , OUTPUT);
  pinMode( I2C_SCL , OUTPUT);
  pinMode( FLASH_BTN, INPUT);

  #if (ENABLE_GSR)
  sensorGsr.set_averaging_samples(10);
  sensorGsr.set_adc_resolution(4096);
  sensorGsr.set_sample_delay(5);
  sensorGsr.set_adc_voltage(3.3000);
  #endif



  BLEDevice::init("Umbrella");

  LOG("Initialized the device.");
  
  BLEServer *pServer = BLEDevice::createServer();

  LOG("Initialized the Server.");


  BLEService *hrService = pServer->createService( HR_SERVICE_UUID );

  LOG("Created Service.");

  hrCharacteristic = hrService->createCharacteristic(
                                         HR_CHARACTERISTIC_UUID,
                                         BLECharacteristic::PROPERTY_NOTIFY);
  hrCharacteristic->addDescriptor(new BLE2902());
  // BLEDescriptor hrdesc(BLEUUID((uint16_t)0x290C));

  uint8_t hrFlag[2] = {80, 0b11111111};
  LOG("\n\n------------------------XXXXXXXXXXXXXXX---------------\n\n");
  hrCharacteristic->setValue( hrFlag , 2 ); // BPM , DUMMY DATA , Please Comment out
  LOG("\n\n");

  // hrCharacteristic->

  BLECharacteristic *hrCpCharacteristic = hrService->createCharacteristic(
                                         HR_CP_CHARACTERISTIC_UUID, BLECharacteristic::PROPERTY_NOTIFY );
  uint16_t hr = 80;
  hrCharacteristic->setValue( hr ); // BPM , DUMMY DATA , Please Comment out


  
  // hrCharacteristic->setCallbacks( new GenericCharacteristicCallback );
  hrService->start();

  // BLEService* bodyTempService = pServer->createService(BODY_TEMP_SERVICE_UUID);
  // BLECharacteristic* bodyTempCharacteristic = bodyTempService->createCharacteristic( BODY_TEMP_T_DESC_CHARACTERISTIC_UUID, BLECharacteristic::PROPERTY_READ);
  // uint16_t bt = 80;
  // bodyTempCharacteristic->setValue( bt ); // BPM , DUMMY DATA , Please Comment out
  // bodyTempCharacteristic->setCallbacks( new GenericCharacteristicCallback );
  // bodyTempService->start();


  
  // Generic Layout for BLE Service
  // BLEService* pService = pServer->createService(HR_SERVICE_UUID);
  // BLECharacteristic* pCharacteristic = pService->createCharacteristic( HR_CHARACTERISTIC_UUID, BLECharacteristic::PROPERTY_READ);
  // uint16_t hr = 80;
  // pCharacteristic->setValue( hr ); // BPM , DUMMY DATA , Please Comment out
  // pCharacteristic->setCallbacks( new GenericCharacteristicCallback );
  // hrService->start();

  BLEAdvertising* pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(HR_SERVICE_UUID);


  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);  // functions that help with iPhone connections issue
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();
  Serial.println("Characteristic defined! Now you can read it in your phone!");
}

uint8_t hrg[2] = {80, 0b1110000};
void loop () {
  
  hrCharacteristic->setValue(hrg , 2);
  hrg[0]++;
  hrg[1]++;
  hrCharacteristic->notify(1);
  delay(1800);

  #if (ENABLE_GSR)
  digitalWrite(LED_RED , 1);
  Serial.println(sensorGsr.get_value());
  digitalWrite( LED_RED , 0);
  delay(300);
  #endif
  
}
