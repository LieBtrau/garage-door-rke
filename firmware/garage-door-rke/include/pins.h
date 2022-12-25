#pragma once

#if BRD220522_VERSION == 1
//SPI - signals
const int pinSCK = 10;
const int pinMOSI = 7;
const int pinMISO = 8;
const int pinnCS = 6;

//Radio connection 
const int pinDIO0 = 2;
const int pinRST_RADIO = 3;

//ADC-connection
const int pinSampleADC = 1;
const int pinEn_ADC = 0;

const int pinRedLED = 5;    // LoBatt indicator on remote, garage-door pulse on controller
const int pinEn_Pwr = 4;    // Enable power on remote, no function on controller

#elif BRD220522_VERSION == 2
//SPI - signals
const int pinSCK = 7;
const int pinMOSI = 0;
const int pinMISO = 1;
const int pinnCS = 2;

//Radio connection 
const int pinDIO0 = 6;
const int pinRST_RADIO = 8;

//ADC-connection
const int pinSampleADC = 3;
const int pinEn_ADC = 10;

const int pinRedLED = 4;    // LoBatt indicator on remote, garage-door pulse on controller
const int pinEn_Pwr = 5;

#else
#error "No pin definitions defined"
#endif