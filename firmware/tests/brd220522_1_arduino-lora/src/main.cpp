/*
  LoRa register dump

  This examples shows how to inspect and output the LoRa radio's
  registers on the Serial interface

  Patching LoRa.cpp, line 120 to match SPI-connections on BRD220522_1

*/
#include <SPI.h> // include libraries
#include <LoRa.h>

const int PIN_EN_PWR=4;

void setup()
{
  pinMode(PIN_EN_PWR, OUTPUT);
  digitalWrite(PIN_EN_PWR, HIGH);
  Serial.begin(115200); // initialize serial
  while (!Serial)
    ;

  Serial.println("LoRa Dump Registers");
  delay(500);

  // override the default CS, reset, and IRQ pins (optional)
  LoRa.setPins(6 /*CS*/, 3 /*RST*/, 2 /*IRQ*/); // set CS, reset, IRQ pin

  if (!LoRa.begin(868E6))
  { // initialize ratio at 868 MHz
    Serial.println("LoRa init failed. Check your connections.");
    while (true)
      ; // if failed, do nothing
  }

  LoRa.dumpRegisters(Serial);
  LoRa.setTxPower(20);
}

void loop()
{
  static int counter = 0;
  Serial.print("Sending packet: ");
  Serial.println(counter);

  // send packet
  LoRa.beginPacket();
  LoRa.print("hello ");
  LoRa.print(counter);
  LoRa.endPacket();

  counter++;

  delay(1000);
}
