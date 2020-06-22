/*
* File: Umbrella.ino
* Part of Project Umbrella
* Parent Suite for Umbrella Firmware.
* V-1.0 REF 20JUN2020
* This Piece of Software is NOT for public sharing.
* Authour: Ubdussamad <ubdussamad@gmail.com>
*/

#include "Gsr.h"

#define ADC0_CH1 36
#define GSR_PWR 18
#define LED_BLUE 32
#define LED_RED 33
#define I2C_SDA 21
#define I2C_SCL 22
#define FLASH_BTN 0
#define ADC_VOLTAGE 3.5

gsr sensorGsr(ADC0_CH1 , GSR_PWR , ADC_VOLTAGE );


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
}


void loop () {
  digitalWrite(LED_BLUE , 1);
  Serial.println(sensorGsr.get_value());
  digitalWrite( LED_BLUE , 0);
  delay(300);
  
}
