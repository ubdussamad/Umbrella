/*
* File: Umbrella.ino
* Part of Project Umbrella
* Parent Suite for Umbrella Firmware.
* V-1.0 REF 20JUN2020
* This Piece of Software is NOT for public sharing.
* Authour: Ubdussamad <ubdussamad@gmail.com>
*/

#include "Gsr.h"
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <Arduino.h>


#define DEVICE_INFO_SERIVCE                    "0x180A" // SiG Standard
#define BATTERY_SERVICE_UUID                   "0x180F" // SiG Standard
#define HR_SERVICE_UUID                        "0x180D" // SiG Standard Heart Rate Service
#define HR_CHARACTERISTIC_UUID                 "0x2A37" // SiG Standard Heart Rate Measurement Serivce
#define BODY_TEMP_SERVICE_UUID                 "0x1809" // SiG Standard , Health Thermometer Service
#define BODY_TEMP_CHARACTERISTIC_UUID          "0x2A1C" // SiG Standard , Health Thermometer Charac..
#define BODY_TEMP_T_DESC_CHARACTERISTIC_UUID   "0x2A1F" // Temperature Celsius Descriptor
#define BODY_TEMP_T_DESIG_CHARACTERISTIC_UUID  "0x2A1D" // Temp type Descriptor
#define GSR_SENSOR_SERVICE_UUID                "4fafc201-1fb5-459e-8fcc-c5c9c331914b" // Custom UUID for GSR Sensor
#define GSR_SENSOR_CHARACTERISTIC_UUID         "248efce7-3ccc-48ee-ba7f-17a2e891266b" // Custom Characteristic UUID


#define ADC0_CH1 36
#define GSR_PWR 18
#define LED_BLUE 32
#define LED_RED 33
#define I2C_SDA 21
#define I2C_SCL 22
#define FLASH_BTN 0
#define ADC_VOLTAGE 3.5

gsr sensorGsr(ADC0_CH1 , GSR_PWR , ADC_VOLTAGE );




class GenericCharacteristicCallback : public BLECharacteristicCallbacks {
  
  void onRead(BLECharacteristic* pCharacteristic) {

    digitalWrite(LED_BLUE , HIGH);
    delay(50);
    digitalWrite(LED_BLUE , LOW);
    return;

  }
  
  void onWrite(BLECharacteristic* pCharacteristic) {
  
  return;
  
  }
};

class ConnectionCallback : public BLEServerCallbacks {
  void onConnect ( BLEServer *Server ) {

    short counter = 0x03;
    while ( counter ) {
      delay(50);
      digitalWrite(LED_BLUE , HIGH);
      delay(50);
      digitalWrite(LED_BLUE , LOW);
    }
  }

  void onDisconnect ( BLEServer *Server ) {

    short counter = 0x03;
    while ( counter ) {
      delay(50);
      digitalWrite(LED_BLUE , HIGH);
      delay(50);
      digitalWrite(LED_BLUE , LOW);
    }
  }
};




void setup () {

  Serial.begin(115200);

  Serial.println("Initializing.");

  pinMode( LED_BLUE, OUTPUT);
  pinMode( LED_RED , OUTPUT);
  pinMode( I2C_SDA , OUTPUT);
  pinMode( I2C_SCL , OUTPUT);
  pinMode( FLASH_BTN, INPUT);

  sensorGsr.set_averaging_samples(10);
  sensorGsr.set_adc_resolution(4096);
  sensorGsr.set_sample_delay(5);
  sensorGsr.set_adc_voltage(3.3000);

  BLEDevice::init("Umbrella");
  
  BLEServer *pServer = BLEDevice::createServer();
  
  BLEService *pService = pServer->createService(PROJECT_UMBRELLA_SERVICE_UUID);
  
  BLECharacteristic *pCharacteristic = pService->createCharacteristic(
                                         CHARACTERISTIC_UUID,
                                         BLECharacteristic::PROPERTY_READ |
                                         BLECharacteristic::PROPERTY_WRITE
                                       );

  pCharacteristic->setValue("Working this block.");

  pCharacteristic->setCallbacks( GenericCharacteristicCallback );
  
  pService->start();

  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);  // functions that help with iPhone connections issue
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();
  Serial.println("Characteristic defined! Now you can read it in your phone!");
}


void loop () {
  digitalWrite(LED_BLUE , 1);
  Serial.println(sensorGsr.get_value());
  digitalWrite( LED_BLUE , 0);
  delay(300);
  
}
