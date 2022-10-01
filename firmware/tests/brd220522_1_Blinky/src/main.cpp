#include <Arduino.h>

const int led_green = 3;
const int led_red = 5;

void setup() {
  // put your setup code here, to run once:
  pinMode(led_green, OUTPUT);
  pinMode(led_red, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(led_green, HIGH);
  digitalWrite(led_red, LOW);
  delay(500);
  digitalWrite(led_green, LOW);
  digitalWrite(led_red, HIGH);
  delay(500);
}