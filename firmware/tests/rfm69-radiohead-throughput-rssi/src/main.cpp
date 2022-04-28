/**
 * @brief NODEMCU32-S to RFM69 connections
 *
 * Signal   NODEMCU32-S		Bluepill
 * --------------------------------------
 * NSS      GPIO5			PA4
 * DIO0     GPIO4			PB0
 * MISO     GPIO19			PA6
 * MOSI     GPIO23			PA7
 * SCK      GPIO18			PA5
 * 3V3		3V3				3.3
 * GND		GND				G
 */

#include <RH_RF69.h>
#include <SPI.h>
#include "AsyncDelay.h"

#ifdef ARDUINO_NodeMCU_32S
RH_RF69 driver(5, 4);
bool isClient = false;
#elif defined(ARDUINO_BLUEPILL_F103C8)
RH_RF69 driver(PA4, PB0);
bool isClient = true;
#else
#error "Please define a hardware platform"
#endif


AsyncDelay wperfTimer;
unsigned long startInterval = 0;
const int PACKET_INTERVAL_ms = 40; // in ms
const int PACKET_SIZE = 6;		   // in bytes
int packetCount = 0;
float averageRssi = 0;
int totalBytes = 0;
byte buf[RH_RF69_MAX_MESSAGE_LEN];

void setup()
{
	Serial.begin(115200);
	while (!Serial)
		;
	delay(1000);
	Serial.printf("Build %s\r\n", __TIMESTAMP__);
	if (isClient)
	{
		wperfTimer.start(PACKET_INTERVAL_ms, AsyncDelay::MILLIS);
	}
	else
	{
		pinMode(LED_BUILTIN, OUTPUT);
	}
	Serial.println("* Initializing radio...");
	if (!driver.init())
	{
		Serial.println("init failed");
		while (true)
			;
	}
	Serial.println("init ok");
	// If you are using a high power RF69 eg RFM69HW, you *must* set a Tx power with the
	// ishighpowermodule flag set like this:
	driver.setFrequency(868.0);
	driver.setTxPower(0, true);
	driver.setModemConfig(RH_RF69::GFSK_Rb2Fd5);
}

void clientloop()
{
	uint8_t data[PACKET_SIZE];
	if (wperfTimer.isExpired())
	{
		wperfTimer.repeat();
		data[0] = packetCount++;
		if (packetCount == 250)
		{
			packetCount = 0;
		}
		if (!driver.send(data, sizeof(data)) || !driver.waitPacketSent())
		{
			Serial.println("Packet TX error.");
		}
	}
}

void serverloop()
{
	if (driver.available())
	{
		digitalWrite(LED_BUILTIN, digitalRead(LED_BUILTIN)==HIGH ? LOW : HIGH);
		uint8_t len = sizeof(buf);
		if (driver.recv(buf, &len))
		{
			totalBytes += len;
			packetCount++;
			averageRssi += driver.lastRssi();
			if (buf[0] == 0)
			{
				int bitrate = (totalBytes << 3) * 1000 / (millis() - startInterval);
				Serial.printf("Total bytes : %d\tTotal packets : %d\tBitrate : %d bps", totalBytes, packetCount, bitrate);
				if (packetCount > 0)
				{
					Serial.printf("\tAverage RSSI : %.2fdBm\r\n", averageRssi / packetCount);
				}
				else
				{
					Serial.println();
				}
				packetCount = 0;
				averageRssi = 0;
				totalBytes = 0;
				startInterval = millis();
			}
		}
		else
		{
			Serial.println("recv failed");
		}
	}
}

void loop()
{
	if (!isClient)
	{
		serverloop();
	}
	else
	{
		clientloop();
	}
}