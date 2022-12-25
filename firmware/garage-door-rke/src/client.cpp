#include "constants.h"
#include "pins.h"

#if REMOTE == 1
#include "KryptoknightClient.h"
#include "esp_adc_cal.h"

static const ClientRecord myID = {0x002ba78c, {0xE4, 0xFF, 0x4B, 0x3C, 0x9C, 0x4D, 0x0F, 0xCD, 0xB3, 0x17, 0x8A, 0xA1, 0xE3, 0x51, 0x66, 0xEE, 0xE6, 0x1A, 0x77, 0x7C, 0x1E, 0xE1, 0x47, 0x56, 0x46, 0x73, 0x85, 0x3E, 0x81, 0x51, 0xDF, 0xB7}};
static KryptoknightClient client_2pap(myID.client_id, (byte *)myID.shared_secret_key, SERVER_ID);
static AsyncDelay timerStartAuthentication;
static esp_adc_cal_characteristics_t adc_chars;
static const uint32_t MIN_BAT_VOLTAGE = 3600; //[mV]

static uint8_t buf[BUFF_SIZE];

void authenticationOkAction()
{
    digitalWrite(pinRST_RADIO, LOW); // Light green LED && reset radio.
    delay(500);
}

static void powerOff()
{
    digitalWrite(pinEn_Pwr, LOW);
}

void presetup()
{
    // Make sure this device remains powered
    pinMode(pinEn_Pwr, OUTPUT);
    digitalWrite(pinEn_Pwr, HIGH);

    // Turn red LED off before ADC-measurement
    pinMode(pinRedLED, OUTPUT);
    digitalWrite(pinRedLED, LOW);

    // Enable ADC
    pinMode(pinEn_ADC, OUTPUT);
    digitalWrite(pinEn_ADC, HIGH);
}

void postsetup()
{
    uint32_t voltage;
    const int R5 = 3300;  // ohm
    const int R6 = 10000; // ohm

    // Get ADC-calibration data
    ESP_ERROR_CHECK(esp_adc_cal_check_efuse(ESP_ADC_CAL_VAL_EFUSE_TP));
    Serial.println(esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_6, ADC_WIDTH_BIT_12, 0, &adc_chars), DEC);
    // Configure the ADC with the calibration data
    adc1_config_channel_atten(ADC1_CHANNEL_1, ADC_ATTEN_DB_6);
    // Do the ADC-measurement
    if (esp_adc_cal_get_voltage(ADC_CHANNEL_1, &adc_chars, &voltage) == ESP_OK)
    {
        Serial.printf("ESP-IDF : ADC analog value = %dmV\n", voltage);
        int battery_voltage = voltage / R5 * (R5 + R6);
        if (battery_voltage < MIN_BAT_VOLTAGE)
        {
            digitalWrite(pinRedLED, HIGH);
        }
    }
    digitalWrite(pinEn_ADC, LOW);
}

bool setupKryptoKnight()
{
    client_2pap.setTransmitPacketEvent(radioTx);
    timerStartAuthentication.start(2000, AsyncDelay::MILLIS);
    return true;
}

void onReceive(int packetSize)
{
    if (packetSize == 0)
        return; // if there's no packet, return

    if (LoRa.readBytes(buf, min(packetSize, BUFF_SIZE)) == packetSize)
    {
#ifdef DEBUG
        Serial.printf("Client receives %dbytes.\r\n", packetSize);
        showArray(buf, packetSize);
#endif
        if (client_2pap.handleIncomingPacket(buf, packetSize))
        {
            Serial.println("Authentication successful");
            authenticationOkAction();
            powerOff();
        }
    }
    else
    {
        Serial.println("invalid message");
    }
}

void loop()
{
    if (timerStartAuthentication.isExpired())
    {
        powerOff();
    }
    onReceive(LoRa.parsePacket());
}

#endif