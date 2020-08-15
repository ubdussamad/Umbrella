/*
* File: Umbrella.ino
* Part of Project Umbrella
* Parent Suite for Umbrella Firmware.
* V-1.0 REF 20JUN2020
* This Piece of Software is NOT for public sharing.
* Author: Ubdussamad <ubdussamad@gmail.com>
*/

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

#if !defined(GSR_H)
#define GSR_H
#include "Gsr.h"
#endif // GSR_H

#define PROJECT_UMBRELLA_SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"

#define CHARACTERISTIC_UUID               "2A37" // UUID for defining Heart Rate
#define BODY_TEMP_CHARACTERISTIC_DEFN_UUID   "2A1D" // UUID for defining type of temp.
#define BODY_TEMP_C_CHARACTERISTIC_UUID      "2A3C" // UUID for defining temp in deg C.
//


#define ADC0_CH1 36
#define GSR_PWR 18
#define LED_BLUE 32
#define LED_RED 34
#define I2C_SDA 21
#define I2C_SCL 22
#define FLASH_BTN 0
#define ADC_VOLTAGE 3.5

gsr sensorGsr(ADC0_CH1 ,ADC_VOLTAGE , GSR_PWR  );

class GenericCharacteristicCallback : public BLECharacteristicCallbacks {
 
 void onRead(BLECharacteristic* pCharacteristic) {
   uint16_t x = 78;
   pCharacteristic->setValue(x);

   digitalWrite(LED_RED , HIGH);
   delay(50);
   digitalWrite(LED_RED , LOW);
   Serial.println("Read Called!");
   return;

 }

};

class ConnectionCallback : public BLEServerCallbacks {
 void onConnect ( BLEServer *Server ) {

   short counter = 0x03;
   while ( counter-- ) {
     delay(50);
     digitalWrite(LED_BLUE , HIGH);
     delay(50);
     digitalWrite(LED_BLUE , LOW);
     Serial.println("Connected!");
   }
 }

 void onDisconnect ( BLEServer *Server ) {

   short counter = 0x03;
   while ( counter-- ) {
     delay(50);
     digitalWrite(LED_BLUE , HIGH);
     delay(50);
     digitalWrite(LED_BLUE , LOW);
   }
   Serial.println("Disconnected!");
 }
};


void setup () {

  Serial.begin(115200);

  Serial.println("Initializing.");

  pinMode( LED_BLUE, OUTPUT);
  pinMode( LED_RED , OUTPUT);
  // pinMode( I2C_SDA , OUTPUT);
  // pinMode( I2C_SCL , OUTPUT);
  pinMode( FLASH_BTN, INPUT);

 sensorGsr.set_averaging_samples(10);
 sensorGsr.set_adc_resolution(4096);
 sensorGsr.set_sample_delay(5);
 sensorGsr.set_adc_voltage(3.3000);

  BLEDevice::init("Umbrella");
  
  BLEServer *pServer = BLEDevice::createServer();

  pServer->setCallbacks( new ConnectionCallback() );
  
  BLEService *pService = pServer->createService(PROJECT_UMBRELLA_SERVICE_UUID);
  
  BLECharacteristic *pCharacteristic = pService->createCharacteristic(
                                         CHARACTERISTIC_UUID,
                                         BLECharacteristic::PROPERTY_READ |
                                         BLECharacteristic::PROPERTY_WRITE
                                       );
  uint16_t bpm = 80;
  pCharacteristic->setValue(bpm);
  auto xcb = new GenericCharacteristicCallback();
  pCharacteristic->setCallbacks( xcb );
  
  BLECharacteristic *pCharacteristic1 = pService->createCharacteristic(
                                         BODY_TEMP_C_CHARACTERISTIC_UUID,
                                         BLECharacteristic::PROPERTY_READ );
  uint16_t tmp = 30;
  pCharacteristic1->setValue(tmp);
  pCharacteristic1->setCallbacks( xcb );
  
  pService->start();

  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(PROJECT_UMBRELLA_SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);  // functions that help with iPhone connections issue
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();
  Serial.println("Characteristic defined! Now you can read it in your phone!");
}


void loop () {
  digitalWrite(LED_RED , 1);
  delay(100);
  digitalWrite( LED_RED , 0);
  delay(1000);
  
}
