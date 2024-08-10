/*
  myOneWire.h - Library for mqtt client
  lib_deps =
    paulstoffregen/OneWire 
    milesburton/DallasTemperature
*/
#ifndef MYONEWIRE_h
#define MYONEWIRE_h

#include "Arduino.h"

#include <OneWire.h>
#include <DallasTemperature.h>

#define DS18B20_PIN 18 // GPIO-Pin für Daten des DS18B20
#define LOOP_SECONDS 10
#define SCAN_SECONDS 30

class myOneWire // define class
{
public:
  myOneWire(void); // constructor, which is used to create an instance of the class
  bool begin(void);
  void loop(void);
  int value(void);
  void status(char *value);
  int version(void);

private:
  unsigned long currentMillisLoop = 0, previousMillis = 0, intervalLoop, previousMillisScan = 0, intervalScanDevice;
  float temp_ds; //Temperatur des DS18B20
  char statusChar[100];
  bool sensorFound;
};

#endif
