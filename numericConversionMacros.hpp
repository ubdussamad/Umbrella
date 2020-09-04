
#define FLT_TO_UINT16(m, e)         (((uint16_t)(m) & 0x0FFFU) | (uint16_t)((int16_t)(e) << 12))
#define uint16_t unsigned short
#define int16_t  signed short


#ifndef COMPILE
int main ()  {
    float x = -13;
    // uint16_t x =  (((uint16_t)(m) & 0x0FFFU) | (uint16_t)((int16_t)(e) << 12))

}


/**
 * For example, value 0xE113 :

exponent: 0xE = -2
mantissa: 0x113 = 275 

-> Actual value: 275 * 10^-2 = 2.75
*/

#endif