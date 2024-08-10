/*
https://github.com/WifWaf/MH-Z19/blob/master/examples/BasicUsage/BasicUsage.ino
https://unsinnsbasis.de/co2-sensor-mhz19b/
*/
#include "Arduino.h"
#include "myCO2.h"

HardwareSerial SerialCO2(2);
// SoftwareSerial SerialCO2(RX2, TX2); // RX, TX

// ------------------------------------------------------------------------------------------------------------------------
// Class members
// constructor, which is used to create an instance of the class
myCO2::myCO2(void)
{
  sensorFound = false;
  co2_ser = 0;
  strcpy(statusChar, "myCO2 classs constructor");
} // end of function

// ------------------------------------------------------------------------------------------------------------------------
void myCO2::begin(char *cValue)
{
  SerialCO2.begin(MHZ19_BAUDRATE, MHZ19_PROTOCOL, RX2, TX2);
  // SerialCO2.begin(MHZ19_BAUDRATE);
  delay(100);
  mhz19b.begin(SerialCO2); // MH-Z19B-Sensor eine Schnittstelle zuweisen

  // ein paar Daten der Sensor-Konfiguration ausgeben
  mhz19b.getVersion(mhz19_version);
  delay(100);
  Serial.println("--------------------");
  if (mhz19_version[0] == '\0')
  {
    strcpy(statusChar, "ERR MH-Z19B nicht gefunden");
    sensorFound = false;
    Serial.println(statusChar);
  }
  else
  {
    sensorFound = true;
    strncpy(statusChar, mhz19_version, sizeof(mhz19_version));
    strncpy(cValue, mhz19_version, sizeof(mhz19_version));
    Serial.print("MH-Z19B Firmware Version: ");
    for (int i = 0; i < 4; i++)
    {
      Serial.print(mhz19_version[i]);
      if (i == 1)
        Serial.print(".");
    }
    Serial.print("\nMH-Z19B Messbereich: ");
    Serial.println(mhz19b.getRange());
    Serial.print("MH-Z19B Autokalibrierung (ABC): ");
    mhz19b.getABC() ? Serial.println("AN") : Serial.println("AUS");
  }
  Serial.println("--------------------");

} // end of function

// ------------------------------------------------------------------------------------------------------------------------
// void myCO2::loop(int *iValue, char *cValue)
void myCO2::loop(sensor_data_struct *sData)
{
  currentMillisLoop = millis();
  if (currentMillisLoop - previousMillis > intervalLoop)
  {
    Serial.print("CO2-Loop: ");
    if (sensorFound == true)
    {
      Serial.print(" read sensor ");
      if (mhz19bCalibration == false)
      {
        Serial.print(" CO2: ");
        
        co2_ser = mhz19b.getCO2();
        if (co2_ser > MHZ19_RANGE)
          co2_ser = 0;
        sData->co2_ppm = co2_ser;
        Serial.print(co2_ser);
        sData->myCo2Buffer.push(co2_ser);      // fill ring buffer
        dtostrf(co2_ser, 5, 0, sData->co2Char); // 5 digits, no decimal
        Serial.print(" = char: ");
        Serial.print(sData->co2Char);

        Serial.print(" / temp ");
        temp_mh = mhz19b.getTemperature();
        Serial.println(temp_mh);
        strcpy(statusChar, "CO2-Sensor gelesen");
      }
      else // calibration is running
      {
        co2_ser = 1;
        strcpy(sData->co2Char, "cali");
        Serial.print("calibration ending in ");
        calibrateTimer();
        Serial.println(statusChar);
      }
    }
    else
    {
      co2_ser = 0;
      strcpy(sData->co2Char, "----");
      Serial.println("no sensor ");
    }
    previousMillis = millis();
  }

  if (currentMillisLoop - previousMillisScan > intervalScanDevice)
  {
    if (sensorFound == false)
    {
      begin(sData->co2Char);
    }
    previousMillisScan = millis();
  } // end of timer

} // end of function

// ------------------------------------------------------------------------------------------------------------------------
void myCO2::status(char *value)
{
  strcpy(value, statusChar);
} // end of function

// ------------------------------------------------------------------------------------------------------------------------
int myCO2::value(void)
{
  return co2_ser;
} // end of function

// ------------------------------------------------------------------------------------------------------------------------
int myCO2::valueTemp()
{
  return temp_mh;
} // end of function

// ------------------------------------------------------------------------------------------------------------------------
void myCO2::calibrateStart(void)
{
  calibrateStartTime = millis();

  // DEBUG mhz19b.autoCalibration(false); // make sure auto calibration is off
  Serial.print("ABC Status: ");
  // DEBUG mhz19b.getABC() ? Serial.println("ON") : Serial.println("OFF"); // now print it's status

  Serial.println("Waiting 20 minutes to stabalise...");
  strcpy(statusChar, "CO2-Kalibrierung gestartet");
  mhz19bCalibration = true;
} // end of function

// ------------------------------------------------------------------------------------------------------------------------
long myCO2::calibrateTimer(void)
{
  unsigned long calibrateRunTime = millis() - calibrateStartTime;
  long calibrateRemainTime = calibrateInterval - calibrateRunTime;

  if (calibrateRemainTime > 0)
  {
    Serial.print("cal-Timer running ");
    calibrateRemainTime /= 1000;
    dtostrf(calibrateRemainTime, 1, 0, statusChar);
    Serial.println(statusChar);
  }
  else
  {
    Serial.println("cal-Timer done ");
    calibrateEnd();
    calibrateRemainTime = -1;
  }

  strcpy(statusChar, "CO2-Sensor gelesen");
  return (calibrateRemainTime);
} // end of function

// ------------------------------------------------------------------------------------------------------------------------
void myCO2::calibrateEnd(void)
{
  Serial.println("Calibrating..");
  // DEBUG mhz19b.calibrate(); // Take a reading which be used as the zero point for 400 ppm
  strcpy(statusChar, "CO2-Kalibrierung fertig");
  mhz19bCalibration = false;
} // end of function

// ------------------------------------------------------------------------------------------------------------------------
/*
  version() returns the version of the library:
*/
int myCO2::version(void)
{
  return 1;
} // end of function
