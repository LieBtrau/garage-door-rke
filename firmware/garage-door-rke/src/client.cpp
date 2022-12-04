#include "constants.h"

#if REMOTE == 1
#include "KryptoknightClient.h"

static const ClientRecord myID = {0x002ba78c, {0xE4, 0xFF, 0x4B, 0x3C, 0x9C, 0x4D, 0x0F, 0xCD, 0xB3, 0x17, 0x8A, 0xA1, 0xE3, 0x51, 0x66, 0xEE, 0xE6, 0x1A, 0x77, 0x7C, 0x1E, 0xE1, 0x47, 0x56, 0x46, 0x73, 0x85, 0x3E, 0x81, 0x51, 0xDF, 0xB7}};
static KryptoknightClient client_2pap(myID.client_id, (byte *)myID.shared_secret_key, SERVER_ID);
static AsyncDelay timerStartAuthentication;
static int totalCount = 0;
static int successCount = 0;
static uint8_t buf[BUFF_SIZE];

bool setupKryptoKnight()
{
    client_2pap.setTransmitPacketEvent(radioTx);
    timerStartAuthentication.start(2000, AsyncDelay::MILLIS);
    return true;
}

void loop()
{
    if (timerStartAuthentication.isExpired())
    {
        timerStartAuthentication.restart();
        Serial.printf("TotalCount: %d, FailCount: %d\r\n", totalCount, totalCount - successCount);
        client_2pap.startAuthentication();
        totalCount++;
    }
}

void onReceive(int packetSize)
{
    if (packetSize == 0)
        return; // if there's no packet, return

    if (LoRa.readBytes(buf, min(packetSize,BUFF_SIZE)) == packetSize)
    {
#ifdef VERBOSE
        USBSerial.printf("Client receives %dbytes.\r\n", len);
        showArray(buf, len);
#endif
        if (client_2pap.handleIncomingPacket(buf, packetSize))
        {
            Serial.println("Authentication successful");
            successCount++;
        }
    }
    else
    {
        Serial.println("invalid message");
    }
}

#endif