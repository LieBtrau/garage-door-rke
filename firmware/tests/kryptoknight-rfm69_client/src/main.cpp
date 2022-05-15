/**
 * @file main.cpp
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2022-05-13
 * 
 * @copyright Copyright (c) 2022
 * 
 * 	Client sends 4bytes.
		8c a7 2b 00 
	Server sends 24bytes.
		aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
	Client sends 56bytes.
		aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
		a1 85 8a b2 36 7b 54 f9 d4 7b 2f 55 11 0d 52 73 09 91 6b 29 47 4b bf bb 
		2e 60 0b 19 48 0c 0e c4 
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
KryptoknightClient client_2pap(0x002ba78c, shared_secret_key);
AsyncDelay timerStartAuthentication;

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
		if((i+1) % 24 == 0)
		{
			USBSerial.println();
		}
	}
	USBSerial.println();
}

bool clientTx(byte *packet, byte packetlength)
{
	USBSerial.printf("Client sends %dbytes.\r\n", packetlength);
	showArray(packet, packetlength);
	return driver.send(packet, packetlength) && driver.waitPacketSent(500);
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
	timerStartAuthentication.start(3000, AsyncDelay::MILLIS);
}

void loop()
{
	if(timerStartAuthentication.isExpired())
	{
		timerStartAuthentication.restart();
		client_2pap.startAuthentication();
	}
	if (driver.available())
	{
		// Wait for a message addressed to us from the client
		uint8_t len = sizeof(buf);
		uint8_t from;
		if (driver.recv(buf, &len))
		{
			USBSerial.printf("Client receives %dbytes.\r\n", len);
			showArray(buf, len);
			if(client_2pap.handleIncomingPacket(buf, len))
			{
				USBSerial.println("Authentication successful");
			}
		}
		else
		{
			USBSerial.println("invalid message");
		}
	}
}