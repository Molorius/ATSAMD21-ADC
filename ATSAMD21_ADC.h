// Blake Felt
// ATSAMD21_ADC.h
// Adds some extra functions to the ADC, such as
// 16 bit and differential mode

#pragma once // only includes this once max

#ifdef __cplusplus
  extern "C" {
#endif

#ifndef ADC_CTRLB_RESSEL_12BIT_Val 
  #define ADC_CTRLB_RESSEL_8BIT_Val 0x3
  #define ADC_CTRLB_RESSEL_10BIT_Val 0x2
  #define ADC_CTRLB_RESSEL_12BIT_Val 0x0
#endif
#define ADC_CTRLB_RESSEL_16BIT_Val 0x1 // used for averaging mode output

#define RSHIFT  0x0 // pg 853
#define NOSHIFT 0x0
#define SAMPLES16 0x8 // pg 853
#define SAMPLES12 0x0

#define ADC_PIN_TEMP 0x18 // positive mux, pg 870
#define ADC_PIN_BANDGAP 0x19
#define ADC_PIN_SCALEDCOREVCC 0x1A
#define ADC_PIN_SCALEDIOVCC 0x1B
#define ADC_PIN_DAC 0x1C

#define ADC_PIN_GND 0x18 // negative mux, pg 869
#define ADC_PIN_IOGND 0x19

#define ADC_GAIN_1 0x0 // pg 868
#define ADC_GAIN_2 0x1
#define ADC_GAIN_4 0x2
#define ADC_GAIN_8 0x3
#define ADC_GAIN_16 0x4
#define ADC_GAIN1_DIV2 0xF

static int _ADCRes = 10; // default for Arduino
static int _diffmode = 0; // it starts in single-ended mode

// Taken from Arduino IDE:
// Wait for synchronization of registers between the clock domains
static __inline__ void syncADC() __attribute__((always_inline, unused));
static void syncADC() {
  while(ADC->STATUS.bit.SYNCBUSY == 1);
}

// Taken from Arduino IDE:
// Wait for synchronization of registers between the clock domains
static __inline__ void syncDAC() __attribute__((always_inline, unused));
static void syncDAC() {
  while (DAC->STATUS.bit.SYNCBUSY == 1);
}

// taken from Arduino IDE, changes the pin to an input:
int pinPeripheral( uint32_t ulPin, EPioType ulPeripheral );

uint8_t analogReadExtended(uint8_t bits) {
/*
 * Allows for adc to read 8, 10, or 12 bits normally or 13-16 bits using oversampling and decimation.
 * See pages 853 & 862
 * 8,10,12 bit = 1 sample ~ 436 microseconds
 * 13 bit = 4 samples ~ 1668 microseconds
 * 14 bit = 16 samples ~ 6595 microseconds
 * 15 bit = 64 samples ~ 26308 microseconds
 * 16 bit = 256 samples ~ 105156 microseconds
 */
  switch(bits) {
    case 8:
      ADC->CTRLB.bit.RESSEL = ADC_CTRLB_RESSEL_8BIT_Val;
      ADC->AVGCTRL.bit.ADJRES = 0x0;
      ADC->AVGCTRL.bit.SAMPLENUM = 0x0;
      return 0;
    break;

    case 10:
      ADC->CTRLB.bit.RESSEL = ADC_CTRLB_RESSEL_10BIT_Val;
      ADC->AVGCTRL.bit.ADJRES = 0x0;
      ADC->AVGCTRL.bit.SAMPLENUM = 0x0;
      return 0;
    break;
    
    case 12:
      ADC->CTRLB.bit.RESSEL = ADC_CTRLB_RESSEL_12BIT_Val;
      ADC->AVGCTRL.bit.ADJRES = 0x0;
      ADC->AVGCTRL.bit.SAMPLENUM = 0x0;
      return 0;
    break;
      
    case 13:
      ADC->CTRLB.bit.RESSEL = ADC_CTRLB_RESSEL_16BIT_Val;
      ADC->AVGCTRL.bit.ADJRES = 0x1;
      ADC->AVGCTRL.bit.SAMPLENUM = 0x2;
      return 0;
    break;
    
    case 14:
      ADC->CTRLB.bit.RESSEL = ADC_CTRLB_RESSEL_16BIT_Val;
      ADC->AVGCTRL.bit.ADJRES = 0x2;
      ADC->AVGCTRL.bit.SAMPLENUM = 0x4;
      return 0;
    break;
    
    case 15:
      ADC->CTRLB.bit.RESSEL = ADC_CTRLB_RESSEL_16BIT_Val;
      ADC->AVGCTRL.bit.ADJRES = 0x1;
      ADC->AVGCTRL.bit.SAMPLENUM = 0x6;
      return 0;
    break;
    
    case 16:
      ADC->CTRLB.bit.RESSEL = ADC_CTRLB_RESSEL_16BIT_Val;
      ADC->AVGCTRL.bit.ADJRES = 0x0;
      ADC->AVGCTRL.bit.SAMPLENUM = 0x8;
      return 0;
    break;
    
    default:
      return -1;
    break;
   }
}

// modified from Arduino analogRead, can be used in conjunction with analogRead:
int16_t analogDifferential(uint8_t pin_pos,uint8_t pin_neg) {
  if(pin_pos<A0) pin_pos += A0;
  if(pin_neg<A0) pin_neg += A0;
  
  if((g_APinDescription[pin_neg].ulADCChannelNumber>0x07) && (pin_neg<ADC_PIN_GND)) { // if the negative pin is out of bounds
    return 0;
  }  
  
  uint32_t value_read = 0;
  
  pinPeripheral(pin_pos,PIO_ANALOG); // set pins to analog mode
  pinPeripheral(pin_neg,PIO_ANALOG);

  if((pin_pos == A0) || (pin_neg == A0)) { // Disable DAC
    syncDAC();
    DAC->CTRLA.bit.ENABLE = 0x00; // Disable DAC
    syncDAC();
  }

  syncADC();
  ADC->INPUTCTRL.bit.MUXPOS = g_APinDescription[pin_pos].ulADCChannelNumber; // Selection for the positive ADC input
  ADC->INPUTCTRL.bit.MUXNEG = g_APinDescription[pin_neg].ulADCChannelNumber; // negative ADC input

  syncADC();
  ADC->CTRLA.bit.ENABLE = 0x01; // enable adc
  ADC->CTRLB.bit.DIFFMODE = 1; // set to differential mode

  syncADC();
  ADC->SWTRIG.bit.START = 1; // start conversion

  ADC->INTFLAG.reg = ADC_INTFLAG_RESRDY; // clear the data ready flag
  syncADC();

  ADC->SWTRIG.bit.START = 1; // restart conversion, as changing inputs messes up first conversion
  
  while(ADC->INTFLAG.bit.RESRDY == 0);   // Wait for conversion to complete
  value_read = ADC->RESULT.reg; // read the value

  syncADC();
  ADC->CTRLA.bit.ENABLE = 0x00; // disable adc
  ADC->CTRLB.bit.DIFFMODE = 0; // put back into single-ended mode
  ADC->INPUTCTRL.bit.MUXNEG = ADC_PIN_GND; // set back muxneg to internal ground
  syncADC();

  return value_read;
}

// same as the above function, but no error checking, no pin types are changed, and the positive and negative
// inputs are the raw values being input. The DAC is not automatically shut off either. See datasheet page
int16_t analogDifferentialRaw(uint8_t mux_pos,uint8_t mux_neg) {
  
  uint32_t value_read = 0;

  syncADC();
  ADC->INPUTCTRL.bit.MUXPOS = mux_pos; // Selection for the positive ADC input
  ADC->INPUTCTRL.bit.MUXNEG = mux_neg; // negative ADC input

  syncADC();
  ADC->CTRLA.bit.ENABLE = 0x01; // enable adc
  ADC->CTRLB.bit.DIFFMODE = 1; // set to differential mode

  syncADC();
  ADC->SWTRIG.bit.START = 1; // start conversion

  ADC->INTFLAG.reg = ADC_INTFLAG_RESRDY; // clear the data ready flag
  syncADC();

  ADC->SWTRIG.bit.START = 1; // restart conversion, as changing inputs messes up first conversion
  
  while(ADC->INTFLAG.bit.RESRDY == 0);   // Wait for conversion to complete
  value_read = ADC->RESULT.reg; // read the value

  syncADC();
  ADC->CTRLA.bit.ENABLE = 0x00; // disable adc
  ADC->CTRLB.bit.DIFFMODE = 0; // put back into single-ended mode
  ADC->INPUTCTRL.bit.MUXNEG = ADC_PIN_GND; // set back muxneg to internal ground
  syncADC();

  return value_read;
}

// sets the gain of the ADC. See page 868. All values defined above. 
void analogGain(uint8_t gain) {
  syncADC();
  ADC->INPUTCTRL.bit.GAIN = gain;
  syncADC();
}

#ifdef __cplusplus
  }
#endif
