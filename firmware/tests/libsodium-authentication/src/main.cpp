/**
 * @file main.cpp
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2022-05-01
 * 
 * @copyright Copyright (c) 2022
 * 
 * Mutual authentication of two parties using Kryptoknight-2PAP
 * //https://books.google.be/books?id=ieJ6CwAAQBAJ&pg=PA152&dq=2PAP&hl=en&sa=X&ved=2ahUKEwjs7rzzs773AhV-hv0HHYIgB-EQ6AF6BAgGEAI#v=onepage&q=2PAP&f=false
 * 
 */

#include <Arduino.h>
extern "C"
{
#include "bootloader_random.h"
}
#include "sodium.h"

unsigned char shared_secret_key[crypto_auth_KEYBYTES];
uint32_t id_alice;
uint32_t id_bob;

uint32_t getEsp32UniqueId()
{
	uint32_t chipId = 0;
	for (int i = 0; i < 17; i = i + 8)
	{
		chipId |= ((ESP.getEfuseMac() >> (40 - i)) & 0xff) << i;
	}
	return chipId;
}

void setup()
{
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
	id_alice = getEsp32UniqueId();
	id_bob = getEsp32UniqueId() + 1;
	// Generate the shared secret key.  Both parties should have this key before the start of the session.
	crypto_auth_keygen(shared_secret_key);

	// Start of the protocol
	// Message 1: Alice sends Nonce_A to Bob
	byte nonce_A[crypto_secretbox_NONCEBYTES];
	randombytes_buf(nonce_A, crypto_secretbox_NONCEBYTES);
	Serial.printf("Message 1 size: %dbytes.\r\n", sizeof(nonce_A));
	// Message 2: Bob sends the following to Alice : Nonce_B, MACba(Nonce_A, Nonce_B, id_bob)
	// Bob creates nonce_B
	byte nonce_B[crypto_secretbox_NONCEBYTES];
	randombytes_buf(nonce_B, crypto_secretbox_NONCEBYTES);
	// Bob creates message_ba which is the concatentation of nonce_A, nonce_B and id_bob
	byte message_ba[sizeof(nonce_A) + sizeof(nonce_B) + sizeof(id_bob)];
	memcpy(message_ba, nonce_A, sizeof(nonce_A));
	memcpy(message_ba + sizeof(nonce_A), nonce_B, sizeof(nonce_B));
	memcpy(message_ba + sizeof(nonce_A)+ sizeof(nonce_B), &id_bob, sizeof(id_bob));
	// Bob calculates mac_ba from message_ba
	unsigned char mac_ba[crypto_auth_BYTES];
	crypto_auth(mac_ba, message_ba, sizeof(message_ba), shared_secret_key);
	Serial.printf("Message 2 size: %dbytes.\r\n", sizeof(nonce_B)+ sizeof(mac_ba));
	//Alice verifies Message 2 from Bob
	if (crypto_auth_verify(mac_ba, message_ba, sizeof(message_ba), shared_secret_key) != 0)
	{
		Serial.println("Message 2 forged");
		while(1);
	}
	// Message 3 : Alice sends the following to Bob : MACab(Nonce_A, Nonce_B)
	// Alice creates message_ab which is the concatenation of nonce_A and nonce_B
	byte message_ab[sizeof(nonce_A) + sizeof(nonce_B)];
	memcpy(message_ab, nonce_A, sizeof(nonce_A));
	memcpy(message_ab + sizeof(nonce_A), nonce_B, sizeof(nonce_B));
	// Alice calculates mac_ab from message_ab
	unsigned char mac_ab[crypto_auth_BYTES];
	crypto_auth(mac_ab, message_ab, sizeof(message_ab), shared_secret_key);
	Serial.printf("Message 3 size: %dbytes.\r\n", sizeof(mac_ab));
	//Bob verifies Message 3 from Alice
	if (crypto_auth_verify(mac_ab, message_ab, sizeof(message_ab), shared_secret_key) != 0)
	{
		Serial.println("Message 3 forged");
		while(1);
	}
	Serial.println("Authentication ok");
}

void loop()
{
	// put your main code here, to run repeatedly:
}