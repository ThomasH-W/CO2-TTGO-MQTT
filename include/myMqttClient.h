/*
  mqtt.h - Library for mqtt client
   CMD TEST
    topic  cmnd/TTGO
    payload { "name":"test", "value":123}

Begin
  MQTT Server
  MQTT Name

Methoden
  publishTele
  publishStat


*/
#ifndef MqttClient_h
#define MqttClient_h

#include "Arduino.h"

#include <WiFi.h>
#include <PubSubClient.h>

#include <ArduinoJson.h>

#include "user_config.h"
//#define MQTT_PORT 1883
//#define MQTT_DEVICENAME "TTGO-1"

struct json_cmnd_struct
{
  int count;
  char name[100];
  int value;
};

class MqttClient2 // define class
{
public:
  MqttClient2(void); // constructor, which is used to create an instance of the class
  void begin(const char *hostName);
  void begin(const char *hostName, int portNo);
  void loop(void);
  void publishCO2(const char payload[100]);
  void publishTemp(const char payload[100]);
  void publishHumi(const char payload[100]);
  void publishTele(const char topicItem[10], const char payload[100]);
  void publishStat(const char topicItem[10], const char payload[100]);
  void setDeviceName(const char deviceName[10]);
  int version(void);
  void classInterruptHandler(void);
  void callback(char *topic, byte *message, unsigned int length); // must be public
  json_cmnd_struct command(void);
  json_cmnd_struct json_cmnd;

private:
  void (*localPointerToCallback)(const int);
  void reconnect(void);
  void subscribe(void);
  char _deviceName[10];
  char _topic[100];
  char tempString[10];
};

#endif
