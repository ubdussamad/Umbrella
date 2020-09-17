#include <Wire.h>
#include "MAX30100_PulseOximeter.h"
#include "MAX30100_Registers.h"
#define REPORTING_PERIOD_MS     1000
#define LOGL Serial.println
#define LOG  Serial.print

// PulseOximeter is the higher level interface to the sensor
// it offers:
//  * beat detection reporting
//  * heart rate calculation
//  * SpO2 (oxidation level) calculation





class Oximeter
{
private:
    /* data */
    PulseOximeter pox;
    uint32_t tsLastReport = 0;
    bool oximeterOn = false;
public:
    Oximeter( LEDCurrent irC = DEFAULT_IR_LED_CURRENT );
    ~Oximeter();
    bool anomalyDetected ( void );
    uint8_t getHR ( short wait_time );
    uint8_t getOx ( short wait_time );
};

int Oximeter::getHR ( short wait_time ) {
    int res = 0;
    pox.resume();
    while (wait_time--) {
        delay(100);
        pox.update();
        res += (int) pox.getHeartRate();
    }
    pox.shutdown();
    return res / wait_time;
}

uint8_t Oximeter::getOx ( short wait_time ) {
    int res = 0;
    pox.resume();
    while (wait_time--) {
        delay(100);
        pox.update();
        res += (int) pox.getSpO2();
    }
    pox.shutdown();
    return res / wait_time;
}

Oximeter::Oximeter( LEDCurrent irC = DEFAULT_IR_LED_CURRENT ) {
    pox.setIRLedCurrent(irC);
    oximeterOn = (bool) pox.begin();
    if (oximeterOn) { pox.update(); }
    pox.shutdown()

}

Oximeter::~Oximeter() {
    pox.shutdown();
    return;
}