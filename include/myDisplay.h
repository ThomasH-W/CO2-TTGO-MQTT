/*
  myDisplay.h - Library for mqtt client
*/
#ifndef MYDISPLAY_h
#define MYDISPLAY_h

#include "Arduino.h"

#include <TFT_eSPI.h> // Hardware-specific library
#include <SPI.h>

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
  void Gui1(const char *co2Char, const char *tempChar);
  void Gui2(int min, int max, int curlevel);
  void Gui5(int x, int y, int start_angle, int seg_count, int rx, int ry, int w, unsigned int colour);
  void Gui3(const char *co2Char, const char *tempChar);
  void Gui4(const char *co2Char, const char *tempChar);
  void GuiCalibration(int caliTime);
  void gaugeDraw(int min, int max, int curlevel, int x, int y, int start_angle, int seg_count_max, int rx, int ry, int w);
  void gaugeFull(int min, int max, int curlevel);
  void gaugeLeft(int min, int max, int curlevel);
  void gaugeRight(int min, int max, int curlevel);
  void wfiSignal(int level);

private:
  TFT_eSPI myTFT = TFT_eSPI(); // Invoke custom library
  unsigned long timer_output_0 = 0,
                timer_output_5 = 5000;
  int printlines = 0;
  int16_t tft_h = 1;
  int16_t tft_w = 1;
  void fillArc(int x, int y, int start_angle, int seg_count, int rx, int ry, int w, unsigned int colour);
};

#endif
