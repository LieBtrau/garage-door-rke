#include <Arduino.h>
#include "SPI.h"
#include "KryptoknightServer.h"
#include <RH_RF69.h>
extern "C"
{
#include "bootloader_random.h"
}

#define VERBOSE
/**
 * @brief
 *
 * Signal   NODEMCU32-S-pin
 * NSS      GPIO5
 * DIO0     GPIO4
 * MISO     GPIO19
 * MOSI     GPIO23
 * SCK      GPIO18
 * 3V3		3V3
 * GND		GND
 */

RH_RF69 driver(5, 4);
uint8_t buf[RH_RF69_MAX_MESSAGE_LEN];
unsigned char shared_secret_key[1][crypto_auth_KEYBYTES] =
{
	{0xE4, 0xFF, 0x4B, 0x3C, 0x9C, 0x4D, 0x0F, 0xCD, 0xB3, 0x17, 0x8A, 0xA1, 0xE3, 0x51, 0x66, 0xEE,
	 0xE6, 0x1A, 0x77, 0x7C, 0x1E, 0xE1, 0x47, 0x56, 0x46, 0x73, 0x85, 0x3E, 0x81, 0x51, 0xDF, 0xB7}
};
KryptoknightServer server_2pap(0x00697960);

uint32_t getEsp32UniqueId()
{
	uint32_t chipId = 0;
	for (int i = 0; i < 17; i = i + 8)
	{
		chipId |= ((ESP.getEfuseMac() >> (40 - i)) & 0xff) << i;
	}
	return chipId;
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

byte* getSharedSecretKey(uint32_t client_id)
{
	switch (client_id)
	{
	case 0x002ba78c:
		return shared_secret_key[0];
	default:
		Serial.printf("Unkown id: 0x%08lx\n",client_id);
		Serial.println(client_id, HEX);
		return nullptr;
	}
}

bool serverTx(byte *packet, byte packetlength)
{
#ifdef VERBOSE
	Serial.printf("Server sends %dbytes.\r\n", packetlength);
	showArray(packet, packetlength);
#endif
	return driver.send(packet, packetlength) && driver.waitPacketSent(500);
}

void setup()
{
	uint32_t id_client;
	Serial.begin(115200);
	while (!Serial)
		;
	Serial.printf("\r\nBuild %s\r\n", __TIMESTAMP__);
	Serial.printf("Chip ID: 0x%08x\r\n", getEsp32UniqueId());
	bootloader_random_enable();
	if (sodium_init() < 0)
	{
		/* panic! the library couldn't be initialized; it is not safe to use */
		while (1)
			;
	}

	// // Generate the shared secret key.  Both parties should have this key before the start of the session.
	// crypto_auth_keygen(shared_secret_key);
	// for (int i = 0; i < sizeof(shared_secret_key); i++)
	// {
	// 	Serial.printf("0x%02X,", shared_secret_key[i]);
	// }
	// Serial.println();

	server_2pap.setTransmitPacketEvent(serverTx);
	server_2pap.setGetKeyEvent(getSharedSecretKey);
	//server_2pap.setMutualAuthentication(false);

	if (!driver.init())
	{
		Serial.println("init failed");
		while (true)
			;
	}
	driver.setFrequency(868.0);
	driver.setTxPower(0, true);
	driver.setModemConfig(RH_RF69::GFSK_Rb2Fd5);
	Serial.println("Ready to rumble");
}

void loop()
{
	if (driver.available())
	{
		// Wait for a message addressed to us from the client
		uint8_t len = sizeof(buf);
		uint8_t from;
		if (driver.recv(buf, &len))
		{
#ifdef VERBOSE
			Serial.printf("Server receives %dbytes.\r\n", len);
			showArray(buf, len);
#endif
			if (server_2pap.handleIncomingPacket(buf, len))
			{
				Serial.println("Authentication successful");
			}
		}
		else
		{
			Serial.println("invalid message");
		}
	}
}