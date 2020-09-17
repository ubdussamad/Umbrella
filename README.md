# Umbrella
Code repository for the Umbrella Bio-Medical Device.

Part of an on going reasearch project.
This devices includes, GSR, Pulse Oximeter and few other sensors.
The data genrated can be analysed for research purposes.

![Front view of the biosensor device.](https://github.com/ubdussamad/Umbrella/raw/master/data/photo.jpeg "Second Version")


# Hardware
The hardware files are available in the hardware folder,
which includes schematics and PCB layout design files.

# Library Dependencies
This project needs few libraries to work:
 * Max30100 lib (I2C Connection , thus Wire library is required too.)
   Although this library includes all the DSP code to do basic pulse
   oximetry, but the state machine which detects the heart beat is NOT
   upto the standard.
   Thus I'll be replacing it with sparkfun's max30101 board which also has
   a specialized  cortex for DSP butlit by maxim integrated.
 * U8g2 lib
   Library for Driving the OLED display.
 * MPU 6050 lib
   For driving the mpu6050 sensor.

# Usage:
Although this firmware is not written in avery generic way, you could still easily understand whats going on
just by going through the code itself as it contains decent amount of documentation.
I am planning to make this firmware generic-ish as soon as I am done with writing the whole of it.

This code is compiled from the arduino IDE with target board being ESP32, the main file is ```umbrella.ino```


## Version
2.0 R:4 16AUG2020 (Last Version Update)

## Author
ubdussamad <ubdussamad@gmail.com>
Opensouce Firmware.
