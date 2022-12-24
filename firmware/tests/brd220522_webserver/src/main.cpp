/** \file brd220522_webserver
 * 
 * \author Christoph Tack
 * \date 2022-12-24
 * \brief Webserver for access control with pin code
 * \details When browsing to the webserver, pin_form.html is served to the client.  The client fills in the pin code and generates
 * a HTTP-POST message.  The server checks the pin code and send a dynamically generated webpage (door.html) back to the client.
 */

// Import required libraries
#include "WiFi.h"
#include "ESPAsyncWebServer.h"
#include "SPIFFS.h"
#include "wifi_credentials.h"

// Set LED GPIO
const int ledPin = 2;
// Stores LED state
String ledState;
bool isAccessGranted=false;

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

String processor(const String &var)
{
  Serial.println(var);
  if (var == "DOOR_STATE")
  {
    return isAccessGranted ? "open" : "blijft gesloten";
  }
  return String();
}

void action(AsyncWebServerRequest *request)
{
  int params = request->params();
  for (int i = 0; i < params; i++)
  {
    AsyncWebParameter *p = request->getParam(i);
    if (p->name() == "pincode" && p->value() == PIN_CODE)
    {
      isAccessGranted = true;
    }
  }
  request->send(SPIFFS, "/door.html", String(), false, processor);
}

void setup()
{
  // Serial port for debugging purposes
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);

  // Initialize SPIFFS
  if (!SPIFFS.begin(true))
  {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  // Configures static IP address
  IPAddress local_IP(192, 168, 1, 253); // Set your Static IP address
  IPAddress gateway(192, 168, 1, 1);    // Set your Gateway IP address
  IPAddress subnet(255, 255, 255, 0);
  if (!WiFi.config(local_IP, gateway, subnet))
  {
    Serial.println("STA Failed to configure");
  }
  // Connect to Wi-Fi
  WiFi.begin(SSID, PASSWORD);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  // Print ESP32 Local IP Address
  Serial.println(WiFi.localIP());

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, "/pin_form.html", String(), false); });

  server.on("/login", HTTP_POST, action);
  server.begin(); // Start server
}

void loop()
{
}
