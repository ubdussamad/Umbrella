/*
* File: Gsr.h
* Part of Project Umbrella
* Header File for GSR Library
* V-1.0 REF 20JUN2020
* This Piece of Software is NOT for pulic sharing.
* Authour: Ubdussamad <ubdussamad@gmail.com>
*/

// Prototype for GSR class

#define DEFUALT_SAMPLE_WIDTH 20
#define DEFAULT_ADC_RESOLUTION 4096 // x = 2^Resolution
#define MAXIMUM_BUFFER_SIZE 1000 
#define DELAY_PER_SAMPLE 30 // ms   


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
    void set_adc_resolution (const double& value );
    void set_sample_delay ( const short& value);
    void set_adc_voltage ( const double& value);
};
