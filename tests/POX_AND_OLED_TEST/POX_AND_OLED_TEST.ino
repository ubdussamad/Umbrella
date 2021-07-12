/**
 * @file POX_AND_OLED_TEST.ino
 * @author Mohammad Samad (ubdussamad@gmail.com)
 * @brief Firmware for testing the Pulse-Oximeter and LCD.
 * @version 0.1
 * @date 2021-01-06
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#include <Arduino.h>

#define U8X8_USE_PINS
#include <U8g2lib.h>

#include <Wire.h>
#include <SparkFun_Bio_Sensor_Hub_Library.h>

#define I2C_SDA         21
#define I2C_SCL         22
#define POX_RST         13
#define POX_MFIO        15

#define AVG_COUNT       20

bioData   body;
uint16_t  pox_sum(0);
uint16_t  ecg_sum(0);
uint8_t   sample_counter(0);



U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2 ( U8G2_R2 , U8X8_PIN_NONE, I2C_SCL , I2C_SDA );
SparkFun_Bio_Sensor_Hub bioHub(POX_RST, POX_MFIO);



void setup(void) {
  Serial.begin(115200);

  Serial.println("Ishtarting....");
  
  u8g2.begin();
  u8g2.enableUTF8Print();		// enable UTF8 support for the Arduino print() function
  u8g2.setFont(u8g2_font_pxplusibmcgathin_8u);
  int result = bioHub.begin();
  u8g2.clearBuffer();
  u8g2.drawStr(30,15, "UMBRELLA" );  // write something to the internal memory
  u8g2.sendBuffer();
  
  if (result == 0) //Zero errors!
    Serial.println("Sensor started!");
  else
    Serial.println("Could not communicate with the sensor!!!");
 
  Serial.println("Configuring Sensor...."); 
  int error = bioHub.configBpm(MODE_TWO); // Configuring just the BPM settings. 


  if(error == 0){ // Zero errors
    Serial.println("Sensor configured.");
    u8g2.clearBuffer();
    u8g2.drawStr(0,10, "POX is ONLINE [O]" );  // write something to the internal memory
    u8g2.sendBuffer();
    delay(2000);
    u8g2.clearBuffer();
    u8g2.drawStr(0,10, "GSR&GYRO:0.0/OK" );  // write something to the internal memory
    // u8g2.drawStr(0,60, "GSR: 2.25mV [X]" );
    u8g2.sendBuffer();
  }
  delay(4000);
}

void loop(void) {
  

  body = bioHub.readBpm();
  if ( body.confidence > 80) {
    u8g2.drawStr(125,15, "*" );  // write something to the internal memory
    u8g2.sendBuffer();
    delay(200);
    u8g2.drawStr(125,15, " " );  // write something to the internal memory
    u8g2.sendBuffer();
    sample_counter++;
    ecg_sum+= body.heartRate;
    pox_sum+=body.oxygen;
  }

  if (sample_counter == AVG_COUNT) {
    u8g2.clearBuffer();
    char cstr[8];
    u8g2.drawStr(15,15,  "HR:" ); 
    u8g2.drawStr(40,15,  itoa( (int) ecg_sum/AVG_COUNT, cstr, 10) );  // write something to the internal memory
    u8g2.drawStr(70,15,  "OX:" ); 
    u8g2.drawStr(95,15,  itoa( (int) pox_sum/AVG_COUNT, cstr, 10) );  // write something to the internal memory
    u8g2.sendBuffer();
    sample_counter = 0;
    pox_sum = 0;
    ecg_sum = 0;

  }
  

  delay(300);
}
