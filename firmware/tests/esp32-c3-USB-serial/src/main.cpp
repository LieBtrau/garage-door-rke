#include <Arduino.h>
/**
 * @brief Checking influence of USB-macros on USBSerial port behavior
 * 
 * ARDUINO_USB_CDC_ON_BOOT=1 && ARDUINO_USB_MODE=1
 *    default
 *    Serial = HWCDC
 *    Serial0 = HardwareSerial
 * 
 * ARDUINO_USB_CDC_ON_BOOT=1 && ARDUINO_USB_MODE=0
 *    no HWCDC
 *    Serial0 = HardwareSerial
 * 
 * ARDUINO_USB_CDC_ON_BOOT=0 && ARDUINO_USB_MODE=1
 *    Serial = HardwareSerial
 *    USBSerial = HWCDC
 * 
 * ARDUINO_USB_CDC_ON_BOOT=0 && ARDUINO_USB_MODE=0
 *    Serial = HardwareSerial
 *    no HWCDC
 */
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println("Serial0");
  delay(1000);
}