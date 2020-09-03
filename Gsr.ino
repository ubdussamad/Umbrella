/*
* File: Gsr.ino
* Part of Project Umbrella
* Suite for Handling Groove GSR Sensor.
* V-1.0 REF 20JUN2020
* This Piece of Software is NOT for pulic sharing.
* Author: Ubdussamad <ubdussamad@gmail.com>
*/
#if !defined(GSR_H)
#define GSR_H
#include "Gsr.hpp"
#endif // GSR_H

#define GSR_LOG_LEVEL 0
// 0: No logging
// 1: Basic Info logging.
// 2: All Info logging.
// 3: Verbose Logging



/**
 * @brief Get GSR value.
 * @return [float] Gsr value in millivolts.
 */
double gsr::get_value ( void ) {
    double rtn_value = 0;
    if (enable_pin_defined) {
        digitalWrite( gsr_module_power , HIGH );
        #if (GSR_LOG_LEVEL)
            Serial.println("Turning GSR on.");
        #endif
    }
    for ( short i = 0; i < sample_width ; i++ ) {
        delay(sample_delay);
        rtn_value += ( (v_adc*1000) / adc_resolution ) * analogRead( adc_channel );
    }
    if (enable_pin_defined) {
        digitalWrite( gsr_module_power , LOW );
        #if (GSR_LOG_LEVEL)
            Serial.println("Turning GSR off.");
        #endif
    }
    return (rtn_value/sample_width);
}

/**
 * @brief Set the time delay between consecutive samples in milliseconds.
 * @param [in] Delay Value in Milliseconds.
 */
void gsr::set_sample_delay ( const short& value ){
    sample_delay = value > 0 ? value : 10;
}

/**
 * @brief Contructor for Gsr class.
 * @param [in] Channel Pin Number
 * @param [in] ADC refrence voltage
 * @param [in] Enable Pin Number (if -1 then no enable pin available).
 */
gsr::gsr( const short int& channel, const double& adc_voltage, const short int& module_en_pin ) {
    pinMode( channel,  INPUT);

    if (module_en_pin != -1) {
        pinMode( module_en_pin , OUTPUT);
        digitalWrite( module_en_pin , LOW );
        enable_pin_defined = 1;
        gsr_module_power = module_en_pin;
    }
    else {
        enable_pin_defined = 0;
    }
    adc_channel = channel;
    v_adc = adc_voltage;
}

/* Set the resoulution of the ADC in use. 
   Parms: 2^Resolution ( 1024 for 10Bit ADC )
   Return: void
*/
void gsr::set_adc_resolution ( const int& value ) {
    adc_resolution = (double) (1<<value);
    #if (DEBUG)
    Serial.print("The adc resolution is set to: ");
    Serial.println(adc_resolution);
    #endif
}

/* Set the Voltage of the ADC in use. 
   Parms: Voltage (Volts > 0)
   Return: void
*/
void gsr::set_adc_voltage ( const double& value ) {
    v_adc = value <= 0 ? 1: value ;
}

/* Set the number of samples to take for each measure. 
   Parms: Number of samples.
   Return: void
*/
void gsr::set_averaging_samples( const short& samples ) {
    sample_width = samples < MAXIMUM_BUFFER_SIZE ? samples : MAXIMUM_BUFFER_SIZE;
}
