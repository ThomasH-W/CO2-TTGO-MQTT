/*
https://github.com/WifWaf/MH-Z19/blob/master/examples/BasicUsage/BasicUsage.ino
https://unsinnsbasis.de/co2-sensor-mhz19b/
*/
#include "Arduino.h"
#include "myXmodule.h"

// Instance of external ibrary  object
// TFT_eSPI myTFT = TFT_eSPI();

// ------------------------------------------------------------------------------------------------------------------------
// Class members
// constructor, which is used to create an instance of the class
myXmodule::myXmodule(void)
{
  // set PIN
  begin(); // you do not need begin() usually, but if you want too attach sensor during runtime, you should use begin
} // end of function

// ------------------------------------------------------------------------------------------------------------------------
bool myXmodule::begin(void)
{
  // check is sensor is present
  return 0; // you may indicate success
} // end of function

// ------------------------------------------------------------------------------------------------------------------------
void myXmodule::loop(void)
{
  currentMillisLoop = millis();
  if (currentMillisLoop - previousMillis > intervalLoop)
  {
    // Serial.print("t");
    if (sensorFound == true)
    {
      // do something
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
int myXmodule::value(void)
{
  return temp_ds;
} // end of function

// ------------------------------------------------------------------------------------------------------------------------
void myXmodule::status(char *value)
{
  strcpy(value, statusChar);
} // end of function

// ------------------------------------------------------------------------------------------------------------------------
/*
  version() returns the version of the library:
*/
int myXmodule::version(void)
{
  return 1;
} // end of function
