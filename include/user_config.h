/*
  user_config.h
*/
#ifndef _USER_CONFIG_H_
#define _USER_CONFIG_H_

// -- Setup your own Wifi settings  ---------------
#undef STA_SSID1
#define STA_SSID1 "" // [Ssid1] Wifi SSID
#undef STA_PASS1
#define STA_PASS1 "" // [Password1] Wifi password

// -- Setup your own MQTT settings  ---------------
#undef MQTT_HOST
#define MQTT_HOST "" // [MqttHost]

#undef MQTT_PORT
#define MQTT_PORT 1883 // [MqttPort] MQTT port (10123 on CloudMQTT)

#undef MQTT_USER
#define MQTT_USER "" // [MqttUser] Optional user

#undef MQTT_PASS
#define MQTT_PASS "" // [MqttPassword] Optional password

#undef MQTT_DEVICENAME
#define MQTT_DEVICENAME "TTGO_1234"

#ifdef USE_CONFIG_OVERRIDE
  #include "user_config_override.h" // Configuration overrides for my_user_config.h
#endif

#endif
