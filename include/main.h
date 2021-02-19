/*
  main.h - Library for mqtt client
*/
#ifndef MAIN_h
#define MAIN_h

// https://github.com/Locoduino/RingBuffer

#include "Arduino.h"
#include "user_config.h"

#define ADC_EN 14 //ADC_EN is the ADC detection enable port
#define ADC_PIN 34
#define BUTTON_1 35
#define BUTTON_2 0
#define BUTTON_A_PIN 35
#define BUTTON_B_PIN 0

#define LOOP_SECONDS_DATA 20

enum States
{
  ST_BOOT,        // 0 pure text
  ST_GUI_1,       // 1 Time/WLAN -- CO2 -- Temp/Humi
  ST_GUI_2,       // 2 CO2 -- Temp
  ST_GUI_3,       // 3 CO2 Text w/ color
  ST_GUI_4,       // 4 CO2 Gauge
  ST_GUI_5,       // 5 Admin: WiFi, Sensor Version, Sensor ID
  ST_GUI_6,       // 6 Trend Graph
  ST_CALIBRATION, // 6 Calibration incl. Count Down
};
#define ST_MAX ST_CALIBRATION // roll-over when switching through modes

struct sensor_data_struct
{
  char tempChar[20];
  char humiChar[20];
  char co2Char[20];
  char co2simChar[20];
  char timeOfDayChar[20];
  char dateChar[20];
  float temperature = 0;
  float humidity = 0;
  int co2_ppm = 0, co2_ppm_last = 0;
  int co2sim = 0;
  char rssiChar[20];    // wifi signal strength
  int rssiLevel = 0;    // wifi signal strength
  char ssidChar[20];    // SSID - wlan name
  char IPChar[20];      // SSID - wlan name
  char co2FWChar[20];   // CO2 firmware
  char co2AutoChar[20]; // CO2 firmware
  char owIDChar[20];    // 1 Wire ID
};

void setup_wifi(void);
void displayReset(void);
void displayDebugPrint(const char *message);
void displayDebugPrintln(const char *message);

#endif
