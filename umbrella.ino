/**
 * @file umbrella.ino
 * @author ubdussamad (ubdussamad@gmail.com)
 * @brief Parent Suite for Umbrella Firmware.
 * Part of Project Umbrella
 * @version V-1.5 REF 08NOV2020
 * @date 2020-11-8
 * @license This Piece of Software is NOT for public sharing.
 * @copyright Copyright (c) 2020, ubdussamad@gmail.com
 */


/* Block for enabling different types of perpherals. */
#define COMPILING                         1
#define ENABLE_GSR                        1
#define ENABLE_OLED                       0
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

/**
 * @brief Setup Block Containing Initializers.
 * 
 */
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
  
  Wire.begin();
  uSysVars::hrInitFailed = bioHub.begin();

  if (uSysVars::hrInitFailed) {
    LOG("ERROR: Failed to initialize pulse oximeter");
  }
  else {

      int error = bioHub.configBpm(MODE_ONE); // Configuring just the BPM settings. 
      if (error) {
      }

    pulseOx.setOnBeatDetectedCallback(onBeatDetectedCb); /* Callback if a beat is detected, notify the Client then. */


    BLEService *hrService = uSysVars::umbrellaServer->createService( HR_SERVICE_UUID );
    uSysVars::hrCharacteristic = hrService->createCharacteristic(HR_CHARACTERISTIC_UUID, BLE_NOTIFY);
    BLEDescriptor* d2902 = new BLE2902();
    uint8_t descHex[2] = {0x01,0x00}; // NU,NU,NU,NU,NU,NU, Indication bit, Notification bit.
    d2902->setValue(descHex,2);
    uSysVars::hrCharacteristic->addDescriptor(d2902);
    BLECharacteristic *hrCpCharacteristic = hrService->createCharacteristic(HR_CP_CHARACTERISTIC_UUID, BLE_NOTIFY);
    hrService->start();
    BLEService *poxService = uSysVars::umbrellaServer->createService( POX_SERVICE_UUID );
    uSysVars::poxCharacteristic = poxService->createCharacteristic(POX_CHARACTERISTIC_UUID,BLE_NOTIFY|BLE_INDICATE);
    BLEDescriptor* d2902_pox = new BLE2902();
    d2902_pox->setValue(descHex,2);
    uSysVars::poxCharacteristic->addDescriptor(d2902_pox);
    BLECharacteristic *poxPlxFeatures = poxService->createCharacteristic(POX_PLX_FEATURES,BLE_NOTIFY|BLE_READ);
    uint8_t pox_plxD[] = {0x00,0x00};
    poxPlxFeatures->setValue(pox_plxD , 2);
    poxService->start();
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

}



void loop () {
  #if (ENABLE_HR)
  if (uSysVars::isConnected) {
    if (!uSysVars::pulseOxState) { 
      pulseOx.resume();
      uSysVars::pulseOxState = true;
    }
    pulseOx.update(); // This will eventually call the BeatDetected Callback
  }
  else {
    if (uSysVars::pulseOxState) {
      pulseOx.shutdown();
      LOG("Pulse Ox has been switched off as there is no connection.");
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
  #endif
  }
  

}
