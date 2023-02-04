#include "constants.h"
#include "pins.h"
#include "secretKey.h"

#if REMOTE == 0
#include "KryptoknightServer.h"

static const char* TAG = "Server";

KryptoknightServer server_2pap(SERVER_ID);

// List of known clients
static const int CLIENT_LIST_COUNT = 1;
static const ClientRecord clientList[CLIENT_LIST_COUNT] = {
    0x002ba78c, SECRET_KEY};
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
            ESP_LOGE(TAG, "Web server init failed.");
            delay(1000);
        }
    }
}

void authenticationOkAction()
{
    
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
    ESP_LOGE(TAG, "Unkown id: 0x%08lx\n", client_id);
    return nullptr;
}

bool setupKryptoKnight()
{
    // Generate the shared secret key.  Both parties should have this key before the start of the session.
    // unsigned char shared_secret_key[crypto_auth_KEYBYTES];
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
    ESP_LOGI(TAG, "Device sends %dbytes.\r\n", packetSize);
#if LOG_LOCAL_LEVEL == ESP_LOG_VERBOSE
    showArray(buf, packetSize);
#endif

        if (server_2pap.handleIncomingPacket(buf, packetSize))
        {
            ESP_LOGD(TAG, "Server : Authentication successful");
            authenticationOkAction();
        }
    }
    else
    {
        ESP_LOGE(TAG, "invalid message");
    }
}

void loop()
{
    onReceive(LoRa.parsePacket());
}

#endif
