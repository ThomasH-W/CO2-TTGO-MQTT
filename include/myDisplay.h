/*
  myDisplay.h - Library for mqtt client
*/
#ifndef MYDISPLAY_h
#define MYDISPLAY_h

#include "Arduino.h"

#include <TFT_eSPI.h> // Hardware-specific library
#include <SPI.h>

#include "main.h"

#define DEG2RAD 0.0174532925

class myDisplay // define class
{
public:
  myDisplay(void); // constructor, which is used to create an instance of the class
  bool begin(void);
  void clear(void);
  void loop(void);
  int value(void);
  void print(const char *msg);
  void println(const char *msg);
  void getScreenSize(void);
  int version(void);
   void Text(const char *co2Char, const char *tempChar);
  void Gui1(sensor_data_struct sensorData);
  void Gui2(sensor_data_struct sensorData);
  void Gui3(sensor_data_struct sensorData);
  void Gui4(int min, int max, int value, sensor_data_struct sData);
  void Gui5(sensor_data_struct sensorData);
  void Gui6(sensor_data_struct sensorData);
  void GuiCalibration(int caliTime);
  void Gui7(int x, int y, int start_angle, int seg_count, int rx, int ry, int w, unsigned int colour);
  void Gui8(int curlevel1, int curlevel2, sensor_data_struct sData);

private:
  TFT_eSPI myTFT = TFT_eSPI(); // Invoke custom library
  unsigned long timer_output_0 = 0,
                timer_output_5 = 5000;
  int printlines = 0;
  int16_t tft_h = 1;
  int16_t tft_w = 1;
  void fillArc(int x, int y, int start_angle, int seg_count, int rx, int ry, int w, unsigned int colour);
  void gaugeDraw(int min, int max, int curlevel, int x, int y, int start_angle, int seg_count_max, int rx, int ry, int w);
  void gaugeFull(int min, int max, int curlevel);
  void gaugeLeft(int min, int max, int curlevel);
  void gaugeRight(int min, int max, int curlevel);
  void wfiSignal(int my_x, int my_y, int my_max, int level);
  void drawTrend(sensor_data_struct sData);
  void drawTrendRing(sensor_data_struct sData);
};

#endif
