#include <SPI.h>
#include <RH_MRF89.h>
#include "pins.h"


// Singleton instance of the radio driver
RH_MRF89 mrf89(PB0, PA4, PA1);

void setup()
{
	Serial.begin(9600);
	while (!Serial)
		; // wait for serial port to connect. Needed for native USB

	if (!mrf89.init())
		Serial.println("init failed");
	// Default after init is 1dBm, 915.4MHz, FSK_Rb20Fd40
	// But you can change that if you want:
	//  mrf89.setTxPower(RH_MRF89_TXOPVAL_M8DBM); // Min power -8dBm
	//  mrf89.setTxPower(RH_MRF89_TXOPVAL_13DBM); // Max power 13dBm
	if (!mrf89.setFrequency(868.0))
	{
		Serial.println("setFrequency failed");
	}
	//  if (!mrf89.setModemConfig(RH_MRF89::FSK_Rb200Fd200)) // Fastest
	//    Serial.println("setModemConfig failed");
	Serial.print("all ok");
}

void clientLoop()
{
	Serial.println("Sending to mrf89_server");
	// Send a message to mrf89_server
	uint8_t data[] = "Hello World!";
	mrf89.send(data, sizeof(data));

	mrf89.waitPacketSent();
	// Now wait for a reply
	uint8_t buf[RH_MRF89_MAX_MESSAGE_LEN];
	uint8_t len = sizeof(buf);

	if (mrf89.waitAvailableTimeout(3000))
	{
		// Should be a reply message for us now
		if (mrf89.recv(buf, &len))
		{
			Serial.print("got reply: ");
			Serial.println((char *)buf);
			//      Serial.print("RSSI: ");
			//      Serial.println(mrf89.lastRssi(), DEC);
		}
		else
		{
			Serial.println("recv failed");
		}
	}
	else
	{
		Serial.println("No reply, is mrf89_server running?");
	}
	delay(400);
}

void serverLoop()
{
	if (mrf89.available())
	{
		// Should be a message for us now
		uint8_t buf[RH_MRF89_MAX_MESSAGE_LEN];
		uint8_t len = sizeof(buf);
		if (mrf89.recv(buf, &len))
		{
			//      RH_MRF89::printBuffer("request: ", buf, len);
			Serial.print("got request: ");
			Serial.println((char *)buf);
			//      Serial.print("RSSI: ");
			//      Serial.println(mrf89.lastRssi(), DEC);

			// Send a reply
			uint8_t data[] = "And hello back to you";
			mrf89.send(data, sizeof(data));
			mrf89.waitPacketSent();
			Serial.println("Sent a reply");
		}
		else
		{
			Serial.println("recv failed");
		}
	}
	//  delay(10000);
	//  mrf89.printRegisters();
	//  while (1);
}

void loop()
{
	if (digitalRead(PIN_MODE_SELECT) == HIGH)
	{
		clientLoop();
	}
	else
	{
		serverLoop();
	}
}
