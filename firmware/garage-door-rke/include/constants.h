#pragma once

#include "Arduino.h"
#include "sodium.h"
#include <LoRa.h>

//ESP-IDF has an option to set this, but we're using PlatformIO.
#define CONFIG_LOG_MAXIMUM_LEVEL ESP_LOG_DEBUG

//Application layer : Kryptoknight
const uint32_t SERVER_ID = 0x00697960; //!< 32-bit server address
typedef struct
{
    uint32_t client_id;
    byte shared_secret_key[crypto_auth_KEYBYTES];
} ClientRecord;

const int BUFF_SIZE = 128;
bool radioTx(byte *packet, byte packetlength);
bool setupKryptoKnight();
void showArray(byte *data, byte len);
void loop();
void presetup();
void postsetup();

bool webserver_setup();
void authenticationOkAction();