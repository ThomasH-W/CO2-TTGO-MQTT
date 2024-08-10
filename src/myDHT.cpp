/*
lib_deps =
    adafruit/Adafruit Unified Sensor 
    adafruit/DHT sensor library
*/
#include "Arduino.h"
#include "myDHT.h"

// Instance of external ibrary  object
DHT dht;

// ------------------------------------------------------------------------------------------------------------------------
// Class members
// constructor, which is used to create an instance of the class
myDHT::myDHT(void)
{
  begin();
  strcpy(statusChar, "myDHT classs constructor");
} // end of function

// ------------------------------------------------------------------------------------------------------------------------
bool myDHT::begin(void)
{
  dht.setup(DHTPIN); // data pin 2
  sensorFound = true;
  return sensorFound; // you may indicate success
} // end of function

// ------------------------------------------------------------------------------------------------------------------------
void myDHT::loop(float *temp, char *cValueTemp, float *humi, char *cValueHumi)
{
  currentMillisLoop = millis();
  if (currentMillisLoop - previousMillis > intervalLoop)
  {
    Serial.print("DHT-loop: ");
    if (sensorFound == true)
    {
      humi_dht = dht.getHumidity();
      temp_dht = dht.getTemperature();

      Serial.print("Humidity: ");
      Serial.print(humi_dht);
      Serial.print("%  Temperature: ");
      Serial.print(temp_dht);
      Serial.println("°C ");
      *humi = humi_dht;
      dtostrf(humi_dht, 2, 0, cValueHumi); // 5 digits, no decimal
      *temp = temp_dht;
      dtostrf(temp_dht, 5, 1, cValueTemp); // 5 digits, no decimal
      strcpy(statusChar, "DHT22-Sensor gelesen");
      // }
    } else
    {
      humi_dht = 0;
      strcpy(cValueHumi, "----");
      temp_dht = 0;
      strcpy(cValueTemp, "----");
      Serial.println("no DHT sensor ");
      strcpy(statusChar, "ERR DHT22-Sensor not found");
    }
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
