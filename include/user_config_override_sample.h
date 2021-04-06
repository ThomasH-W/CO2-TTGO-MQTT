/*
  user_config_override_sample.h
*/
#ifndef _USER_CONFIG_OVERRIDE_H_
#define _USER_CONFIG_OVERRIDE_H_

// -- Setup your own Wifi settings  ---------------
#undef STA_SSID1
#define STA_SSID1 "YourSSID" // [Ssid1] Wifi SSID
#undef STA_PASS1
#define STA_PASS1 "YourWifiPassword" // [Password1] Wifi password

// -- Setup your own MQTT settings  ---------------
#undef MQTT_HOST
#define MQTT_HOST "your-mqtt-server.com" // [MqttHost]

#undef MQTT_PORT
#define MQTT_PORT 1883 // [MqttPort] MQTT port (10123 on CloudMQTT)

#undef MQTT_USER
#define MQTT_USER "YourMqttUser" // [MqttUser] Optional user

#undef MQTT_PASS
#define MQTT_PASS "YourMqttPass" // [MqttPassword] Optional password

#undef MQTT_DEVICENAME
#define MQTT_DEVICENAME "TTGO"

#endif
