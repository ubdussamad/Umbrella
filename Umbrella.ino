/* LifeSense 
 * V-1.0 REF 20JUN2020
 * Firmware for LifeSense.
 * This Piece of Software is NOT for pulic sharing.
 * Authour: Ubdussamad <ubdussamad@gmail.com>
 */

#define ADC0_CH1 36
#define GSR_PWR 18
#define LED_BLUE 32
#define LED_RED 33
#define I2C_SDA 21
#define I2C_SCL 22
#define FLASH_BTN 0


void setup () {

  pinMode( LED_BLUE, OUTPUT);
  pinMode( LED_RED , OUTPUT);
  pinMode( I2C_SDA , OUTPUT):
  pinMode( I2C_SCL , OUTPUT);
  pinMode( FLASH_BTN, INPUT);
}


void loop () {

  
}
