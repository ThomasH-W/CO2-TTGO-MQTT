/*
https://github.com/WifWaf/MH-Z19/blob/master/examples/BasicUsage/BasicUsage.ino
https://unsinnsbasis.de/co2-sensor-mhz19b/
*/
#include "Arduino.h"
#include "myOneWire.h"

OneWire ow(DS18B20_PIN);
DallasTemperature ds18b20(&ow);
DeviceAddress ds18b20_id; // ID des DS18B20 (Adresse)

// ------------------------------------------------------------------------------------------------------------------------
// Class members
// constructor, which is used to create an instance of the class
myOneWire::myOneWire(void)
{
  sensorFound = false;
  temp_ds = 0;
  intervalLoop = LOOP_SECONDS * 1000;
  intervalScanDevice = SCAN_SECONDS * 1000;
  strcpy(statusChar, "myOneWire classs constructor");
  // return error;
} // end of function

// ------------------------------------------------------------------------------------------------------------------------
bool myOneWire::begin(void)
{
  int i;
  bool error = false;

  // Sensoren initialisieren
  // ROM-Adresse (ID) des DS18B20 ermitteln und anzeigen
  if (!ow.search(ds18b20_id))
  {
    strcpy(statusChar, "ERR DS18B20-Sensor nicht gefunden");
    Serial.println(statusChar);
    sensorFound = false;
    error = true;
  }
  else
  {
    strcpy(statusChar, "DS18B20-Sensor gefunden");
    sensorFound = true;
    Serial.print("DS18B20 ID (Sensor-Adresse):");
    // Adress-Bytes als Hexadezimalwerte ausgeben
    for (i = 0; i < sizeof(DeviceAddress); i++)
    {
      Serial.print(' ');
      Serial.print(ds18b20_id[i], HEX);
    }
    Serial.print(' ');
  }

  return error;
} // end of function

// ------------------------------------------------------------------------------------------------------------------------
void myOneWire::loop(void)
{
  currentMillisLoop = millis();
  if (currentMillisLoop - previousMillis > intervalLoop)
  {
    // Serial.print("t");
    if (sensorFound == true)
    {
      ds18b20.requestTemperatures(); // DS18B20-Abfrage starten
      temp_ds = ds18b20.getTempC(ds18b20_id);
      strcpy(statusChar, "DS18B20-Sensor gelesen");
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
int myOneWire::value(void)
{
  return temp_ds;
} // end of function

// ------------------------------------------------------------------------------------------------------------------------
void myOneWire::status(char *value)
{
  strcpy(value, statusChar);
} // end of function

// ------------------------------------------------------------------------------------------------------------------------
/*
  version() returns the version of the library:
*/
int myOneWire::version(void)
{
  return 1;
} // end of function
