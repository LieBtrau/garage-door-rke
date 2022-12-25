#include "constants.h"
#include "pins.h"

#if REMOTE == 0
#include "KryptoknightServer.h"

KryptoknightServer server_2pap(SERVER_ID);

// List of known clients
static const int CLIENT_LIST_COUNT = 1;
static const ClientRecord clientList[CLIENT_LIST_COUNT] = {
    0x002ba78c, {0xE4, 0xFF, 0x4B, 0x3C, 0x9C, 0x4D, 0x0F, 0xCD, 0xB3, 0x17, 0x8A, 0xA1, 0xE3, 0x51, 0x66, 0xEE, 0xE6, 0x1A, 0x77, 0x7C, 0x1E, 0xE1, 0x47, 0x56, 0x46, 0x73, 0x85, 0x3E, 0x81, 0x51, 0xDF, 0xB7}};
static uint8_t buf[BUFF_SIZE];

void presetup()
{
    //Initialize pulse output to gate control.
    pinMode(pinRedLED, OUTPUT);
    digitalWrite(pinRedLED, LOW);
}

void postsetup()
{
    if (!webserver_setup())
    {
        while (true)
        {
            Serial.println("Web server init failed.");
            delay(1000);
        }
    }
}

void authenticationOkAction()
{
    Serial.println("opening door");
    digitalWrite(pinRedLED, HIGH);
    delay(500);
    digitalWrite(pinRedLED, LOW);
}

byte *getSharedSecretKey(uint32_t client_id)
{
    for (int i = 0; i < CLIENT_LIST_COUNT; i++)
    {
        if (clientList[i].client_id == client_id)
        {
            return (byte *)clientList[i].shared_secret_key;
        }
    }
    Serial.printf("Unkown id: 0x%08lx\n", client_id);
    return nullptr;
}

bool setupKryptoKnight()
{
    // // Generate the shared secret key.  Both parties should have this key before the start of the session.
    // crypto_auth_keygen(shared_secret_key);
    // for (int i = 0; i < sizeof(shared_secret_key); i++)
    // {
    // 	Serial.printf("0x%02X,", shared_secret_key[i]);
    // }
    // Serial.println();

    server_2pap.setTransmitPacketEvent(radioTx);
    server_2pap.setGetKeyEvent(getSharedSecretKey);
    return true;
}

void onReceive(int packetSize)
{
    if (packetSize == 0)
        return; // if there's no packet, return

    if (LoRa.readBytes(buf, min(packetSize, BUFF_SIZE)) == packetSize)
    {
#ifdef DEBUG
        Serial.printf("Device receives %dbytes.\r\n", packetSize);
        showArray(buf, packetSize);
#endif
        if (server_2pap.handleIncomingPacket(buf, packetSize))
        {
            Serial.println("Server : Authentication successful");
            authenticationOkAction();
        }
    }
    else
    {
        Serial.println("invalid message");
    }
}

void loop()
{
    onReceive(LoRa.parsePacket());
}

#endif
