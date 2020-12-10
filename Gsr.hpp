/**
 * @file Gsr.h
 * @author ubdussamad (ubdussamad@gmail.com)
 * @brief Header File for GSR Library
 * Part of Project Umbrella
 * @version V-1.0 REF 20JUN2020
 * @date 2020-06-20
 * @license This piece of firmware is in public domian.
 * @copyright Copyright (c) 2020, ubdussamad@gmail.com
 */

// Prototype for GSR class

#if !defined(GSR_H)
#define GSR_H

#define DEFUALT_SAMPLE_WIDTH 20
#define DEFAULT_ADC_RESOLUTION 4096 // x = 2^Resolution
#define MAXIMUM_BUFFER_SIZE 1000 
#define DELAY_PER_SAMPLE 30 // ms


#include <Arduino.h>


class gsr
{
private:
    short int adc_channel;
    short int gsr_module_power;
    short int sample_width = DEFUALT_SAMPLE_WIDTH;
    double last_avg_data;
    double v_adc;
    bool enable_pin_defined;
    double adc_resolution = DEFAULT_ADC_RESOLUTION;
    int sample_delay = DELAY_PER_SAMPLE;

public:
    gsr( const short int& channel, const double& adc_voltage, const short int& module_en_pin = - 1  );
    void set_averaging_samples( const short& samples );
    double  get_value( void );
    void set_adc_resolution (const int& value );
    void set_sample_delay ( const short& value);
    void set_adc_voltage ( const double& value);
    // virtual void voltageOfsetCorrection (double deltaV);
    // virtual double getVoffCorrectedvalue(void);
};

#endif
