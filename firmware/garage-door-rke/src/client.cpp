#include "constants.h"
#include "pins.h"
#include "secretKey.h"

static const char* TAG = "Client";

#if REMOTE == 1
#include "KryptoknightClient.h"
#include "esp_adc_cal.h"

static const ClientRecord myID = {0x002ba78c, SECRET_KEY};
static KryptoknightClient client_2pap(myID.client_id, (byte *)myID.shared_secret_key, SERVER_ID);
static AsyncDelay timerStartAuthentication;
static esp_adc_cal_characteristics_t adc_chars;
static const uint32_t MIN_BAT_VOLTAGE = 3600; //[mV]

static uint8_t buf[BUFF_SIZE];

void authenticationOkAction()
{
    // for (int i = 0; i < 3; i++)
    // {
    //     digitalWrite(pinRST_RADIO, LOW); // Light green LED && reset radio.
    //     delay(150);
    //     digitalWrite(pinRST_RADIO, HIGH);
    //     delay(150);
    // }
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

    LoRa.setTxPower(0);

    // Get ADC-calibration data
    ESP_ERROR_CHECK(esp_adc_cal_check_efuse(ESP_ADC_CAL_VAL_EFUSE_TP));
    esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_6, ADC_WIDTH_BIT_12, 0, &adc_chars);
    // Configure the ADC with the calibration data
    adc1_config_channel_atten(ADC1_CHANNEL_3, ADC_ATTEN_DB_6);
    // Give voltage some time to stabilize
    delay(25);
    // Do the ADC-measurement
    int voltage_sum = 0;

    // Average the voltage over a number of ADC-samples
    const int SAMPLE_COUNT = 10;
    for (int i = 0; i < SAMPLE_COUNT; i++)
    {
        if (esp_adc_cal_get_voltage(ADC_CHANNEL_3, &adc_chars, &voltage) == ESP_OK)
        {
            voltage_sum += voltage;
        }
    }
    double average = voltage_sum / SAMPLE_COUNT;

    int battery_voltage = average * (R5 + R6) / R5;
    ESP_LOGD(TAG, "sampled battery voltage = %dmV\n", battery_voltage);
    if (battery_voltage < MIN_BAT_VOLTAGE)
    {
        digitalWrite(pinRedLED, HIGH);
    }

    // Disable ADC to save some power
    digitalWrite(pinEn_ADC, LOW);

    // Send authentication request
    client_2pap.startAuthentication();
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
    ESP_LOGI(TAG, "Device sends %dbytes.\r\n", packetSize);
#if LOG_LOCAL_LEVEL == ESP_LOG_VERBOSE
    showArray(buf, packetSize);
#endif
        if (client_2pap.handleIncomingPacket(buf, packetSize))
        {
            ESP_LOGD(TAG, "Authentication successful");
            authenticationOkAction();
            powerOff();
        }
    }
    else
    {
        ESP_LOGE(TAG, "invalid message");
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