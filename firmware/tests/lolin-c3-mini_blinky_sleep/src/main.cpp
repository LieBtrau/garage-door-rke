/**
 * @file main.cpp
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2022-05-06
 *
 * @copyright Copyright (c) 2022
 *
 * https://diyi0t.com/reduce-the-esp32-power-consumption/
 * 
 * Wemos Lolin C3 mini:
 * - deep sleep : 36µA (using ME6211C33)
 * - hibernation : 35µA (using ME6211C33)
 */
#include <Arduino.h>

#define uS_TO_S_FACTOR 1000000 /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP 5		   /* Time ESP32 will go to sleep (in seconds) */

void print_wakeup_reason()
{
	esp_sleep_wakeup_cause_t wakeup_reason;
	wakeup_reason = esp_sleep_get_wakeup_cause();

	switch (wakeup_reason)
	{
	case ESP_SLEEP_WAKEUP_EXT0:
		Serial.println("Wakeup caused by external signal using RTC_IO");
		break;
	case ESP_SLEEP_WAKEUP_EXT1:
		Serial.println("Wakeup caused by external signal using RTC_CNTL");
		break;
	case ESP_SLEEP_WAKEUP_TIMER:
		Serial.println("Wakeup caused by timer");
		break;
	case ESP_SLEEP_WAKEUP_TOUCHPAD:
		Serial.println("Wakeup caused by touchpad");
		break;
	case ESP_SLEEP_WAKEUP_ULP:
		Serial.println("Wakeup caused by ULP program");
		break;
	default:
		Serial.printf("Wakeup was not caused by deep sleep: %d\n", wakeup_reason);
		break;
	}
}

void setup()
{
	Serial.begin(9600);
	pinMode(7, OUTPUT);
	while (!Serial)
	{
		; // wait for serial port to connect. Needed for native USB
	}
	digitalWrite(7, HIGH);

	print_wakeup_reason(); //Print the wakeup reason for ESP32

	delay(5000);

	esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR); // ESP32 wakes up every 5 seconds

	esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_ON); // all RTC Peripherals are powered
	//esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_OFF); //Hibernation mode

	Serial.println("Going to deep-sleep now");
	Serial.flush();
	digitalWrite(7, LOW);
	esp_deep_sleep_start();
}

void loop()
{
}
