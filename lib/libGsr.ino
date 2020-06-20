/*
* Suite for Handling Groove GSR Sensor.
* V-1.0 REF 20JUN2020
* This File is a part of LifeSense project.
* This Piece of Software is NOT for pulic sharing.
* Authour: Ubdussamad <ubdussamad@gmail.com>
*/

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
    float last_avg_data;
    float v_adc;
    int adc_resolution = DEFAULT_ADC_RESOLUTION;
public:
    gsr( const short int& channel, const short int& module_en_pin, const float& adc_voltage  );
    void set_averaging_samples( const short& samples );
    float  get_value( void );
    void set_adc_resolution (const int& value );
};    


float gsr::get_value ( void ) {
    float rtn_value = 0;
    digitalWrite( gsr_module_power , HIGH );
    for ( short i = 0; i < sample_width ; i++ ) {
        delay(DELAY_PER_SAMPLE);
        rtn_value += ( v_adc / adc_resolution ) * analogRead( adc_channel );
    }
    return (rtn_value/sample_width);
}

gsr::gsr( const short int& channel, const short int& module_en_pin, const float& adc_voltage ) {
    pinMode( channel,  INPUT);
    pinMode( module_en_pin , OUTPUT);
    digitalWrite( module_en_pin , LOW );
    adc_channel = channel;
    gsr_module_power = module_en_pin;
    v_adc = adc_voltage;
}

void gsr::set_adc_resolution ( const int& value ) {
    adc_resolution = value;
}

void gsr::set_averaging_samples( const short& samples ) {
    sample_width = samples < MAXIMUM_BUFFER_SIZE ? samples : MAXIMUM_BUFFER_SIZE;
}
