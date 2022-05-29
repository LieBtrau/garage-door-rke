/**
 * @file main.cpp
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2022-05-29
 *
 * @copyright Copyright (c) 2022
 *
 *
 * - ADC is not calibrated on Lolin-c3, so mV-measurements are totally wrong.
 * - ADC 4095 = 2.76V
 * - max. ADC-value according to datasheet is 2.5V (ATTEN3)
 */
#include "Arduino.h"
#include "esp_adc_cal.h"

#define DebugPort USBSerial

void setup()
{
	// initialize serial communication at 115200 bits per second:
	DebugPort.begin(115200);
	while(!DebugPort);

	if (esp_adc_cal_check_efuse(ESP_ADC_CAL_VAL_EFUSE_VREF) != ESP_OK)
	{
		DebugPort.println("No reference voltage stored in eFuse");
	}
	if (esp_adc_cal_check_efuse(ESP_ADC_CAL_VAL_EFUSE_TP) != ESP_OK)
	{
		DebugPort.println("No characterization based on Two Point values stored in eFuse");
	}
	if (esp_adc_cal_check_efuse(ESP_ADC_CAL_VAL_DEFAULT_VREF) != ESP_OK)
	{
		DebugPort.println("No characterization based on default reference voltage stored in eFuse");
	}

	// set the resolution to 12 bits (0-4095)
	analogReadResolution(12);
}

void loop()
{
	// read the analog / millivolts value for pin 1:
	int analogValue = analogRead(1);
	int analogVolts = analogReadMilliVolts(1);

	// print out the values you read:
	DebugPort.printf("ADC analog value = %d\n", analogValue);
	DebugPort.printf("ADC millivolts value = %d\n", analogVolts);

	delay(100); // delay in between reads for clear read from serial
}
