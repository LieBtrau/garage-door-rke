#include <Arduino.h>

const int PIN_ENPWR = 4;
const int PIN_LED_RED = 5;
const unsigned long startUpTime = 0;

void setup()
{
  // put your setup code here, to run once:
  pinMode(PIN_ENPWR, OUTPUT);
  digitalWrite(PIN_ENPWR, HIGH);

  pinMode(PIN_LED_RED, OUTPUT);
}

void loop()
{
  // put your main code here, to run repeatedly:
  if ((unsigned long)(millis() - startUpTime) > 3000)
  {
    digitalWrite(PIN_ENPWR, LOW);
  }
  digitalWrite(PIN_LED_RED, HIGH);
  delay(100);
  digitalWrite(PIN_LED_RED, LOW);
  delay(100);
}