#include <ESP8266WiFi.h>

#include <SPI.h>
#include <PubSubClient.h>

// IPAddress server(192, 168, 1, 1);
// IPAddress server(73,109,80,198);
char server[] = MQTT_SERVER;

void callback(char *topic, byte *payload, unsigned int length)
{
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++)
  {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  String strTopic = (String)topic;

  char charPayload[length];
  int i = 0;
  for (i = 0; i < length; i++)
  {
    charPayload[i] = (char)payload[i];
  }
  charPayload[i] = '\0';
  String strPayload = String(charPayload);

  if (strTopic.equals((String)MQTT_TOPIC + "/" + MQTT_POWER_SUBTOPIC + "/" + MQTT_COMMAND_SUBTOPIC))
  {
    Serial.println("Matches power subtopic");
    if (strPayload.equals("OFF"))
    {
      Serial.println("Lights off");
      setLedsState(false);
    }
    else if (strPayload.equals("ON"))
    {
      Serial.println("Lights on");
      setLedsState(true);
    }
  }
  else if (strTopic.equals((String)MQTT_TOPIC + "/" + MQTT_RGB_SUBTOPIC + "/" + MQTT_COMMAND_SUBTOPIC))
  {
    Serial.println("Matches RGB subtopic");
    uint8_t firstIndex = strPayload.indexOf(',');
    uint8_t lastIndex = strPayload.lastIndexOf(',');
    int R = strPayload.substring(0, firstIndex).toInt();
    int G = strPayload.substring(firstIndex + 1, lastIndex).toInt();
    int B = strPayload.substring(lastIndex + 1).toInt();
    fadeToColor(R, G, B);
  }
}

WiFiClient espClient;
PubSubClient client(espClient);

void reconnect()
{
  Serial.print("MQTT: ");
  if (client.connect("arduinoClient", "homeassistant", "password1"))
  {
    Serial.println("connected");
    // ... and resubscribe
    String str = MQTT_TOPIC + (String) "/#";
    if (client.subscribe(str.c_str()))
    {
      Serial.println("Subscribed!");
    }
  }
  else
  {
    Serial.print("failed, rc=");
    Serial.println(client.state());
  }
}

void mqttSetup()
{
  client.setServer(server, 1883);
  client.setCallback(callback);
}

void mqttLoop()
{
  if (!client.connected())
  {
    reconnect();
  }
  client.loop();
}