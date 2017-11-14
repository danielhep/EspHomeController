#include <config/all.h>

#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <NtpClientLib.h>

#include <ESP8266WebServer.h> //Local WebServer used to serve the configuration portal
#include <WiFiManager.h>      //https://github.com/tzapu/WiFiManager WiFi Configuration Magic

void setup()
{
  Serial.begin(115200);

  Serial.println(WiFi.macAddress());
  Serial.println("Booting");

  WiFiManager wifiManager;
  // wifiManager.resetSettings();
  wifiManager.autoConnect("AutoConnectAP");

  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // No authentication by default
  // ArduinoOTA.setPassword("admin");

  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH)
      type = "sketch";
    else // U_SPIFFS
      type = "filesystem";

    // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
    Serial.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR)
      Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR)
      Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR)
      Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR)
      Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR)
      Serial.println("End Failed");
  });
  ArduinoOTA.begin();
  Serial.println("Ready");

  mqttSetup();
  ledSetup();

  Serial.println("Ending setup");
}

void loop()
{
  // unsigned long time_spent_putzing_around_ms = 0;
  // make note of current time:
  // unsigned long start_time = millis();
  // ledLoop();

  ArduinoOTA.handle();
  mqttLoop();

  // unsigned long end_time = millis();

  // if (end_time > start_time)
  // {
  //   time_spent_putzing_around_ms = end_time - start_time;
  // }

  // // delay appropriate amount of time (or try to catch up!)
  // if (time_spent_putzing_around_ms < getLoopDelay())
  // {
  //   delay(getLoopDelay() - time_spent_putzing_around_ms);
  // }
}
