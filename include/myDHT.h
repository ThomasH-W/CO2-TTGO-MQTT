/*
  myXmodule.h - Library for mqtt client

  inject two lines in main.cpp
    #include <myXmodule.h>
    myXmodule myXmodule1;

*/
#ifndef MYDHT_h
#define MYDHT_h

#include "DHT.h"
#define DHTPIN 25 // Digital pin connected to the DHT sensor

// Uncomment whatever type you're using!
//#define DHTTYPE DHT11   // DHT 11
#define DHTTYPE DHT22 // DHT 22  (AM2302), AM2321
//#define DHTTYPE DHT21   // DHT 21 (AM2301

// Connect pin 1 (on the left) of the sensor to +5V
// NOTE: If using a board with 3.3V logic like an Arduino Due connect pin 1
// to 3.3V instead of 5V!
// Connect pin 2 of the sensor to whatever your DHTPIN is
// Connect pin 4 (on the right) of the sensor to GROUND
// Connect a 10K resistor from pin 2 (data) to pin 1 (power) of the sensor

#define LOOP_SECONDS_DHT 10
#define SCAN_SECONDS_DHT 30

class myDHT // define class
{
public:
  myDHT(void); // constructor, which is used to create an instance of the class
  bool begin(void);
  void loop(float *temp, char *cValueHumi, float *humi, char *cValueTemp);
  float valueTemp(void);
  float valueHumi(void);
  void status(char *value);
  int version(void);

private:
  unsigned long currentMillisLoop = 0, previousMillis = 0, previousMillisScan = 0;
  unsigned long intervalLoop = LOOP_SECONDS_DHT * 1000;
  unsigned long intervalScanDevice = SCAN_SECONDS_DHT * 1000;
  float temp_dht; //Temperatur des DHT11
  float humi_dht; //Feuchtigkeit des DHT11
  char statusChar[100];
  bool sensorFound;
};

#endif
