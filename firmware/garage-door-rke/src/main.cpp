/*
  Adaptation of example created by Tom Igoe, created 28 April 2017
*/
#include <SPI.h> // include libraries
#include "pins.h"
#include "AsyncDelay.h"
extern "C"
{
#include "bootloader_random.h"
}
#include "constants.h"

void sendMessage(String outgoing);
void onReceive(int packetSize);

bool radioTx(byte *packet, byte packetlength)
{
#ifdef DEBUG
  Serial.printf("Device sends %dbytes.\r\n", packetlength);
  showArray(packet, packetlength);
#endif
  LoRa.beginPacket();
  LoRa.write(packet, packetlength);
  LoRa.endPacket();
  return true;
}

void showArray(byte *data, byte len)
{
  for (int i = 0; i < len; i++)
  {
    Serial.printf("%02x ", data[i]);
    if ((i + 1) % 24 == 0)
    {
      Serial.println();
    }
  }
  Serial.println();
}

void setup()
{
  pinMode(pinEn_Pwr, OUTPUT);
  digitalWrite(pinEn_Pwr, HIGH);

  // Init serial port
  Serial.begin(115200);
  while (!Serial)
    ;
  Serial.printf("\r\nBuild %s\r\n", __TIMESTAMP__);

  // Init LoRa
  LoRa.setPins(pinnCS, pinRST_RADIO, pinDIO0, pinSCK, pinMOSI, pinMISO);
  if (!LoRa.begin(868E6))
  {
    while (true)
    {
      Serial.println("LoRa init failed. Check your connections.");
      delay(1000);
    }
  }
  // Init Kryptoknight
  bootloader_random_enable();
  if (sodium_init() < 0 || !setupKryptoKnight())
  {
    while (true)
    {
      Serial.println("Kryptoknight init failed. Check your connections.");
      delay(1000);
    }
  }
  #if REMOTE == 0
  //Init web server : for garage-controller only
  if(!webserver_setup())
  {
    while(true)
    {
      Serial.println("Web server init failed.");
      delay(1000);
    }
  }
  #endif

  Serial.println("Ready to rumble.");
}