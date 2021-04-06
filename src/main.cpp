/*
  main.cpp
    Date  2021-02-16
    Version 0.2

  ToDo
    - calibration
    - admin data
    - co2 levels - color
*/

#include "main.h"

#include "myDHT.h"
myDHT myDHT22;

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

#include "myNTP.h"
myNTP myNtp;
unsigned long lastNtp = 0;

#include "myMqttClient.h"
MqttClient2 myMqttClient2;
int jsonCountOld = 0;

#include "myDisplay.h"
myDisplay myDisplay1;
char statusChar[50];

long lastMsg = 0;
long lastMsgCo2 = 0;
char msg[100];
int value = 0;
sensor_data_struct sensorData;

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

#include <AceButton.h>
using namespace ace_button;
const int BUTTON1_PIN = 0;
const int BUTTON2_PIN = 35;
AceButton button1(BUTTON1_PIN);
AceButton button2(BUTTON2_PIN);
void handleEvent(AceButton *, uint8_t, uint8_t);
const int LED_PIN = 2; // for ESP32

// --------------------------------------------------------------------------
// https://github.com/bxparks/AceButton
// https://github.com/bxparks/AceButton/blob/develop/examples/TwoButtonsUsingOneButtonConfig/TwoButtonsUsingOneButtonConfig.ino
void setup_button()
{
  // Buttons use the built-in pull up register.
  pinMode(BUTTON1_PIN, INPUT_PULLUP);
  pinMode(BUTTON2_PIN, INPUT_PULLUP);

  // Configure the ButtonConfig with the event handler, and enable all higher
  // level events.
  ButtonConfig *buttonConfig = ButtonConfig::getSystemButtonConfig();
  buttonConfig->setEventHandler(handleEvent);
  buttonConfig->setFeature(ButtonConfig::kFeatureClick);
  buttonConfig->setFeature(ButtonConfig::kFeatureDoubleClick);
  buttonConfig->setFeature(ButtonConfig::kFeatureLongPress);
  buttonConfig->setFeature(ButtonConfig::kFeatureRepeatPress);

  // Check if the button was pressed while booting
  if (button1.isPressedRaw())
  {
    Serial.println(F("setup(): button 1 was pressed while booting"));
  }
  if (button2.isPressedRaw())
  {
    Serial.println(F("setup(): button 2 was pressed while booting"));
  }

} // end of function

// ----------------------------------------------------------------------------------------
void switch_mode(int reqMode)
{
  if (reqMode > ST_GUI_6)
    reqMode = ST_GUI_1;
  else if (reqMode < ST_GUI_1)
    reqMode = ST_GUI_6;

  Serial.printf("switch_mode>  old=%d new=%d \n", mode, reqMode);
  mode = reqMode;
  displayUpdate = true;
}

// --------------------------------------------------------------------------
// The event handler for the button.
void handleEvent(AceButton *button, uint8_t eventType, uint8_t buttonState)
{
  int butPressed = button->getPin();

  // Print out a message for all events, for both buttons.
  Serial.print(F("handleEvent(): pin: "));
  Serial.print(butPressed);
  Serial.print(F("; eventType: "));
  Serial.print(eventType);
  Serial.print(F("; buttonState: "));
  Serial.println(buttonState);

  // Control the LED only for the Pressed and Released events of Button 1.
  // Notice that if the MCU is rebooted while the button is pressed down, no
  // event is triggered and the LED remains off.
  switch (eventType)
  {
  case AceButton::kEventReleased: // kEventPressed
    if (butPressed == BUTTON1_PIN)
    {
      Serial.printf("handleEvent> GUI for pin %d\n", butPressed);
      switch_mode(mode + 1);
    }
    else if (butPressed == BUTTON2_PIN)
    {
      Serial.printf("handleEvent> GUI for pin %d\n", butPressed);
      switch_mode(mode - 1);
    }
    break;
  default:
    Serial.printf("handleEvent> unknown for pin %d & %d\n", butPressed, eventType);
  }
} // end of function

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
  myCO2Sensor.begin(sensorData.co2FWChar);
  myCO2Sensor.status(statusChar);
  myDisplay1.println(statusChar);

  myDisplay1.print("> WiFi ");
  delay(10);
  setup_wifi();

  myMqttClient2.begin(MQTT_HOST, MQTT_PORT); // include/defined in user_config_override.h
  myMqttClient2.setDeviceName(MQTT_DEVICENAME);

  setup_button();

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

  Serial.print("> WiFi SSID: ");
  strncpy(sensorData.ssidChar, WiFi.SSID().c_str(), 15);
  Serial.println(sensorData.ssidChar);

  Serial.print("> WiFi IP address: ");
  strncpy(sensorData.IPChar, WiFi.localIP().toString().c_str(), 15);
  Serial.println(sensorData.IPChar);

  Serial.println("> setup_wifi complete.");
  myDisplay1.println(" connected");
} // end of function

// ----------------------------------------------------------------------------------------
// check which command has been received and assign value
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
    sensorData.co2sim = value;
    dtostrf(sensorData.co2sim, 1, 0, sensorData.co2simChar);
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
// retrieve last message for subscribed topics
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
    displayDebugPrint("mqtt> ");
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
  /*  ST_BOOT,        // 0 pure text
      ST_GUI_1,       // 1 Time/WLAN -- CO2 -- Temp/Humi
      ST_GUI_2,       // 2 CO2 -- Temp
      ST_GUI_3,       // 3 CO2 Text w/ color
      ST_GUI_4,       // 4 CO2 Gauge
      ST_GUI_5,       // 5 Admin: WiFi, Sensor Version, Sensor ID
      ST_CALIBRATION, // 6 Calibration incl. Count Down
*/
  if (mode != oldMode)
  {
    myDisplay1.clear();
    oldMode = mode;
    displayUpdate = true;
  }

  if (displayUpdate == true)
  {
    switch (mode)
    {
    case ST_GUI_1:                 // 3
      myDisplay1.Gui1(sensorData); // text
      break;
    case ST_GUI_2: // 4
      myDisplay1.clear();
      myDisplay1.Gui2(sensorData); // gauge left/right
      break;
    case ST_GUI_3:                 // 5 boot mode
      myDisplay1.Gui3(sensorData); // wifi signal
      break;
    case ST_GUI_4: // 6 boot mode
      myDisplay1.clear();
      myDisplay1.Gui4(0, 3000, sensorData.co2_ppm, sensorData);
      Serial.print("> GUI4: ");
      Serial.println(sensorData.co2_ppm);
      // myDisplay1.Gui7(arc_x, arc_y, arc_sa, arc_sc, arc_rx, arc_ry, arc_w, arc_colour); // gauge design test
      break;
    case ST_GUI_5:                 // 5 Admin
      myDisplay1.Gui5(sensorData); // wifi signal
      break;
    case ST_GUI_6:                 // 5 Trend Graph
      myDisplay1.Gui6(sensorData); // wifi signal
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
-30 dBm	Max achievable signal strength. The client can only be a few feet from the AP to achieve this.
-67 dBm	Minimum signal strength for applications that require very reliable, timely packet delivery.	VoIP/VoWiFi, streaming video
-70 dBm	Minimum signal strength for reliable packet delivery.	Email, web
-80 dBm	Minimum signal strength for basic connectivity. Packet delivery may be unreliable.	N/A
-90 dBm	Approaching or drowning in the noise floor. Any functionality is highly unlikely.
 */

// ----------------------------------------------------------------------------------------
int dBmtoPercentage(int dBm)
{
  Serial.print(" (");
  Serial.print(dBm);
  Serial.print(" db) ");
  int quality = 0;
  if (dBm > -90) // s/b be done differetnly with else / if
    quality = 1;
  if (dBm > -80)
    quality = 2;
  if (dBm > -70)
    quality = 3;
  if (dBm > -67)
    quality = 4;
  if (dBm > -30)
    quality = 5;

  return quality;
} // end of fuction dBmtoPercentage

// ----------------------------------------------------------------------------------------
void pullData_loop()
{
  long now = millis();
  if (now - lastMsgCo2 > LOOP_SECONDS_DATA * 1000)
  {

    myCO2Sensor.loop(&sensorData); // every 10sec read val, every 30s reconnect
    myMqttClient2.publishCO2(sensorData.co2Char);
    displayDebugPrint("> CO2: ");
    displayDebugPrintln(sensorData.co2Char);

    myDHT22.loop(&sensorData.temperature, sensorData.tempChar, &sensorData.humidity, sensorData.humiChar); // every 10sec read val
    myMqttClient2.publishTemp(sensorData.tempChar);
    displayDebugPrint("DHT22 temp: ");
    displayDebugPrint(sensorData.tempChar);

    myMqttClient2.publishHumi(sensorData.humiChar);
    displayDebugPrint(" / humi: ");
    displayDebugPrintln(sensorData.humiChar);

    myNtp.loop(sensorData.timeOfDayChar, sensorData.dateChar); // every 10sec read val
    displayDebugPrint("> ntp time: ");
    displayDebugPrint(sensorData.timeOfDayChar);
    displayDebugPrint("  / date: ");
    displayDebugPrintln(sensorData.dateChar);

    displayDebugPrint("> Wifi.RSSI: ");
    sensorData.rssiLevel = dBmtoPercentage(WiFi.RSSI());
    dtostrf(sensorData.rssiLevel, 1, 0, sensorData.rssiChar); // 5 digits, no decimal
    displayDebugPrintln(sensorData.rssiChar);                 //Signal strength in %

    displayUpdate = true;
    lastMsgCo2 = millis();
  }

} // end of function

// ----------------------------------------------------------------------------------------
void loop()
{

  if (wifiMulti.run() != WL_CONNECTED)
  {
    setup_wifi();
  }

  pullData_loop(); // retrieve update form sensors every 20 seconds

  myMqttClient2.loop(); //client.loop(); + reconnect if required
  mqttcmd_loop();       // fetch + process data for subsribed mqtt topics
  // AsyncElegantOTA.loop();

  button1.check();
  button2.check();

  displayLoop(); // mqtt of button may demand to change GUI

  yield();
} // end of function
