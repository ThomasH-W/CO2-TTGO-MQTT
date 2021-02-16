/*
  myXmodule.h - Library for mqtt client

  inject two lines in main.cpp
    #include <myXmodule.h>
    myXmodule myXmodule1;

*/
#ifndef MYONEWIRE_h
#define MYONEWIRE_h

#include "Arduino.h"

// includes for external libraries
// #include <OneWire.h>

// define for items which could close a global config , e.g. PINs
// #define DS18B20_PIN 18 // GPIO-Pin für Daten des DS18B20

#define LOOP_SECONDS_XXX 10
#define SCAN_SECONDS_XXX 30

class myXmodule // define class
{
public:
  myXmodule(void); // constructor, which is used to create an instance of the class
  bool begin(void);
  void loop(void);
  int value(void);
  void status(char *value);
  int version(void);

private:
  unsigned long currentMillisLoop = 0, previousMillis = 0, previousMillisScan = 0;
  unsigned long intervalLoop = LOOP_SECONDS_XXX * 1000;
  unsigned long intervalScanDevice = SCAN_SECONDS_XXX * 1000;
  float temp_ds; //Temperatur des DS18B20
  char statusChar[100];
  bool sensorFound;
};

#endif
