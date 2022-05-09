#include <Arduino.h>
#include "KryptoknightClient.h"
#include "KryptoknightServer.h"
extern "C"
{
#include "bootloader_random.h"
}

KryptoknightServer* pserver_2pap=nullptr;
KryptoknightClient* pclient_2pap=nullptr;

uint32_t getEsp32UniqueId()
{
	uint32_t chipId = 0;
	for (int i = 0; i < 17; i = i + 8)
	{
		chipId |= ((ESP.getEfuseMac() >> (40 - i)) & 0xff) << i;
	}
	return chipId;
}

bool clientTx(byte* packet, byte packetlength)
{
	Serial.printf("Client sends %dbytes.\r\n", packetlength);
	if(pserver_2pap->rx_handler(packet, packetlength))
	{
		Serial.println("Server: authentication ok.");
	}
	return true;
}

bool serverTx(byte* packet, byte packetlength)
{
	Serial.printf("Server sends %dbytes.\r\n", packetlength);
	if(pclient_2pap->rx_handler(packet, packetlength))
	{
		Serial.println("Client: authentication ok.");
	}
	return true;
}

void setup()
{
	unsigned char shared_secret_key[crypto_auth_KEYBYTES];
	uint32_t id_client;
	Serial.begin(115200);
	Serial.printf("\r\nBuild %s\r\n", __TIMESTAMP__);
	bootloader_random_enable();
	Serial.printf("Chip ID: 0x%08x\r\n", getEsp32UniqueId());
	if (sodium_init() < 0)
	{
		/* panic! the library couldn't be initialized; it is not safe to use */
		while (1)
			;
	}

	// Define Alice's and Bob's identifications
	id_client = getEsp32UniqueId();
	// Generate the shared secret key.  Both parties should have this key before the start of the session.
	crypto_auth_keygen(shared_secret_key);

	//Init server
	KryptoknightServer server_2pap(shared_secret_key);
	server_2pap.set_tx_handler(serverTx);
	pserver_2pap = &server_2pap;

	//Init client
	KryptoknightClient client_2pap(id_client, shared_secret_key);
	client_2pap.set_tx_handler(clientTx);
	pclient_2pap = &client_2pap;

	//Disable mutual authentication
	client_2pap.setMutualAuthentication(false);
	server_2pap.setMutualAuthentication(false);

	//Start authentication
	client_2pap.startAuthentication();
}

void loop()
{
	// put your main code here, to run repeatedly:
}