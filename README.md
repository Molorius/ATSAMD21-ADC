# ATSAMD21-ADC
Functions for many of the ADC capabilities of the Atmel SAMD21 series. Currently this requires some of the functions given in the Arduino IDE, but I will make it optional in the future. Tested on the Arduino MKR1000. 
Datasheet: https://cdn.sparkfun.com/datasheets/Dev/Arduino/Boards/Atmel-42181-SAM-D21_Datasheet.pdf

### uint8_t analogReadExtended(uint8_t bits)
Allows for the ADC to read 8, 10, or 12 bits normally or 13-16 bits using oversampling and decimation. This works with all analog read functions, including ***analogRead(uint8_t pin)*** given in the Arduino IDE. See pages 853 & 862 of the datasheet. 
 * ***8***, ***10***, ***12*** bit = 1 sample ~ 436 microseconds
 * ***13*** bit        = 4 samples ~ 1668 microseconds
 * ***14*** bit        = 16 samples ~ 6595 microseconds
 * ***15*** bit        = 64 samples ~ 26308 microseconds
 * ***16*** bit        = 256 samples ~ 105156 microseconds

### int16_t analogDifferential(uint8_t pos_pin, uint8_t neg_pin)
Reads a differential voltage with ***pos_pin*** and ***neg_pin***. These pins correspond to the values given in the Arduino headers. For example, to connect A0 and A3 on the Arduino use analogDifferential(A0,A3).  ***neg_pin*** cannot be A1 or A2 on the Arduino MKR1000, as these are connected internally to different pins on the main chip. See pages 869-870 of the datasheet for more information on that. This code performs very similarly to ***analogRead(uint8_t pin)*** given in the Arduino IDE, but puts it into differential mode instead of single-ended. It also shuts off the DAC if one of the pins you selected is connected to it internally. 

### int16_t analogDifferentialRaw(uint8_t mux_pos, uint8_t mux_neg)
This is very similar to the above ***analogDifferential(uint8_t pos_pin, uint8_t neg_pin)*** function above, but instead takes in takes in the hex values for the pins you want to connect. See pages 869-870 of the datasheet for more information on this. Note that the internal pin values and the pins broken out on the Arduino boards do not necessarily match. This also doesn't check that the pin used is internally connected to the DAC. 

***mux_pos*** values can be found on page 870 of the datasheet. These are:
* ***0***-***19*** - corresponding to ADC AIN 0-19 pins. 
* ***ADC_PIN_TEMP*** - Temperature reference
* ***ADC_PIN_BANDGAP*** - Bandgap voltage
* ***ADC_PIN_SCALEDCOREVCC*** - 1/4 scaled core supply
* ***ADC_PIN_SCALEDIOVCC*** - 1/4 scaled I/O supply 
* ***ADC_PIN_DAC*** - DAC output

***mux_neg*** values can be found on page 869 of the datasheet. These are:
* ***0***-***7*** - corresponding to ADC AIN 0-7 pins. 
* ***ADC_PIN_GND*** - Internal ground
* ***ADC_PIN_IOGND*** - I/O ground 

### void analogGain(uint8_t gain)
Sets the gain of the ADC. See pages 868-869 of the datasheet. Possible options are:
* ***ADC_GAIN_1*** - 1x
* ***ADC_GAIN_2*** - 2x
* ***ADC_GAIN_4*** - 4x
* ***ADC_GAIN_8*** - 8x
* ***ADC_GAIN_16*** - 16x
* ***ADC_GAIN_DIV2*** - 1/2x
