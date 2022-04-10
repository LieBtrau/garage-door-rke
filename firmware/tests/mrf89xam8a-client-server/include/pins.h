#pragma once

#include "Arduino.h"

/**
 * @brief MRF89XAM8A Connection
 * 
 * Signal Name      Bluepill    MRF89XAM8A
 * GND              GND         1, 11, 12
 * RST              R           2
 * /CSCON           PIN_PB0     3
 * IRQ0                         4
 * SDI              PIN_PA7     5
 * SCK              PIN_PA5     6
 * SDO              PIN_PA6     7
 * /CSDATA          PIN_PA4     8
 * IRQ1             PIN_PA1     9
 * VIN              3.3         10
 * 
 */

/**
 * @brief Monitor port connection
 * PA9 : MCU-TX -> RX
 * 
 */
const byte PIN_MODE_SELECT = 3;