/**
 * @file main.cpp
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2022-05-29
 *
 * @copyright Copyright (c) 2022
 *
 */
#include "Arduino.h"
#include "esp_adc_cal.h"

esp_adc_cal_characteristics_t adc_chars;
const int pin_ADC_EN = 0;

void setup()
{
	pinMode(pin_ADC_EN, OUTPUT);
	digitalWrite(pin_ADC_EN, HIGH);

	// initialize serial communication at 115200 bits per second:
	Serial.begin(115200);
	while (!Serial)
		;
	ESP_ERROR_CHECK(esp_adc_cal_check_efuse(ESP_ADC_CAL_VAL_EFUSE_TP));
	Serial.println(esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_6, ADC_WIDTH_BIT_12, 0, &adc_chars), DEC);
	adc1_config_channel_atten(ADC1_CHANNEL_1, ADC_ATTEN_DB_6);
}

void loop()
{
	uint32_t voltage;
	if (esp_adc_cal_get_voltage(ADC_CHANNEL_1, &adc_chars, &voltage) == ESP_OK)
	{
		Serial.printf("ESP-IDF : ADC analog value = %d\n", voltage);
	}
	delay(100); // delay in between reads for clear read from serial
}