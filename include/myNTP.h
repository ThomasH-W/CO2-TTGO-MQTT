/*
  myXmodule.h - Library for mqtt client

  inject two lines in main.cpp
    #include <myXmodule.h>
    myXmodule myXmodule1;

*/
#ifndef MYNTP_h
#define MYNTP_h

#include "Arduino.h"

/* Configuration of NTP */
#define MY_NTP_SERVER "de.pool.ntp.org"
#define MY_TZ "CET-1CEST,M3.5.0/02,M10.5.0/03"

// define for items which could close a global config , e.g. PINs
// #define DS18B20_PIN 18 // GPIO-Pin für Daten des DS18B20

#define LOOP_SECONDS_NTP 10
#define SCAN_SECONDS_NTP 30


class myNTP // define class
{

public:
  myNTP(void); // constructor, which is used to create an instance of the class
  bool begin(void);
  void loop(char *timeOfDayChar,char *dateChar);
  void value(char *bufTime, char *bufDate);
  char *localTime();
  void status(char *value);
  int version(void);

private:
  struct tm tm;
  unsigned long currentMillisLoop = 0, previousMillis = 0, previousMillisScan = 0;
  unsigned long intervalLoop = LOOP_SECONDS_NTP * 1000;
  unsigned long intervalScanDevice = SCAN_SECONDS_NTP * 1000;
  float temp_ds; //Temperatur des DS18B20
  char statusChar[100];
  bool sensorFound;
};

#endif
