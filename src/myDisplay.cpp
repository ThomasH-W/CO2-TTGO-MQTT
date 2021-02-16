/*

https://github.com/Bodmer/TFT_eSPI

*/
#include "Arduino.h"
#include "myDisplay.h"




// ------------------------------------------------------------------------------------------------------------------------
// Class members
// constructor, which is used to create an instance of the class
myDisplay::myDisplay(void)
{

} // end of function

// ------------------------------------------------------------------------------------------------------------------------
bool myDisplay::begin(void)
{
  Serial.print("Display> begin() ... ");
  myTFT.init();
  myTFT.setRotation(3); // 0-portarit, 1-landscape, 2-flip_portrait, 3-flip_landscape
  getScreenSize();
  myTFT.fillScreen(TFT_BLACK);

  myTFT.setCursor(0, 0, 2);
  myTFT.setTextColor(TFT_WHITE, TFT_BLACK);
  myTFT.setTextSize(1);
  Serial.println(" complete");
  return true;
} // end of function

// ------------------------------------------------------------------------------------------------------------------------
void myDisplay::clear(void)
{
  myTFT.fillScreen(TFT_BLACK);
  myTFT.setCursor(0, 0, 2);
  printlines = 1;
} // end of function

// ------------------------------------------------------------------------------------------------------------------------
void myDisplay::getScreenSize(void)
{
  tft_w = myTFT.width();
  tft_h = myTFT.height();
  Serial.printf("Screen size %u * %u", tft_w, tft_h);
} // end of function

// ------------------------------------------------------------------------------------------------------------------------
void myDisplay::println(const char *msg)
{
  bool errorFlag = strncmp(msg, "ERR", 3);
  if (errorFlag)
    myTFT.setTextColor(TFT_WHITE, TFT_BLACK); // TFT_WHITE
  else
    myTFT.setTextColor(TFT_RED, TFT_BLACK);

  if (++printlines > 8)
  {
    myTFT.fillScreen(TFT_BLACK);
    myTFT.setCursor(0, 0, 2);
    printlines = 1;
  }
  myTFT.println(msg);
} // end of function

// ------------------------------------------------------------------------------------------------------------------------
void myDisplay::print(const char *msg)
{
  bool errorFlag = strncmp(msg, "ERR", 3);
  if (errorFlag)
    myTFT.setTextColor(TFT_WHITE, TFT_BLACK); // TFT_WHITE
  else
    myTFT.setTextColor(TFT_RED, TFT_BLACK);
  myTFT.print(msg);
} // end of function

// ------------------------------------------------------------------------------------------------------------------------
/*
  version() returns the version of the library:
*/
int myDisplay::value(void)
{
  return 1; // temp_ds
} // end of function

// ------------------------------------------------------------------------------------------------------------------------
/*
  version() returns the version of the library:
*/
int myDisplay::version(void)
{
  return 1;
} // end of function

// ------------------------------------------------------------------------------------------------------------------------
void myDisplay::Text(const char *co2Char, const char *tempChar)
{
  myTFT.setTextColor(TFT_WHITE, TFT_BLACK);
} // end of function

// ------------------------------------------------------------------------------------------------------------------------
void myDisplay::Gui1(const char *co2Char, const char *tempChar)
{
  // 240 * 135
  //myTFT.fillScreen(GREY);

  myTFT.setTextDatum(BL_DATUM);
  myTFT.setTextColor(TFT_WHITE, TFT_BLACK);

  myTFT.drawString("CO2", 10, 60, 4);
  myTFT.setTextColor(TFT_RED, TFT_BLACK);
  //myTFT.drawString("4567", 80, 60, 6);
  //Serial.println("Gui1 refresh");
  myTFT.fillRect(80, 0, 110, 65, TFT_BLACK); // x,y, w,h, c fill rectangle in pixels
  myTFT.drawString(co2Char, 80, 65, 6);
  myTFT.setTextColor(TFT_WHITE, TFT_BLACK);
  myTFT.drawString("ppm", 190, 60, 4);

  myTFT.drawString("Temp", 10, 110, 4);
  //myTFT.drawString("123", 80, 100, 4);
  myTFT.drawString(tempChar, 100, 120, 6);
  myTFT.drawString("C", 190, 110, 4);

  printlines = 0;
} // end of function

// ------------------------------------------------------------------------------------------------------------------------
void myDisplay::gaugeFull(int min, int max, int curlevel)
{
  int x = 120, y = 102, start_angle = 270, seg_count_max = 30, rx = 88, ry = 76, w = 19;

  gaugeDraw(min, max, curlevel, x, y, start_angle, seg_count_max, rx, ry, w);
  myTFT.drawString("ppm", 120, 90, 4);

} // end of function

// ------------------------------------------------------------------------------------------------------------------------
void myDisplay::gaugeLeft(int min, int max, int curlevel)
{
  int x = 60, y = 82, start_angle = 270, seg_count_max = 30, rx = 48, ry = 42, w = 10;

  gaugeDraw(min, max, curlevel, x, y, start_angle, seg_count_max, rx, ry, w);
  myTFT.drawString("ppm", 60, 80, 2);
} // end of function

// ------------------------------------------------------------------------------------------------------------------------
void myDisplay::gaugeRight(int min, int max, int curlevel)
{
  int x = 180, y = 82, start_angle = 270, seg_count_max = 30, rx = 48, ry = 42, w = 10;

  gaugeDraw(min, max, curlevel, x, y, start_angle, seg_count_max, rx, ry, w);
  myTFT.drawString("C", 180, 70, 2);
} // end of function

// ------------------------------------------------------------------------------------------------------------------------
void myDisplay::gaugeDraw(int min, int max, int curlevel, int x, int y, int start_angle, int seg_count_max, int rx, int ry, int w)
{

  // int x = 120, y = 102, start_angle = 270, seg_count_max = 30, rx = 88, ry = 76, w = 19;
  unsigned int colour = TFT_RED;

  int range = max - min;
  int needle = curlevel - min;
  int curPerc = needle * 100 / range;
  int seg_count = (curPerc * seg_count_max) / 100;

  int seg_count_1 = seg_count_max / 3;
  int seg_count_2 = seg_count_max / 3 * 2;

  if (seg_count <= seg_count_1)
  {
    fillArc(x, y, start_angle, seg_count, rx, ry, w, TFT_GREEN);
  }

  if (seg_count <= seg_count_2 && seg_count > seg_count_1)
  {
    fillArc(x, y, start_angle, seg_count_1, rx, ry, w, TFT_GREEN);
    fillArc(x, y, start_angle + seg_count_1 * 6, seg_count - seg_count_1, rx, ry, w, TFT_YELLOW);
  }

  if (seg_count > seg_count_2)
  {
    fillArc(x, y, start_angle, seg_count_1, rx, ry, w, TFT_GREEN);
    fillArc(x, y, start_angle + seg_count_1 * 6, seg_count - seg_count_1, rx, ry, w, TFT_YELLOW);
    // fillArc(x, y, start_angle, seg_count_2, rx, ry, w, TFT_YELLOW);
    fillArc(x, y, start_angle + seg_count_2 * 6, seg_count - seg_count_2, rx, ry, w, colour);
  }

  /*
  Serial.print("Gauge Full> min ");
  Serial.println(min);
  Serial.print("Gauge Full> max ");
  Serial.println(max);
  Serial.print("Gauge Full> cur ");
  Serial.println(curlevel);

  Serial.print("Gauge Full> range ");
  Serial.println(range);
  Serial.print("Gauge Full> needle ");
  Serial.println(needle);
  Serial.print("Gauge Full> percent ");
  Serial.println(curPerc);
  Serial.print("Gauge Full> percent calc ");
  Serial.println(needle / range);
  Serial.print("Gauge Full> seg_count ");
  Serial.println(seg_count);
*/

} // end of function

// ------------------------------------------------------------------------------------------------------------------------
void myDisplay::Gui2(int min, int max, int curlevel)
{
  // gaugeFull(0, 4000, curlevel);
  gaugeLeft(0, 4000, curlevel);
  myTFT.drawLine(120, 30, 120, 120, TFT_WHITE);
  gaugeRight(0, 8000, curlevel);
} // end of function

// ------------------------------------------------------------------------------------------------------------------------
void myDisplay::Gui5(int x, int y, int start_angle, int seg_count, int rx, int ry, int w, unsigned int colour)
{
  // 240 * 135
  // myTFT.fillScreen(TFT_BLACK);

  byte inc = 0;
  unsigned int col = 0;

  // Continuous elliptical arc drawing
  fillArc(x, y, start_angle, seg_count, rx, ry, w, TFT_RED);

  // Continuous segmented (inc*2) elliptical arc drawing
  // fillArc(160, 120, ((inc * 2) % 60) * 6, 1, 120, 80, 30, TFT_GREEN);

  // Circle drawing using arc with arc width = radius
  // fillArc(160, 120, inc * 6, 1, 42, 42, 42, TFT_BLUE);

  inc++;
  col += 1;
  if (col > 191)
    col = 0;
  if (inc > 59)
    inc = 0;

} // end of function

// ------------------------------------------------------------------------------------------------------------------------
void myDisplay::Gui3(const char *co2Char, const char *tempChar)
{
  // 240 * 135
  // myTFT.fillScreen(TFT_BLACK);
  myTFT.setTextColor(TFT_WHITE, TFT_BLACK);

  wfiSignal(5);

  printlines = 0;
} // end of function

// ------------------------------------------------------------------------------------------------------------------------
void myDisplay::GuiCalibration(int caliTime)
{
  if (caliTime > 0)
  {
    myTFT.fillScreen(TFT_RED);
    myTFT.setTextColor(TFT_WHITE, TFT_RED);
  }
  else
  {
    myTFT.fillScreen(TFT_GREEN);
    myTFT.setTextColor(TFT_WHITE, TFT_GREEN);
    caliTime = 0;
  }

  myTFT.setTextDatum(BL_DATUM);
  myTFT.drawString("Calibrating", 40, 50, 4);
  myTFT.setCursor(40, 80, 6);
  myTFT.print(caliTime);

} // end of function

// ------------------------------------------------------------------------------------------------------------------------
void myDisplay::wfiSignal(int level)
{
  int my_x = 100;
  int my_y = 100;
  int my_w = 3;
  int my_step = my_w + 5;
  int my_i = 4;

  myTFT.fillRect(my_x + 0 * my_step, my_y - 10, my_w, 10, TFT_RED);                       // width / height of rectangle in pixels
  myTFT.fillRect(my_x + 1 * my_step, my_y - 10 - 1 * my_i, my_w, 10 + 1 * my_i, TFT_RED); // width / height of rectangle in pixels
  myTFT.fillRect(my_x + 2 * my_step, my_y - 10 - 2 * my_i, my_w, 10 + 2 * my_i, TFT_RED); // width / height of rectangle in pixels
  myTFT.fillRect(my_x + 3 * my_step, my_y - 10 - 3 * my_i, my_w, 10 + 3 * my_i, TFT_RED); // width / height of rectangle in pixels
  myTFT.fillRect(my_x + 4 * my_step, my_y - 10 - 4 * my_i, my_w, 10 + 4 * my_i, TFT_RED); // width / height of rectangle in pixels
} // end of function

// ------------------------------------------------------------------------------------------------------------------------
void myDisplay::Gui4(const char *co2Char, const char *tempChar)
{
  // 240 * 135
  // myTFT.fillScreen(TFT_BLACK);
  myTFT.setTextColor(TFT_WHITE, TFT_BLACK);

  myTFT.setTextDatum(TC_DATUM);
  myTFT.drawString("tc", 30, 30, 4); //int drawString(char *string, int poX, int poY, int size);

  myTFT.setTextDatum(BL_DATUM);
  myTFT.drawString("bl", 60, 60, 4);

  myTFT.setTextDatum(BL_DATUM);
  myTFT.drawString("bl", 120, 60, 4);
  myTFT.drawString("l", 0, 100, 4);
  myTFT.drawString("c", 120, 100, 4);
  myTFT.drawString("r", 230, 100, 4);

  myTFT.setTextDatum(BR_DATUM);
  myTFT.drawString("br", 120, 120, 4);

  printlines = 0;
} // end of function

// #########################################################################
// Draw a circular or elliptical arc with a defined thickness
// #########################################################################

// x,y == coords of centre of arc
// start_angle = 0 - 359
// seg_count = number of 6 degree segments to draw (60 => 360 degree arc)
// rx = x axis outer radius
// ry = y axis outer radius
// w  = width (thickness) of arc in pixels
// colour = 16 bit colour value
// Note if rx and ry are the same then an arc of a circle is drawn

// ------------------------------------------------------------------------------------------------------------------------
void myDisplay::fillArc(int x, int y, int start_angle, int seg_count, int rx, int ry, int w, unsigned int colour)
{

  byte seg = 6; // Segments are 3 degrees wide = 120 segments for 360 degrees
  byte inc = 6; // Draw segments every 3 degrees, increase to 6 for segmented ring

  // Calculate first pair of coordinates for segment start
  float sx = cos((start_angle - 90) * DEG2RAD);
  float sy = sin((start_angle - 90) * DEG2RAD);
  uint16_t x0 = sx * (rx - w) + x;
  uint16_t y0 = sy * (ry - w) + y;
  uint16_t x1 = sx * rx + x;
  uint16_t y1 = sy * ry + y;

  // Draw colour blocks every inc degrees
  for (int i = start_angle; i < start_angle + seg * seg_count; i += inc)
  {

    // Calculate pair of coordinates for segment end
    float sx2 = cos((i + seg - 90) * DEG2RAD);
    float sy2 = sin((i + seg - 90) * DEG2RAD);
    int x2 = sx2 * (rx - w) + x;
    int y2 = sy2 * (ry - w) + y;
    int x3 = sx2 * rx + x;
    int y3 = sy2 * ry + y;

    myTFT.fillTriangle(x0, y0, x1, y1, x2, y2, colour);
    myTFT.fillTriangle(x1, y1, x2, y2, x3, y3, colour);

    // Copy segment end to sgement start for next segment
    x0 = x2;
    y0 = y2;
    x1 = x3;
    y1 = y3;
  }
}
