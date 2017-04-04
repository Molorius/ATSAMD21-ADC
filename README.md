# ATSAMD21-ADC
Functions for many of the ADC capabilities of the Atmel SAMD21 series. Currently this requires some of the functions given in the Arduino IDE, but I will make it optional in the future. Tested on the Arduino MKR1000. 
Datasheet: https://cdn.sparkfun.com/datasheets/Dev/Arduino/Boards/Atmel-42181-SAM-D21_Datasheet.pdf

### uint8_t analogReadExtended(uint8_t bits)
Allows for the ADC to read 8, 10, or 12 bits normally or 13-16 bits using oversampling and decimation. This works with all analog read functions, including ***analogRead(uint8_t pin)*** given in the Arduino IDE. See pages 853 & 862 of the datasheet. Arduino has *10* as default.  
 * ***8***, ***10***, ***12*** bit = 1 sample ~ 436 microseconds
 * ***13*** bit        = 4 samples ~ 1668 microseconds
 * ***14*** bit        = 16 samples ~ 6595 microseconds
 * ***15*** bit        = 64 samples ~ 26308 microseconds
 * ***16*** bit        = 256 samples ~ 105156 microseconds

### int16_t analogDifferential(uint8_t pos_pin, uint8_t neg_pin)
Reads a differential voltage with ***pos_pin*** and ***neg_pin***. These pins correspond to the values given in the Arduino headers. For example, to connect A0 and A3 on the Arduino use analogDifferential(A0,A3).  ***neg_pin*** cannot be A1 or A2 on the Arduino MKR1000, as these are connected internally to different pins on the main chip. See pages 869-870 of the datasheet for more information on that. This code performs very similarly to ***analogRead(uint8_t pin)*** given in the Arduino IDE, but puts it into differential mode instead of single-ended. It also shuts off the DAC if one of the pins you selected is connected to it internally. 

### int16_t analogDifferentialRaw(uint8_t mux_pos, uint8_t mux_neg)
This is very similar to the above ***analogDifferential(uint8_t pos_pin, uint8_t neg_pin)*** function above, but instead takes in takes in the hex values for the pins you want to connect. See pages 869-870 of the datasheet for more information on this. Note that the internal pin values and the pins broken out on the Arduino boards do not necessarily match. This also doesn't check that the pin used is internally connected to the DAC. 

*mux_pos* values can be found on page 870 of the datasheet. These are:
* ***0***-***19*** - corresponding to ADC AIN 0-19 pins. 
* ***ADC_PIN_TEMP*** - Temperature reference
* ***ADC_PIN_BANDGAP*** - Bandgap voltage
* ***ADC_PIN_SCALEDCOREVCC*** - 1/4 scaled core supply
* ***ADC_PIN_SCALEDIOVCC*** - 1/4 scaled I/O supply 
* ***ADC_PIN_DAC*** - DAC output

*mux_neg* values can be found on page 869 of the datasheet. These are:
* ***0***-***7*** - corresponding to ADC AIN 0-7 pins. 
* ***ADC_PIN_GND*** - Internal ground
* ***ADC_PIN_IOGND*** - I/O ground 

### uint8_t internalPinValue(uint8_t pin)
Returns the internal value of an external analog pin. Useful for determining pin numbers for the **analogDifferentialRaw** function above. 

### void analogGain(uint8_t gain)
Sets the gain of the ADC. See pages 868-869 of the datasheet. Arduino has *ADC_GAIN_DIV2* as default. Possible options are:
* ***ADC_GAIN_1*** - 1x
* ***ADC_GAIN_2*** - 2x
* ***ADC_GAIN_4*** - 4x
* ***ADC_GAIN_8*** - 8x
* ***ADC_GAIN_16*** - 16x
* ***ADC_GAIN_DIV2*** - 1/2x

### void analogCalibrate()
Copies the values from the NVM software calibration registry to the ADC CALIB registry. See page 882 of the datasheet. 

### void analogReference2(uint8_t ref)
This is similar to the ***analogReference*** function defined in the Arduino IDE, but that version alters the gain depending on the desired reference. This does not. Arduino has *ADC_REF_VCC1* as default. See page 861 of the datasheet. Possible options are:
* ***ADC_REF_INT1V*** - 1.0V reference
* ***ADC_REF_VCC0***  - 1/1.48 VDDANA
* ***ADC_REF_VCC1***  - 1/2 VDDANA
* ***ADC_REF_VREFA*** - external reference
* ***ADC_REF_VREFB*** - external reference

### void analogReferenceCompensation(uint8_t val)
Increases accuracy of gain stage by enabling the reference buffer offset compensation. Takes longer to start. See datasheet page 861. Arduino has it disabled by default. 
* ***0*** - disable the reference buffer offset compensation
* ***1*** - enable the reference buffer offset compensation

### void analogPrescaler(uint8_t val)
Sets the ADC clock relative to the peripheral clock. See page 864 of the datasheet. Arduino default is *ADC_PRESCALER_DIV512*
* ***ADC_PRESCALER_DIV4*** 
* ***ADC_PRESCALER_DIV8***
* ***ADC_PRESCALER_DIV16***
* ***ADC_PRESCALER_DIV32***
* ***ADC_PRESCALER_DIV64***
* ***ADC_PRESCALER_DIV128***
* ***ADC_PRESCALER_DIV256***
* ***ADC_PRESCALER_DIV512***

### void analogReset()
Resets all ADC registers except DBGCTRL back to their original values. Note that it resets them to the cpu defaults, not the Arduino IDE defaults. See page 860 of the datasheet. 
