/**
 * @file main.cpp
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2022-05-13
 *
 * @copyright Copyright (c) 2022
 *
 */
#include <Arduino.h>
#include "SPI.h"
#include "KryptoknightClient.h"
#include <RH_RF69.h>
#include "AsyncDelay.h"
extern "C"
{
#include "bootloader_random.h"
}

#define VERBOSE

/**
 * @brief
 *
 * Signal   LOLIN C3-MINI
 * SCK      10
 * MISO     4
 * MOSI     7
 * NSS      6
 * DIO0     2
 * 3V3		3V3
 * GND		GND
 */

RH_RF69 driver(6);
uint8_t buf[RH_RF69_MAX_MESSAGE_LEN];
unsigned char shared_secret_key[crypto_auth_KEYBYTES] =
	{0xE4, 0xFF, 0x4B, 0x3C, 0x9C, 0x4D, 0x0F, 0xCD, 0xB3, 0x17, 0x8A, 0xA1, 0xE3, 0x51, 0x66, 0xEE,
	 0xE6, 0x1A, 0x77, 0x7C, 0x1E, 0xE1, 0x47, 0x56, 0x46, 0x73, 0x85, 0x3E, 0x81, 0x51, 0xDF, 0xB7};
KryptoknightClient client_2pap(0x002ba78c, shared_secret_key, 0x00697960);
AsyncDelay timerStartAuthentication;
int totalCount = 0;
int successCount = 0;

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
		USBSerial.printf("%02x ", data[i]);
		if ((i + 1) % 24 == 0)
		{
			USBSerial.println();
		}
	}
	USBSerial.println();
}

bool clientTx(byte *packet, byte packetlength)
{
#ifdef VERBOSE
	USBSerial.printf("Client sends %dbytes.\r\n", packetlength);
	showArray(packet, packetlength);
#endif
	if (!driver.waitPacketSent(1))
	{
#ifdef VERBOSE
		USBSerial.println("Driver has not finished sending.  Check 3V3 of the radio");
#endif
		return false;
	}
	return driver.send(packet, packetlength);
}

void setup()
{
	USBSerial.begin(115200);
	while (!USBSerial)
		;
	USBSerial.printf("\r\nBuild %s\r\n", __TIMESTAMP__);
	bootloader_random_enable();
	USBSerial.printf("Chip ID: 0x%08x\r\n", getEsp32UniqueId());
	if (sodium_init() < 0)
	{
		/* panic! the library couldn't be initialized; it is not safe to use */
		while (1)
			;
	}

	client_2pap.setTransmitPacketEvent(clientTx);
	//client_2pap.setMutualAuthentication(false);

	if (!driver.init())
	{
		USBSerial.println("init failed");
		while (true)
			;
	}
	driver.setFrequency(868.0);
	driver.setTxPower(0, true);
	driver.setModemConfig(RH_RF69::GFSK_Rb2Fd5);
	timerStartAuthentication.start(2000, AsyncDelay::MILLIS);
	USBSerial.println("Ready to rumble");
}

void loop()
{
	if (timerStartAuthentication.isExpired())
	{
		timerStartAuthentication.restart();
		USBSerial.printf("TotalCount: %d, FailCount: %d\r\n", totalCount, totalCount - successCount);
		client_2pap.startAuthentication();
		totalCount++;
	}
		if (driver.available())
		{
			// Wait for a message addressed to us from the client
			uint8_t len = sizeof(buf);
			uint8_t from;
			if (driver.recv(buf, &len))
			{
	#ifdef VERBOSE
				USBSerial.printf("Client receives %dbytes.\r\n", len);
				showArray(buf, len);
	#endif
				if (client_2pap.handleIncomingPacket(buf, len))
				{
					USBSerial.println("Authentication successful");
					successCount++;
				}
			}
			else
			{
				USBSerial.println("invalid message");
			}
		}
}