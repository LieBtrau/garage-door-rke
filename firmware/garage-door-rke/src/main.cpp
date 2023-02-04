/*
  Adaptation of example created by Tom Igoe, created 28 April 2017
*/
#include <SPI.h> // include libraries
#include "pins.h"
#include "AsyncDelay.h"

static const char* TAG = "Main";
extern "C"
{
#include "bootloader_random.h"
}
#include "constants.h"

void sendMessage(String outgoing);
void onReceive(int packetSize);

bool radioTx(byte *packet, byte packetlength)
{
  ESP_LOGI(TAG, "Device sends %dbytes.\r\n", packetlength);
#if LOG_LOCAL_LEVEL == ESP_LOG_VERBOSE
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
  esp_log_level_set("*", ESP_LOG_DEBUG);
  presetup();
  
  // Init serial port
  Serial.begin(115200);
  while (!Serial)
    ;
  ESP_LOGD(TAG, "\r\nBuild %s\r\n", __TIMESTAMP__);

  // Init LoRa
  LoRa.setPins(pinnCS, pinRST_RADIO, pinDIO0, pinSCK, pinMOSI, pinMISO);
  if (!LoRa.begin(868E6))
  {
    while (true)
    {
      ESP_LOGE(TAG, "LoRa init failed. Check your connections.");
      delay(1000);
    }
  }
  // Init Kryptoknight
  bootloader_random_enable();
  if (sodium_init() < 0 || !setupKryptoKnight())
  {
    while (true)
    {
      ESP_LOGE(TAG, "Kryptoknight init failed. Check your connections.");
      delay(1000);
    }
  }

  postsetup();

  ESP_LOGI(TAG, "Ready to rumble.");
}