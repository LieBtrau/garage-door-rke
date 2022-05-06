// rf69_reliable_datagram_server.pde
// -*- mode: C++ -*-
// Example sketch showing how to create a simple addressed, reliable messaging server
// with the RHReliableDatagram class, using the RH_RF69 driver to control a RF69 radio.
// It is designed to work with the other example rf69_reliable_datagram_client
// Tested on Moteino with RFM69 http://lowpowerlab.com/moteino/
// Tested on miniWireless with RFM69 www.anarduino.com/miniwireless
// Tested on Teensy 3.1 with RF69 on PJRC breakout board

#include <RHReliableDatagram.h>
#include <RH_RF69.h>
#include <SPI.h>

#define CLIENT_ADDRESS 1
#define SERVER_ADDRESS 2

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

// Singleton instance of the radio driver
RH_RF69 driver(6);

// Class to manage message delivery and receipt, using the driver declared above
RHReliableDatagram manager(driver, SERVER_ADDRESS);

uint8_t data[] = "And hello back to you";
// Dont put this on the stack:
uint8_t buf[RH_RF69_MAX_MESSAGE_LEN];
const int LED_BUILTIN = 7;

void setup()
{
	pinMode(LED_BUILTIN, OUTPUT);
	Serial.begin(115200);
	while (!Serial)
		;
	if (!manager.init())
	{
		Serial.println("init failed");
		while (true)
			;
	}
	// Defaults after init are 434.0MHz, modulation GFSK_Rb250Fd250, +13dbM (for low power module)

	// If you are using a high power RF69 eg RFM69HW, you *must* set a Tx power with the
	// ishighpowermodule flag set like this:
	driver.setFrequency(868.0);
	driver.setTxPower(0, true);
	Serial.println("server ready");
}

void loop()
{
	if (manager.available())
	{
		// Wait for a message addressed to us from the client
		uint8_t len = sizeof(buf);
		uint8_t from;
		if (manager.recvfromAck(buf, &len, &from))
		{
			Serial.print("got request from : 0x");
			Serial.print(from, HEX);
			Serial.print(": ");
			Serial.println((char *)buf);

			// Send a reply back to the originator client
			if (!manager.sendtoWait(data, sizeof(data), from))
			{
				Serial.println("sendtoWait failed");
			}
			digitalWrite(LED_BUILTIN, digitalRead(LED_BUILTIN) == HIGH ? LOW : HIGH);
		}
	}
}