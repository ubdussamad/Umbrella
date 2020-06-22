/*
* File: Gsr.ino
* Part of Project Umbrella
* Suite for Handling Groove GSR Sensor.
* V-1.0 REF 20JUN2020
* This Piece of Software is NOT for pulic sharing.
* Author: Ubdussamad <ubdussamad@gmail.com>
*/

/* Get GSR value.
   Parms: void
   Return: [float] Gsr value in millivolts.
*/
double gsr::get_value ( void ) {
    double rtn_value = 0;
    digitalWrite( gsr_module_power , HIGH );
    for ( short i = 0; i < sample_width ; i++ ) {
        delay(sample_delay);
        rtn_value += ( v_adc / adc_resolution ) * analogRead( adc_channel );
    }
    digitalWrite( gsr_module_power , LOW );
    return (rtn_value/sample_width);
}

/* Set the time delay between consecutive samples in milliseconds.
    Parms: Delay Value in Milliseconds 
    Return: void
*/
void gsr::set_sample_delay ( const short& value ){
    sample_delay = value;
}

/* Contructor for Gsr class.
   Parms: Channel Pin Number , Enable Pin Number , ADC refrence voltage.
   Return: void
*/
gsr::gsr( const short int& channel, const short int& module_en_pin, const double& adc_voltage ) {
    pinMode( channel,  INPUT);
    pinMode( module_en_pin , OUTPUT);
    digitalWrite( module_en_pin , LOW );
    adc_channel = channel;
    gsr_module_power = module_en_pin;
    v_adc = adc_voltage;
}

/* Set the resoulution of the ADC in use. 
   Parms: 2^Resolution ( 1024 for 10Bit ADC )
   Return: void
*/
void gsr::set_adc_resolution ( const double& value ) {
    adc_resolution = value;
}

/* Set the Voltage of the ADC in use. 
   Parms: Voltage (Volts)
   Return: void
*/
void gsr::set_adc_voltage ( const double& value ) {
    v_adc = value;
}

/* Set the number of samples to take for each measure. 
   Parms: Number of samples.
   Return: void
*/
void gsr::set_averaging_samples( const short& samples ) {
    sample_width = samples < MAXIMUM_BUFFER_SIZE ? samples : MAXIMUM_BUFFER_SIZE;
}
