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
#define COMPILING                         1
#define ENABLE_GSR                        1
#define ENABLE_OLED                       0
#define ENABLE_OLED_                      1
#define ENABLE_HR                         1
#define ENABLE_GYRO                       0
#define ENABLE_SLEEPING                   1
#define ENABLE_SLEEPING_TOOLS             0
#define ENABLE_BLINKING                   1
#define ENABLE_CONNECTION_TIMEOUT_SLEEP   0
#define LOG Serial.println
/* Timeout Macros Block */
#define DATA_NOTIFY_DELAY 60000
#define BEATS_TO_COUNT_BEFORE_NOTIFYING 15
#define TIME_TO_SLEEP  20        /* Time ESP32 will go to sleep (in seconds) */
#define FLASH_COUNTER_TRIGGER 5455 // 1364counts = ~1second
#define CONN_TIMEOUT 999999L


#include "umbrella.hpp"


void setup () {

  Serial.begin(115200);

  Serial.println("Initializing.");

  #if (ENABLE_OLED_)
  u8g2.begin();
  u8g2.clearBuffer();                // Clear the internal memory.
  u8g2.drawXBMP( 38 , 0, ULOGO_WIDTH, ULOGO_HEIGHT, uLogo );
  // 8g2.drawStr(20,20,"UMBRELLA");  // write something to the internal memory.
  u8g2.sendBuffer();
  #endif

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

  #if (ENABLE_SLEEPING)
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  if (uSysVars::bootCount > 0) {
    uSysVars::freshRestart = false;
    Serial.println("\n\n-----\n\nWaking up count: "+String(uSysVars::bootCount)+"\n----\n\n\n");
  }
  #endif


  BLEDevice::init("Umbrella");
  LOG("Initialized the device.");
  uSysVars::umbrellaServer = BLEDevice::createServer();
  auto svCb = new uConnectionCallback();
  uSysVars::umbrellaServer->setCallbacks(svCb);
  LOG("Initialized the Server.");


  #if (ENABLE_HR) // This block contains the BLE setup for P-Ox as well.
  uSysVars::hrInitFailed = !pulseOx.begin();
  // auto RED = LEDCurrent::MAX30100_LED_CURR_50MA;
  // auto IR = LEDCurrent::MAX30100_LED_CURR_50MA;
  // pulseOx.setLedsCurrent(IR , RED);

  if (uSysVars::hrInitFailed) {
        LOG("ERROR: Failed to initialize pulse oximeter");
  }
  else {
  pulseOx.setOnBeatDetectedCallback(onBeatDetectedCb); /* Callback if a beat is detected, notify the Client then. */
  BLEService *hrService = uSysVars::umbrellaServer->createService( HR_SERVICE_UUID );
  uSysVars::hrCharacteristic = hrService->createCharacteristic(HR_CHARACTERISTIC_UUID, BLE_NOTIFY);
  BLEDescriptor* d2902 = new BLE2902();
  uint8_t descHex[2] = {0x01,0x00}; // NU,NU,NU,NU,NU,NU, Indication bit, Notification bit.
  d2902->setValue(descHex,2);
  uSysVars::hrCharacteristic->addDescriptor(d2902);
  BLECharacteristic *hrCpCharacteristic = hrService->createCharacteristic(HR_CP_CHARACTERISTIC_UUID, BLE_NOTIFY);
  hrService->start();

  delay(30);

  BLEService *poxService = uSysVars::umbrellaServer->createService( POX_SERVICE_UUID );
  uSysVars::poxCharacteristic = poxService->createCharacteristic(POX_CHARACTERISTIC_UUID,BLE_NOTIFY|BLE_INDICATE);
  BLEDescriptor* d2902_pox = new BLE2902();
  d2902_pox->setValue(descHex,2);
  uSysVars::poxCharacteristic->addDescriptor(d2902_pox);

  BLECharacteristic *poxPlxFeatures = poxService->createCharacteristic(POX_PLX_FEATURES,BLE_NOTIFY|BLE_READ);
  uint8_t pox_plxD[] = {0x00,0x00};
  poxPlxFeatures->setValue(pox_plxD , 2);  
  poxService->start();

  delay(30);
  u8g2.clearBuffer();
  }

  #endif
  
  #if (ENABLE_GSR)
  BLEService *gsrService = uSysVars::umbrellaServer->createService( GSR_SENSOR_SERVICE_UUID );
  uSysVars::gsrCharacteristic = gsrService->createCharacteristic(GSR_SENSOR_CHARACTERISTIC_UUID, 1U | 4U);
  BLEDescriptor* d2902 = new BLE2902();
  uint8_t descHex[2] = {0x01,0x00}; // NU,NU,NU,NU,NU,NU, Indication bit, Notification bit.
  d2902->setValue(descHex,2);
  uSysVars::gsrCharacteristic->addDescriptor(d2902);
  uSysVars::gsrCharacteristic->setCallbacks( new gsrCharCB );
  gsrService->start();
  delay(30);
  #endif


  BLEAdvertising* pAdvertising = BLEDevice::getAdvertising();
  #if (ENABLE_HR)
  if (!uSysVars::hrInitFailed) {
    pAdvertising->addServiceUUID(HR_SERVICE_UUID);
    pAdvertising->addServiceUUID(POX_SERVICE_UUID);
  }
  #endif
  #if (ENABLE_GSR)
  pAdvertising->addServiceUUID(GSR_SENSOR_SERVICE_UUID);
  #endif

  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();
  Serial.println("Device Initialization Complete.");

  #if (ENABLE_OLED_)
  delay(1000);
  u8g2.setFont(u8g2_font_10x20_tf); //u8g2_font_12x10_tf); // Choose a suitable font.
  u8g2.clearBuffer();
  u8g2.drawStr(10,20,"BLE ACTIVATED.");   // write something to the internal memory.
  u8g2.sendBuffer();
  delay(1000);
  #endif
}


void loop () {
  #if (ENABLE_HR)
  if (uSysVars::isConnected) {
    if (!uSysVars::pulseOxState) { 
      pulseOx.resume();
      uSysVars::pulseOxState = true;
      LOG("Pulse Ox Now turned on!");
      #if (ENABLE_OLED)
      delay(300);
      u8g2.clearBuffer();
      u8g2.drawStr(20,20,"POX ON.");   // write something to the internal memory.
      u8g2.sendBuffer();
      #endif

    }
    pulseOx.update(); // This will eventually call the BeatDetected Callback
  }
  else {
    if (uSysVars::pulseOxState) {
      pulseOx.shutdown();
      LOG("Pulse Ox has been switched off as there is no connection.");
      #if (ENABLE_OLED)
      delay(100);
      u8g2.clearBuffer();
      u8g2.drawStr(20,20,"POX OFF.");   // write something to the internal memory.
      u8g2.sendBuffer();
      #endif
      uSysVars::pulseOxState = false;
    }
      #if (ENABLE_BLINKING)
      delay(1);
      #endif

      #if(ENABLE_CONNECTION_TIMEOUT_SLEEP)
      /**
       * TODO: Maybe impliment dynamic sleep Time.
       * Like if the device dosen't gets connected
       * 5 times in a row, just increase the sleep time.
       * 
       * The number of cycles would be monitored by counter
       * stored in RTC memory so it won't perish after 
       * deelpsleep.
       * 
       * Although this feature works, other
       * improvements can be made to it.
       */
      if (uSysVars::connCtr == CONN_TIMEOUT) {
      LOG("Going to sleep since there is no connection.");
      #if (ENABLE_OLED)
      u8g2.clearBuffer();
      u8g2.drawStr(20,20,"Sleeping..");   // write something to the internal memory.
      u8g2.sendBuffer();
      #endif
      Serial.flush();
      uSysVars::connCtr = 0;
      esp_deep_sleep_start();
      }
      uSysVars::connCtr++;
      #endif

  }
  #endif


  #if (ENABLE_BLINKING)
  uSysVars::flashCounter++;
  if (uSysVars::flashCounter == FLASH_COUNTER_TRIGGER) {
  digitalWrite( LED_RED , 1);
  delay(50);
  digitalWrite(LED_RED , 0);
  uSysVars::flashCounter = 0;
  #if (ENABLE_OLED)
  /**
   * This clears the screen buffer to save power
   * every 4 secs or so.
   * Thus the screen can at best be on for a maximum
   * of 4 seconds.
   */
  u8g2.clearBuffer();
  u8g2.sendBuffer();
  #endif
  }
  #endif
  

}
