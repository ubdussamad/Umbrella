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


/* Block for enabling different types of perpherals. */
#define COMPILING    0
#define ENABLE_GSR   1
#define ENABLE_OLED  1
#define ENABLE_HR    1
#define ENABLE_GYRO  0
#define LOG Serial.println

/* Timeout Macros Block */
#define DATA_NOTIFY_DELAY 60000


#include "umbrella.hpp"


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
  sensorGsr.set_adc_resolution(12);
  sensorGsr.set_sample_delay(5);
  sensorGsr.set_adc_voltage(3.3000);
  #endif



  BLEDevice::init("Umbrella");
  LOG("Initialized the device.");
  uSysVars::umbrellaServer = BLEDevice::createServer();
  uSysVars::umbrellaServer->setCallbacks(new ConnectionCallback);
  LOG("Initialized the Server.");


  #if (ENABLE_HR)
  uSysVars::hrInitFailed = !pulseOx.begin();
  if (uSysVars::hrInitFailed) {
        LOG("ERROR: Failed to initialize pulse oximeter");
  }
  else {
  pulseOx.setOnBeatDetectedCallback(onBeatDetectedCb); /* Callback if a beat is detected, notify the Client then. */
  BLEService *hrService = uSysVars::umbrellaServer->createService( HR_SERVICE_UUID );
  uSysVars::hrCharacteristic = hrService->createCharacteristic(HR_CHARACTERISTIC_UUID, BLE_NOTIFY);
  BLEDescriptor* d2902 = new BLE2902();
  uint8_t descHex[2] = {0b1111,0b1111}; // NU,NU,NU,NU,NU,NU, Indication bit, Notification bit.
  d2902->setValue(descHex,2);
  uSysVars::hrCharacteristic->addDescriptor(d2902);
  BLECharacteristic *hrCpCharacteristic = hrService->createCharacteristic(HR_CP_CHARACTERISTIC_UUID, BLE_NOTIFY);
  hrService->start();

  BLEService *poxService = uSysVars::umbrellaServer->createService( POX_SERVICE_UUID );
  uSysVars::poxCharacteristic = poxService->createCharacteristic(POX_CHARACTERISTIC_UUID, BLE_NOTIFY | BLE_READ);
  uSysVars::poxCharacteristic->addDescriptor(d2902);
  poxService->start();
  }

  #endif
  
  #if (ENABLE_GSR)
  BLEService *gsrService = uSysVars::umbrellaServer->createService( GSR_SENSOR_SERVICE_UUID );
  uSysVars::gsrCharacteristic = gsrService->createCharacteristic(GSR_SENSOR_CHARACTERISTIC_UUID, BLE_READ);
  uSysVars::gsrCharacteristic->addDescriptor(new BLE2902);
  uSysVars::gsrCharacteristic->setCallbacks( new gsrCharCB );
  gsrService->start();
  #endif

  


  BLEAdvertising* pAdvertising = BLEDevice::getAdvertising();
  #if (ENABLE_HR)
  if (!uSysVars::hrInitFailed) {pAdvertising->addServiceUUID(HR_SERVICE_UUID);}
  #endif
  #if (ENABLE_GSR)
  pAdvertising->addServiceUUID(GSR_SENSOR_SERVICE_UUID);
  #endif



  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();
  Serial.println("Device Initialization Complete.");
}


void loop () {
  #if (ENABLE_HR)
  pulseOx.update();
  #endif



  digitalWrite( LED_RED , 1);
  delay(50);
  digitalWrite(LED_RED , 0);
  delay(4000);

}
