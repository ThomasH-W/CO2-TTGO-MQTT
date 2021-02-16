/*
  main.h - Library for mqtt client
*/
#ifndef MAIN_h
#define MAIN_h

#include "Arduino.h"
#include "user_config.h"

#define ADC_EN 14 //ADC_EN is the ADC detection enable port
#define ADC_PIN 34
#define BUTTON_1 35
#define BUTTON_2 0
#define BUTTON_A_PIN 35
#define BUTTON_B_PIN 0

enum States
{
  ST_BOOT,            // 0 pure text
  ST_GUI_1,           // 1 Text
  ST_GUI_2,           // 2 CO2 Gauge
  ST_GUI_3,           // 3 CO2 + Temp Gauge
  ST_GUI_4,           // 4 Admin: WiFi, Sensor Version, Sensor ID
  ST_GUI_5,            // 5
  ST_CALIBRATION, // 6 Calibration incl. Count Down
};
#define ST_MAX ST_CALIBRATION

#endif
