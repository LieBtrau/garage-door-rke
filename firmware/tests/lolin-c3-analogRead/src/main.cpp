/**
 * @file main.cpp
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2022-05-29
 *
 * @copyright Copyright (c) 2022
 *
 * - analogReadMilliVolts() measurement voltages are all wrong.
 * - esp_adc_cal_get_voltage() is much better, but still 20 to 30mV off.
 * - Use ADC only in 150mV to 2500mV range for highest precision.
 * - written for ESP-IDF v4.4.0
 */
#include "Arduino.h"
#include "esp_adc_cal.h"

esp_adc_cal_characteristics_t adc_chars;

void setup()
{
	// initialize serial communication at 115200 bits per second:
	Serial.begin(115200);
	while (!Serial)
		;
	delay(5000);
	esp_adc_cal_value_t adc_calibration;

	//Lolin C3-Mini : ESP_ADC_CAL_VAL_EFUSE_VREF : not supported
	ESP_ERROR_CHECK(esp_adc_cal_check_efuse(ESP_ADC_CAL_VAL_EFUSE_TP));

	// set the resolution to 12 bits (0-4095)
	analogReadResolution(12);

	Serial.println(esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_12, 0, &adc_chars), DEC);
	adc1_config_channel_atten(ADC1_CHANNEL_1, ADC_ATTEN_DB_11);
}

void loop()
{
	// // read the analog / millivolts value for pin 1:
	// int analogValue = analogRead(1);
	int analogVolts = analogReadMilliVolts(1);
	// print out the values you read:
	// DebugPort.printf("ADC analog value = %d\n", analogValue);
	Serial.printf("ADC millivolts value = %d\n", analogVolts);

	uint32_t voltage;
	if (esp_adc_cal_get_voltage(ADC_CHANNEL_1, &adc_chars, &voltage) == ESP_OK)
	{
		 Serial.printf("ESP-IDF : ADC analog value = %d\n", voltage);
	}
	delay(100); // delay in between reads for clear read from serial
}
