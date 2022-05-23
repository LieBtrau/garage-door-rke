/**
 * @file main.cpp
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2022-05-13
 * 
 * @copyright Copyright (c) 2022
 * 
Client sends 4bytes.
8c a7 2b 00 
Server sends 24bytes.
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 

Client sends 56bytes.
aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa aa 
a1 85 8a b2 36 7b 54 f9 d4 7b 2f 55 11 0d 52 73 09 91 6b 29 47 4b bf bb 
2e 60 0b 19 48 0c 0e c4 
Server: authentication ok.
 */

#include <Arduino.h>
#include "KryptoknightClient.h"
#include "KryptoknightServer.h"
extern "C"
{
#include "bootloader_random.h"
}

const uint32_t CLIENT_ID0 = 0x002ba78c;
const uint32_t SERVER_ID = 0x00697960;
KryptoknightServer *pserver_2pap = nullptr;
KryptoknightClient *pclient_2pap = nullptr;
unsigned char shared_secret_key[1][crypto_auth_KEYBYTES] =
{
	{0xE4, 0xFF, 0x4B, 0x3C, 0x9C, 0x4D, 0x0F, 0xCD, 0xB3, 0x17, 0x8A, 0xA1, 0xE3, 0x51, 0x66, 0xEE,
	 0xE6, 0x1A, 0x77, 0x7C, 0x1E, 0xE1, 0x47, 0x56, 0x46, 0x73, 0x85, 0x3E, 0x81, 0x51, 0xDF, 0xB7}
};

void showArray(byte *data, byte len)
{
	Serial.print("\t");
	for (int i = 0; i < len; i++)
	{
		Serial.printf("%02x ", data[i]);
		if((i+1) % 24 == 0)
		{
			Serial.print("\r\n\t");
		}
	}
	Serial.println();
}

bool clientTx(byte *packet, byte packetlength)
{
	Serial.printf("Client sends %dbytes.\r\n", packetlength);
	showArray(packet, packetlength);
	if (pserver_2pap->handleIncomingPacket(packet, packetlength))
	{
		Serial.println("Server: authentication ok.");
	}
	return true;
}

bool serverTx(byte *packet, byte packetlength)
{
	Serial.printf("Server sends %dbytes.\r\n", packetlength);
	showArray(packet, packetlength);
	if (pclient_2pap->handleIncomingPacket(packet, packetlength))
	{
		Serial.println("Client: authentication ok.");
	}
	return true;
}

byte* getSharedSecretKey(uint32_t client_id)
{
	switch (client_id)
	{
	case CLIENT_ID0:
		return shared_secret_key[0];
	default:
		Serial.printf("Unkown id: 0x%08lx\n",client_id);
		Serial.println(client_id, HEX);
		return nullptr;
	}
}

void setup()
{
	Serial.begin(115200);
	Serial.printf("\r\nBuild %s\r\n", __TIMESTAMP__);
	bootloader_random_enable();
	if (sodium_init() < 0)
	{
		/* panic! the library couldn't be initialized; it is not safe to use */
		while (1)
			;
	}

	// // Generate the shared secret key.  Both parties should have this key before the start of the session.
	// crypto_auth_keygen(shared_secret_key);

	// Init server
	KryptoknightServer server_2pap(SERVER_ID);
	server_2pap.setTransmitPacketEvent(serverTx);
	server_2pap.setGetKeyEvent(getSharedSecretKey);
	pserver_2pap = &server_2pap;

	// Init client
	KryptoknightClient client_2pap(CLIENT_ID0, shared_secret_key[0], SERVER_ID);
	client_2pap.setTransmitPacketEvent(clientTx);
	pclient_2pap = &client_2pap;

	// Disable mutual authentication
	client_2pap.setMutualAuthentication(true);
	server_2pap.setMutualAuthentication(true);

	// Start authentication
	client_2pap.startAuthentication();
}

void loop()
{
	// put your main code here, to run repeatedly:
}