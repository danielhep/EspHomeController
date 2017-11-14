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

#ifdef MAGICHOME_LED_CONTROLLER
  handleLEDpayload(strTopic, strPayload);
#endif
#ifdef SERVO_LOCK_CONTROL
  handleServoPayload(strTopic, strPayload);
#endif
}

#ifdef MAGICHOME_LED_CONTROLLER
void handleLEDpayload(String topic, String payload)
{
  if (topic.equals((String)MQTT_TOPIC + "/" + MQTT_POWER_SUBTOPIC + "/" + MQTT_COMMAND_SUBTOPIC))
  {
    Serial.println("Matches power subtopic");
    if (payload.equals("OFF"))
    {
      Serial.println("Lights off");
      setLedsState(false);
    }
    else if (payload.equals("ON"))
    {
      Serial.println("Lights on");
      setLedsState(true);
    }
  }
  else if (topic.equals((String)MQTT_TOPIC + "/" + MQTT_RGB_SUBTOPIC + "/" + MQTT_COMMAND_SUBTOPIC))
  {
    Serial.println("Matches RGB subtopic");
    uint8_t firstIndex = payload.indexOf(',');
    uint8_t lastIndex = payload.lastIndexOf(',');
    int R = payload.substring(0, firstIndex).toInt();
    int G = payload.substring(firstIndex + 1, lastIndex).toInt();
    int B = payload.substring(lastIndex + 1).toInt();
    fadeToColor(R, G, B);
  }
}
#endif

#ifdef SERVO_MOTOR_CONTROL
void handleServoPayload(String topic, String payload)
{
  if (topic.equals((String)MQTT_TOPIC + "/" + MQTT_POWER_SUBTOPIC + "/" + MQTT_COMMAND_SUBTOPIC))
  {
  }
}
#endif

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