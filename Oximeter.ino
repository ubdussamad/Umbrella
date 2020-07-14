#include <Wire.h>
#include "/home/ubdussamad/Arduino/libraries/MAX30100lib/src/MAX30100.h"
// #include "MAX30100_Registers.h"

#define LOGL Serial.println
#define LOG  Serial.print


MAX30100 pulseOximeter;

void setup()
{
    Serial.begin(115200);

    pulseOximeter.begin();
    pulseOximeter.setMode( MAX30100_MODE_SPO2_HR );
    pulseOximeter.setLedsPulseWidth( MAX30100_SPC_PW_1600US_16BITS );
    pulseOximeter.setSamplingRate( MAX30100_SAMPRATE_100HZ );
    pulseOximeter.setLedsCurrent( MAX30100_LED_CURR_50MA, MAX30100_LED_CURR_50MA );
    pulseOximeter.setHighresModeEnabled(true);
   
}

void loop() {
    pulseOximeter.update();
    uint16_t ir , red ;
    bool getRawValues( &ir , &red );
    LOGL(red);
}