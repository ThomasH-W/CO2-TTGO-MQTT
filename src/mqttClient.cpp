/*
 CMD TEST
 topic  cmnd/TTGO
 payload { "name":"test", "value":123}
*/
#include "Arduino.h"
#include "myMqttClient.h"

// Outside of class
MqttClient2 *pointerToMqttClass; // declare a pointer to testLib class

WiFiClient espClient;
PubSubClient client(espClient);

// define global handler outsdie of class
static void outsideInterruptHandler(char *topic, byte *message, unsigned int length)
{
  pointerToMqttClass->callback(topic, message, length); // calls class member handler
} // end of function

// ------------------------------------------------------------------------------------------------------------------------
// Class members
// constructor, which is used to create an instance of the class
MqttClient2::MqttClient2(void)
{
  pointerToMqttClass = this;                   // assign current instance to pointer (IMPORTANT!!!)
  client.setCallback(outsideInterruptHandler); // set callback for subscribe
} // end of function

// ------------------------------------------------------------------------------------------------------------------------
void MqttClient2::begin(const char *hostName)
{
  begin(hostName, MQTT_PORT); // use default port
} // end of function

// ------------------------------------------------------------------------------------------------------------------------
void MqttClient2::begin(const char *hostName, int portNo)
{
  client.setServer(hostName, portNo); // define server
  setDeviceName(MQTT_DEVICENAME);     // set default device name
} // end of function

// ------------------------------------------------------------------------------------------------------------------------
// set internal device name
void MqttClient2::setDeviceName(const char deviceName[10])
{
  strlcpy(_deviceName, // <- destination
          deviceName,  // <- source
          sizeof(_deviceName));
} // end of function

// ------------------------------------------------------------------------------------------------------------------------
// try to reconnect, if connection drops
// grant some process time to mqtt library for callback / incoming messages
void MqttClient2::loop(void)
{
  if (!client.connected())
  {
    reconnect();
  }
  client.loop();
} // end of function

// ------------------------------------------------------------------------------------------------------------------------
void MqttClient2::publishStat(const char topicItem[10], const char payload[100])
{
  // build topic: stat/TTGO/[topicItem]
  char _topic[100];
  strcpy(_topic, "stat/"); //_deviceName
  strcat(_topic, _deviceName);
  strcat(_topic, "/");
  strcat(_topic, topicItem);

  client.publish(_topic, payload);
} // end of function

// ------------------------------------------------------------------------------------------------------------------------
void MqttClient2::publishTele(const char topicItem[10], const char payload[100])
{
  // build topic: tele/TTGO/[topicItem]
  char _topic[100];
  strcpy(_topic, "tele/"); //_deviceName
  strcat(_topic, _deviceName);
  strcat(_topic, "/");
  strcat(_topic, topicItem);

  client.publish(_topic, payload);
} // end of function

// ------------------------------------------------------------------------------------------------------------------------
void MqttClient2::publishTemp(const char payload[100])
{
  publishTele("temp", payload);
  publishTele("info", "Temperature published");
} // end of function

// ------------------------------------------------------------------------------------------------------------------------
void MqttClient2::publishHumi(const char payload[100])
{
  publishTele("humi", payload);
  publishTele("info", "Humidity published");
} // end of function

// ------------------------------------------------------------------------------------------------------------------------
void MqttClient2::publishCO2(const char payload[100])
{
  publishTele("CO2", payload);
  publishTele("info", "CO2 published");
} // end of function

// ------------------------------------------------------------------------------------------------------------------------
void MqttClient2::subscribe(void)
{
  // build topic: tele/TTGO/info
  char _topic[100];
  strcpy(_topic, "cmnd/"); //_deviceName
  strcat(_topic, _deviceName);
  strcat(_topic, "/#");

  client.subscribe(_topic);
} // end of function

// ------------------------------------------------------------------------------------------------------------------------
void MqttClient2::callback(char *topic, byte *payload, unsigned int length)
{
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;

  for (int i = 0; i < length; i++)
  {
    Serial.print((char)payload[i]);
    messageTemp += (char)payload[i];
  }
  Serial.println();

  // Deserialize the JSON document
  StaticJsonDocument<256> doc;
  DeserializationError error = deserializeJson(doc, payload, length);

  // Test if parsing succeeds.
  if (error)
  {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
  }
  else
  {

    // { "name":"test", "value":123}

    strlcpy(json_cmnd.name,          // <- destination
            doc["name"] | "unknown", // <- source
            sizeof(json_cmnd.name));
    json_cmnd.value = doc["value"];

    json_cmnd.count++;
    /*
    Serial.print("LIB  json # ");
    Serial.print(json_cmnd.count);
    Serial.print(" > ");
    Serial.print(json_cmnd.name);
    Serial.print(" : ");
    Serial.print(json_cmnd.value);
    Serial.print(" < ");
        Serial.println(error.f_str());
*/
  }

} // end of function

// ------------------------------------------------------------------------------------------------------------------------
json_cmnd_struct MqttClient2::command(void)
{
  return json_cmnd;
} // end of function

// ------------------------------------------------------------------------------------------------------------------------
void MqttClient2::reconnect(void)
{
  // Loop until we're reconnected
  while (!client.connected())
  {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    String clientId = "ESP32CO2-";
    clientId += String(random(0xffff), HEX);
    // if (client.connect(clientId.c_str(), mqtt_user,mqtt_password))
    if (client.connect(clientId.c_str()))
    {
      Serial.println("connected");
      subscribe();
      publishTele("info", "Connected");
      publishStat("result", "");
    }
    else
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
} // end of function

// ------------------------------------------------------------------------------------------------------------------------
/*
  version() returns the version of the library:
*/
int MqttClient2::version(void)
{
  return 1;
} // end of function
