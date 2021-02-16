/*
  main.cpp
    Date  2021-02-07
    Version 0.1

  ToDo
    - calibration
    - display 1: only CO2
    - display 2: CO2 + temp
    - indicator: WLAN, heartbeat
    - wlan-reconnect ?
*/

#include "main.h"

#include <WiFi.h>
#include <Wire.h>

#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <AsyncElegantOTA.h>
AsyncWebServer server(80);

#include <ArduinoJson.h>

#include <WiFiMulti.h>
WiFiMulti wifiMulti;

#include "myCO2.h"
myCO2 myCO2Sensor;

#include "myDHT.h"
myDHT myDHT22;

#include "myNTP.h"
myNTP myNtp;
char timeOfDayChar[20];
char dateChar[20];
unsigned long lastNtp=0;


#include "myMqttClient.h"
    MqttClient2 myMqttClient2;
int jsonCountOld = 0;

#include "myDisplay.h"
myDisplay myDisplay1;
char statusChar[50];

#include "Button2.h"
Button2 buttonA = Button2(BUTTON_A_PIN);
Button2 buttonB = Button2(BUTTON_B_PIN);

long lastMsg = 0;
long lastMsgCo2 = 0;
char msg[100];
int value = 0;

float temperature = 0;
char tempChar[20];
float humidity = 0;
char humiChar[20];
int co2_ppm = 0, co2_ppm_last = 0;
char co2Char[20];
int co2sim = 0;
char co2simChar[20];

// x,y == coords of centre of arc
// start_angle = 0 - 359
// seg_count = number of 6 degree segments to draw (60 => 360 degree arc)
// rx = x axis outer radius
// ry = y axis outer radius
// w  = width (thickness) of arc in pixels
// colour = 16 bit colour value
// Note if rx and ry are the same then an arc of a circle is drawn
// fillArc(160, 120, inc * 6, 1, 140, 100, 10, rainbow(col));
int arc_x = 120, arc_y = 65, arc_sa = 45, arc_sc = 30, arc_rx = 50, arc_ry = 30, arc_w = 10;
unsigned int arc_colour = TFT_BLUE;

bool mqttUpdate = false;
bool displayUpdate = false;

int mode = 0, oldMode = 0; // based on state
unsigned long previousMillis = 0, previousMillisRssi = 0, intervalRssiLoop = 10000;
const int RSSI_MAX = -50;  // define maximum strength of signal in dBm
const int RSSI_MIN = -100; // define minimum strength of signal in dBm

void setup_wifi(void);
void displayReset(void);
void displayDebugPrint(const char *message);
void displayDebugPrintln(const char *message);
void click(Button2 &btn);

// ----------------------------------------------------------------------------------------
void setup()
{
  Serial.begin(115200);
  delay(50);
  Serial.println("---------------------------");

  myDisplay1.begin();
  delay(10);
  myDisplay1.println("> Boot ...");

  myDisplay1.println("> DHT");
  myDHT22.begin();
  myDHT22.status(statusChar);
  myDisplay1.println(statusChar);

  myDisplay1.println("> CO2 Sensor");
  myCO2Sensor.begin();
  myCO2Sensor.status(statusChar);
  myDisplay1.println(statusChar);

  myDisplay1.print("> WiFi ");
  delay(10);
  setup_wifi();

  myMqttClient2.begin("192.168.178.20", 1883);
  myMqttClient2.setDeviceName("TTGO");

  buttonA.setClickHandler(click);
  buttonB.setClickHandler(click);

  myDisplay1.print("> NTP ");
  myNtp.begin();

  myDisplay1.println("> setup complete");
  Serial.println("> setup complete.");
  delay(500);
  mode = ST_GUI_1;
  displayReset();
} // end of function

// ----------------------------------------------------------------------------------------
void setup_wifi()
{
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("setup_wifi> Connecting to ");
  Serial.println(STA_SSID1);

  wifiMulti.addAP(STA_SSID1, STA_PASS1);
  while (wifiMulti.run() != WL_CONNECTED)
  {
    Serial.print(".");
    myDisplay1.print(".");
    delay(500);
  }

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/plain", "Hi! I am ESP32.");
  });

  AsyncElegantOTA.begin(&server); // Start ElegantOTA
  server.begin();
  Serial.println("HTTP server started");

  Serial.println("");
  Serial.print("WiFi connected - IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println("> setup_wifi complete.");
  myDisplay1.println(" connected");
} // end of function

// ----------------------------------------------------------------------------------------
void mqttcmd_read(char name[], int value)
{

  if (!strcmp(name, "arc_x"))
  {
    arc_x = value;
    Serial.print("arc_x : ");
    Serial.println(value);
  }

  if (!strcmp(name, "arc_y"))
  {
    arc_y = value;
    Serial.print("arc_y : ");
    Serial.println(value);
  }

  if (!strcmp(name, "arc_sa"))
    arc_sa = value;
  if (!strcmp(name, "arc_sc"))
    arc_sc = value;

  if (!strcmp(name, "arc_rx"))
    arc_rx = value;
  if (!strcmp(name, "arc_ry"))
    arc_ry = value;

  if (!strcmp(name, "arc_w"))
    arc_w = value;
  if (!strcmp(name, "arc_colour"))
    arc_colour = value;

  if (!strcmp(name, "mode"))
    mode = value;

  if (!strcmp(name, "update"))
    mqttUpdate = true;

  if (!strcmp(name, "co2sim"))
  {
    co2sim = value;
    dtostrf(co2sim, 1, 0, co2simChar);
  }

  if (!strcmp(name, "calibrate"))
  {
    myCO2Sensor.calibrateStart();
    mode = ST_CALIBRATION;
    mqttUpdate = true;
  }

  // displayDebugPrintln("mqttupdate");
  mqttUpdate = true;
  displayUpdate = true;
} // end of function

// ----------------------------------------------------------------------------------------
void mqttcmd_loop()
{
  json_cmnd_struct jsonCmnd = myMqttClient2.command();
  if (jsonCmnd.count != jsonCountOld)
  {
    jsonCountOld = jsonCmnd.count;
    Serial.print("MAIN json # ");
    Serial.print(jsonCmnd.count);
    Serial.print(" > ");
    Serial.print(jsonCmnd.name);
    Serial.print(" : ");
    Serial.print(jsonCmnd.value);
    Serial.println(" < ");
    mqttcmd_read(jsonCmnd.name, jsonCmnd.value);        // do something whith the cmnd
    myMqttClient2.publishStat("result", jsonCmnd.name); // moduel to proivde feedback than mqtt cmnd has been processed
    displayDebugPrint("mq> ");
    displayDebugPrint(jsonCmnd.name);
    displayDebugPrint(" / ");
    char PufferChar1[20];
    dtostrf(jsonCmnd.value, 1, 0, PufferChar1);
    displayDebugPrintln(PufferChar1);
  }

} // end of function

// ----------------------------------------------------------------------------------------
void displayDebugPrint(const char *message)
{
  if (1 > mode)
    myDisplay1.print(message);
  Serial.print(message);
} // end of function

// ----------------------------------------------------------------------------------------
void displayDebugPrintln(const char *message)
{
  if (1 > mode)
    myDisplay1.println(message);
  Serial.println(message);
} // end of function

// ----------------------------------------------------------------------------------------
void dht22_loop()
{
  long now = millis();
  if (now - lastMsg > 20000)
  {
    lastMsg = now;
    // temperature = myOneWireSensor.value();
    temperature = myDHT22.valueTemp();
    dtostrf(temperature, 4, 1, tempChar);

    myMqttClient2.publishTemp(tempChar);
    displayDebugPrint("DHT22 temp: ");
    displayDebugPrint(tempChar);

    // temperature = myOneWireSensor.value();
    humidity = myDHT22.valueHumi();
    dtostrf(humidity, 4, 1, humiChar);

    myMqttClient2.publishHumi(humiChar);
    displayDebugPrint(" / humi: ");
    displayDebugPrintln(humiChar);

    displayUpdate = true;
  }

} // end of function

// ----------------------------------------------------------------------------------------
void ntp_loop()
{
  long now = millis();
  if (now - lastNtp > 20000)
  {
    lastNtp = now;
    myNtp.value(timeOfDayChar, dateChar);
    displayDebugPrint(">ntp time: ");
    displayDebugPrint(timeOfDayChar);
    displayDebugPrint("  / date: ");
    displayDebugPrintln(dateChar);
  }
} // end of fuction

// ----------------------------------------------------------------------------------------
void co2_loop()
{
  long now = millis();
  if (now - lastMsgCo2 > 20000)
  {
    lastMsgCo2 = now;
    co2_ppm_last = co2_ppm;
    co2_ppm = myCO2Sensor.value();
    dtostrf(co2_ppm, 1, 0, co2Char);

    myMqttClient2.publishCO2(co2Char);
    displayDebugPrint("> CO2: ");
    displayDebugPrintln(co2Char);
    displayUpdate = true;
  }

} // end of function

// ---------------------------------------------------------------------------------------
void displayReset(void)
{
  myDisplay1.clear();
  mqttUpdate = true;
  displayUpdate = true;
} // end of function
// ---------------------------------------------------------------------------------------
void displayLoop(void)
{
  if (mode != oldMode)
  {
    myDisplay1.clear();
    oldMode = mode;
  }
  if (displayUpdate == true)
  {
    switch (mode)
    {
    case ST_GUI_1:                        // 3
      myDisplay1.Gui1(co2Char, tempChar); // text
      break;
    case ST_GUI_2: // 4
      myDisplay1.clear();
      myDisplay1.Gui2(0, 4000, co2_ppm); // gauge left/right
      break;
    case ST_GUI_3:                        // 5 boot mode
      myDisplay1.Gui3(co2Char, tempChar); // wifi signal
      break;
    case ST_GUI_4: // 6 boot mode
      myDisplay1.clear();
      myDisplay1.Gui5(arc_x, arc_y, arc_sa, arc_sc, arc_rx, arc_ry, arc_w, arc_colour); // gauge design test
      break;
    case ST_CALIBRATION: // 6 boot mode
      myDisplay1.clear();
      int caliTime = myCO2Sensor.calibrateTimer();
      Serial.print("caliTime: ");
      Serial.println(caliTime);
      myDisplay1.GuiCalibration(caliTime); // gauge design test
      break;                               //default:
      // myDisplay1.Text(co2simChar, tempChar);
    }
    mqttUpdate = false;
    displayUpdate = false;
  }

} // end of function

/*
 * Written by Ahmad Shamshiri
  * with lots of research, this sources was used:
 * https://support.randomsolutions.nl/827069-Best-dBm-Values-for-Wifi 
 * This is approximate percentage calculation of RSSI
 * WiFi Signal Strength Calculation
 * Written Aug 08, 2019 at 21:45 in Ajax, Ontario, Canada
 */

// ----------------------------------------------------------------------------------------
int dBmtoPercentage(int dBm)
{
  int quality;
  if (dBm <= RSSI_MIN)
  {
    quality = 0;
  }
  else if (dBm >= RSSI_MAX)
  {
    quality = 100;
  }
  else
  {
    quality = 2 * (dBm + 100);
  }

  return quality;
} // end of fuction dBmtoPercentage

// -----------------------------------------------------------------------------------------
void wifiRSSIloop()
{
  unsigned long currentMillisLoop = millis();

  if (currentMillisLoop - previousMillisRssi > intervalRssiLoop)
  {
    Serial.print(dBmtoPercentage(WiFi.RSSI())); //Signal strength in %
    Serial.print("% )");
    previousMillis = millis();
  } // end of timer

} // end of fuction

// ----------------------------------------------------------------------------------------
void click(Button2 &btn)
{
  if (btn == buttonA)
  {
    Serial.print("A clicked, ");
    mode++;
    if (mode > ST_MAX)
      mode = ST_BOOT;
    Serial.print("new mode: ");
    Serial.println(mode);
    mqttUpdate = true;
  }
  else if (btn == buttonB)
  {
    Serial.println("B clicked");
  }
} // end of fuction


  // ----------------------------------------------------------------------------------------
  void loop()
  {

    displayLoop();

    myMqttClient2.loop(); //client.loop(); + reconnect if required
    myCO2Sensor.loop();   // every 10sec read val, every 30s reconnect
    myDHT22.loop();       // every 10sec read val
    myNtp.loop();         // every 10sec read val
    // AsyncElegantOTA.loop();

    mqttcmd_loop(); // fetch +process data from mqtt client
    dht22_loop();   // every 20 sec: fetch +process data from 1 Wire sensor
    co2_loop();     // every 20 sec: fetch +process data from CO2 Wire sensor
    ntp_loop();     // every 20 sec: fetch +process data from CO2 Wire sensor

    buttonA.loop();
    buttonB.loop();

    yield();
  } // end of function
