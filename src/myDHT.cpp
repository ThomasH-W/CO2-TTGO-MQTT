/*
lib_deps =
    adafruit/Adafruit Unified Sensor 
    adafruit/DHT sensor library
*/
#include "Arduino.h"
#include "myDHT.h"

// Instance of external ibrary  object
// DHT dht(DHTPIN, DHTTYPE);
DHT22 dht;

myDHT *pointerToDhtClass; // declare a pointer to testLib class
// ------------------------------------------------------------------------------------------------------------------------
// Class members
// constructor, which is used to create an instance of the class
myDHT::myDHT(void)
{
  pointerToDhtClass = this; // assign current instance to pointer (IMPORTANT!!!)
  begin(); // you do not need begin() usually, but if you want too attach sensor during runtime, you should use begin
} // end of function

// ------------------------------------------------------------------------------------------------------------------------
bool myDHT::begin(void)
{
  // dht.begin();
  dht.setup(DHTPIN, RMT_CHANNEL_2);
  dht.setCallback([](int8_t result) {
    if (result > 0)
    {
      pointerToDhtClass->temp_dht = dht.getTemperature();
      pointerToDhtClass->humi_dht = dht.getHumidity();
      // Serial.printf("DHT22 callback> Temp: %.1f°C / Humid: %.1f%%\n", dht.getTemperature(), dht.getHumidity());
    }
    else
    {
      Serial.printf("DHT22 callback> Sensor error: %s\n", dht.getError());
    }
  });

  sensorFound = true;
  strcpy(statusChar, "DHT22-Sensor begin");
  return 0; // you may indicate success
} // end of function

// ------------------------------------------------------------------------------------------------------------------------
void myDHT::loop(void)
{
  currentMillisLoop = millis();
  if (currentMillisLoop - previousMillis > intervalLoop)
  {
    Serial.print("DHT-loop: ");
    dht.read();
 
    Serial.print("Humidity: ");
    Serial.print(humi_dht);
    Serial.print("%  Temperature: ");
    Serial.print(temp_dht);
    Serial.println("°C ");
    strcpy(statusChar, "DHT22-Sensor gelesen");
    // }

    previousMillis = millis();
  } // end of timer

  if (currentMillisLoop - previousMillisScan > intervalScanDevice)
  {
    // Serial.print("T");
    if (sensorFound == false)
    {
      begin();
    }
    previousMillisScan = millis();
  } // end of timer

} // end of function

// ------------------------------------------------------------------------------------------------------------------------
float myDHT::valueTemp(void)
{
  return temp_dht;
} // end of function

// ------------------------------------------------------------------------------------------------------------------------
float myDHT::valueHumi(void)
{
  return humi_dht;
} // end of function

// ------------------------------------------------------------------------------------------------------------------------
void myDHT::status(char *value)
{
  strcpy(value, statusChar);
} // end of function

// ------------------------------------------------------------------------------------------------------------------------
/*
  version() returns the version of the library:
*/
int myDHT::version(void)
{
  return 1;
} // end of function
