/**
 * @file main.cpp
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2022-04-18
 *
 * Power LED removed because it's always ON.
 * https://docs.arduino.cc/learn/electronics/low-power
 * https://www.seeedstudio.com/Seeeduino-XIAO-Arduino-Microcontroller-SAMD21-Cortex-M0+-p-4426.html
 * Power through VCC, not through 3V3, otherwise current consumption goes up to 18µA
 * LowPower.sleep(5000); //	4.20µA
 * LowPower.deepSleep(5000); // also 4.20µA
 * wake-up on external GPIO enabled : 19µA
 *
 * @copyright Copyright (c) 2022
 *
 */

#include <Arduino.h>
#include "ArduinoLowPower.h"

// Pin used to trigger a wakeup
const int pin_wakeup = 8;
volatile int repetitions = 1;

void repetitionsIncrease()
{
	repetitions++;
}

void setup()
{
	pinMode(LED_BUILTIN, OUTPUT);
	pinMode(pin_wakeup, INPUT_PULLUP);
	//LowPower.attachInterruptWakeup(pin_wakeup, repetitionsIncrease, FALLING);
	// Serial.begin(115200);
	// while(!Serial);
}

void loop()
{
	for (int i = 0; i < repetitions; i++)
	{
		digitalWrite(LED_BUILTIN, LOW);
		delay(500);
		digitalWrite(LED_BUILTIN, HIGH);
		delay(500);
	}

	// Serial.print('a');
	// LowPower.sleep(5000);
	LowPower.deepSleep();
}