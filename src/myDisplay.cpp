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
// 1 Time/WLAN -- CO2 -- Temp/Humi
void myDisplay::Gui1(sensor_data_struct sData)
{
  // 240 * 135
  //myTFT.fillScreen(GREY);

  myTFT.setTextDatum(BL_DATUM);
  myTFT.setTextColor(TFT_WHITE, TFT_BLACK);

  wfiSignal(200, 30, 18, sData.rssiLevel); // x=100, y=100, max=22

  myTFT.drawString(sData.timeOfDayChar, 10, 35, 4);
  // myTFT.drawString(sData.dateChar, 140, 35, 4);

  myTFT.drawLine(5, 38, 235, 38, TFT_DARKGREY); // xs, ys,  xe, ye,  color

  myTFT.drawString("CO2", 10, 85, 4);

  myTFT.setTextColor(TFT_GREEN, TFT_BLACK);
  if (sData.co2_ppm > 1000)
    myTFT.setTextColor(TFT_YELLOW, TFT_BLACK);
  if (sData.co2_ppm > 2000)
    myTFT.setTextColor(TFT_RED, TFT_BLACK);
  myTFT.fillRect(86, 39, 110, 94, TFT_BLACK); // x,y, w,h, c fill rectangle in pixels
  myTFT.drawString(sData.co2Char, 80, 94, 6); // string, x,y, font

  myTFT.setTextColor(TFT_WHITE, TFT_BLACK);
  myTFT.drawString("ppm", 195, 85, 2); // string, x,y, font

  myTFT.drawLine(5, 95, 235, 95, TFT_DARKGREY); // xs, ys,  xe, ye,  color

  myTFT.drawString(sData.tempChar, 20, 127, 4); // string, x,y, font
  myTFT.drawString("C", 100, 127, 4);           // string, x,y, font

  myTFT.drawString(sData.humiChar, 160, 127, 4); // string, x,y, font
  myTFT.drawString("%", 200, 127, 4);            // string, x,y, font

  printlines = 0;
} // end of function

// ------------------------------------------------------------------------------------------------------------------------
// 2 CO2 -- Temp
void myDisplay::Gui2(sensor_data_struct sData)
{
  myTFT.setTextDatum(BL_DATUM);
  myTFT.setTextColor(TFT_WHITE, TFT_BLACK);

  myTFT.drawString("CO2", 10, 60, 4);
  myTFT.setTextColor(TFT_GREEN, TFT_BLACK);
  if (sData.co2_ppm > 1000)
    myTFT.setTextColor(TFT_YELLOW, TFT_BLACK);
  if (sData.co2_ppm > 2000)
    myTFT.setTextColor(TFT_RED, TFT_BLACK);
  myTFT.fillRect(80, 0, 110, 65, TFT_BLACK);  // x,y, w,h, c fill rectangle in pixels
  myTFT.drawString(sData.co2Char, 80, 65, 6); // string, x,y, font
  myTFT.setTextColor(TFT_WHITE, TFT_BLACK);
  myTFT.drawString("ppm", 190, 60, 4); // string, x,y, font

  myTFT.drawString("Temp", 10, 110, 4);
  //myTFT.drawString("123", 80, 100, 4);
  myTFT.drawString(sData.tempChar, 85, 120, 6); // string, x,y, font
  myTFT.drawString("C", 190, 110, 4);           // string, x,y, font

  printlines = 0;

} // end of function

// ------------------------------------------------------------------------------------------------------------------------
// 3 CO2 Text w/ color
void myDisplay::Gui3(sensor_data_struct sData)
{
  uint32_t bg; // background color

  if (sData.co2_ppm <= 1000)
  {
    bg = TFT_GREEN;
    myTFT.setTextColor(TFT_BLACK, bg);
  }
  if (sData.co2_ppm > 1000)
  {
    bg = TFT_YELLOW;
    myTFT.setTextColor(TFT_BLACK, bg);
  }
  if (sData.co2_ppm > 2000)
  {
    bg = TFT_RED;
    myTFT.setTextColor(TFT_WHITE, bg);
  }
  myTFT.fillScreen(bg);
  myTFT.setTextDatum(BC_DATUM);
  myTFT.drawString(sData.co2Char, tft_w / 2, tft_h - 15, 8); // string, x,y, font
  // myTFT.drawString(sData.co2Char, 2, 120, 8); // string, x,y, font

  myTFT.setTextDatum(BL_DATUM);
  myTFT.setTextColor(TFT_BLACK, bg);
  myTFT.drawString("CO2", 5, 35, 4);   // string, x,y, font
  myTFT.drawString("ppm", 190, 35, 4); // string, x,y, font

  printlines = 0;

} // end of function

// ------------------------------------------------------------------------------------------------------------------------
// 4 CO2 Gauge
void myDisplay::Gui4(int min, int max, int value, sensor_data_struct sData)
{
  // 240 * 135
  // myTFT.fillScreen(TFT_BLACK);
  myTFT.setTextColor(TFT_WHITE, TFT_BLACK);
  gaugeFull(min, max, value);
  myTFT.drawString("CO2", 5, 35, 4);            // string, x,y, font
  myTFT.drawString("ppm", 190, 35, 4);          // string, x,y, font
  myTFT.drawString(sData.co2Char, 100, 120, 4); // string, x,y, font

  // wfiSignal(5);
  printlines = 0;
} // end of function

// ------------------------------------------------------------------------------------------------------------------------
// 5 Admin: WiFi, Sensor Version, Sensor ID
void myDisplay::Gui5(sensor_data_struct sData)
{
  // 240 * 135
  // myTFT.fillScreen(TFT_BLACK);
  myTFT.setTextColor(TFT_WHITE, TFT_BLACK);

  //wfiSignal(aData.rssiLevel);
  wfiSignal(200, 30, 18, sData.rssiLevel); // x=100, y=100, max=22

  myTFT.drawString(sData.ssidChar, 3, 30, 4); // string, x,y, font

  myTFT.drawString("IP", 3, 65, 4);          // string, x,y, font
  myTFT.drawString(sData.IPChar, 40, 65, 4); // string, x,y, font

  myTFT.drawString("CO2", 3, 100, 4);            // string, x,y, font
  myTFT.drawString(sData.co2FWChar, 40, 100, 4); // string, x,y, font

  printlines = 0;
} // end of function

// ------------------------------------------------------------------------------------------------------------------------
// Trend graph
void myDisplay::Gui6(sensor_data_struct sData)
{
  myTFT.fillScreen(TFT_BLACK);

  // title
  myTFT.setTextColor(TFT_WHITE, TFT_BLACK);
  myTFT.drawString(sData.co2Char, 120, 35, 4); // string, x,y, font
  myTFT.drawString("ppm", 190, 35, 4);         // string, x,y, font

  // drawTrend(sData); // bars
  drawTrendRing(sData); // line

} // end of function

// ------------------------------------------------------------------------------------------------------------------------
void myDisplay::drawTrendRing(sensor_data_struct sData)
{
  int gx0 = 30; // x = 0
  static int xPos = gx0 + 1;
  int gxM = tft_w - 10; // max x
  int gy0 = tft_h - 10; // y=0
  int gyM = 35;         // max y
  int gyH = gy0 - gyM;
  int iVal = 0;
  int graphHeight;
  int iCO2;

  uint32_t lineColor;

  // determine y-values
  int gy1 = gy0 - map(1000, 0, 3000, 0, gyH);
  int gy2 = gy0 - map(2000, 0, 3000, 0, gyH);

  // y-titles and lines
  myTFT.setTextColor(TFT_RED, TFT_BLACK);
  myTFT.drawString("3k", 2, gyM + 4, 2); // string, x,y, font
  myTFT.setTextColor(TFT_RED, TFT_BLACK);
  myTFT.drawString("2k", 2, gy2 + 4, 2); // string, x,y, font
  myTFT.setTextColor(TFT_YELLOW, TFT_BLACK);
  myTFT.drawString("1k", 2, gy1 + 4, 2); // string, x,y, font
  myTFT.drawString("0", 2, gy0 + 4, 2);  // string, x,y, font

  // X-Axis
  myTFT.drawLine(gx0, gy0, gxM, gy0, TFT_DARKGREY); // xs, ys,  xe, ye,  color
  myTFT.drawLine(gx0, gy1, gxM, gy1, TFT_DARKGREY); // xs, ys,  xe, ye,  color
  myTFT.drawLine(gx0, gy2, gxM, gy2, TFT_DARKGREY); // xs, ys,  xe, ye,  color
  // y-Axis
  myTFT.drawLine(gx0, gy0, gx0, gyM, TFT_DARKGREY); // xs, ys,  xe, ye,  color

  Serial.printf("gx0 %d, gxM %d, gy0 %d, gyM %d, gyH %d\n", gx0, gxM, gy0, gyM, gyH);

  /*
  if (sData.co2_ppm <= 1000)
    lineColor = TFT_GREEN;
  if (sData.co2_ppm > 1000)
    lineColor = TFT_YELLOW;
  if (sData.co2_ppm > 2000)
    lineColor = TFT_RED;
    */
  lineColor = TFT_BLUE;

  for (iVal = 0, xPos = gx0 + 1; xPos < gxM; iVal++, xPos++)
  {
    iCO2 = sData.myCo2Buffer[iVal];
    graphHeight = map(iCO2, 0, 3000, 0, gyH);
    // Serial.printf("Line %3d [%4d]: x %d, y0 %d, max %d\n", iVal, iCO2, xPos, gy0 - 1, graphHeight);
    // myTFT.drawLine(xPos, gy0 - 1, xPos, gy0 - graphHeight, lineColor); // xs, ys,  xe, ye,  color
    myTFT.drawLine(xPos, gy0 - graphHeight, xPos, gy0 - graphHeight, lineColor); // xs, ys,  xe, ye,  color
  }

} // end of function

// ------------------------------------------------------------------------------------------------------------------------
void myDisplay::drawTrend(sensor_data_struct sData)
{

  int gx0 = 30; // x = 0
  static int xPos = gx0 + 1;
  int gxM = tft_w - 10; // max x
  int gy0 = tft_h - 10; // y=0
  int gyM = 35;         // max y
  int gyH = gy0 - gyM;
  uint32_t lineColor;

  if (xPos >= gxM)
  {
    xPos = gx0 + 1;
    myTFT.fillScreen(TFT_BLACK);
  }
  else
    xPos++;

  // determine y-values
  int gy1 = gy0 - map(1000, 0, 3000, 0, gyH);
  int gy2 = gy0 - map(2000, 0, 3000, 0, gyH);
  int graphHeight = map(sData.co2_ppm, 0, 3000, 0, gyH);

  // y-titles and lines
  myTFT.drawString("3k", 1, gyM + 4, 2); // string, x,y, font
  myTFT.drawString("2k", 1, gy2 + 4, 2); // string, x,y, font
  myTFT.drawString("1k", 1, gy1 + 4, 2); // string, x,y, font

  // X-Axis
  myTFT.drawLine(gx0, gy0, gxM, gy0, TFT_WHITE);  // xs, ys,  xe, ye,  color
  myTFT.drawLine(gx0, gy1, gxM, gy1, TFT_YELLOW); // xs, ys,  xe, ye,  color
  myTFT.drawLine(gx0, gy2, gxM, gy2, TFT_RED);    // xs, ys,  xe, ye,  color
  // y-Axis
  myTFT.drawLine(gx0, gy0, gx0, gyM, TFT_WHITE); // xs, ys,  xe, ye,  color

  Serial.printf("gx0 %d, gxM %d, gy0 %d, gyM %d, gyH %d\n", gx0, gxM, gy0, gyM, gyH);
  Serial.printf("Line: x %d, y0 %d, max %d\n", xPos, gy0 - 1, graphHeight);

  if (sData.co2_ppm <= 1000)
    lineColor = TFT_GREEN;
  if (sData.co2_ppm > 1000)
    lineColor = TFT_YELLOW;
  if (sData.co2_ppm > 2000)
    lineColor = TFT_RED;

  myTFT.drawLine(xPos, gy0 - 1, xPos, gy0 - graphHeight, lineColor); // xs, ys,  xe, ye,  color

} // end of function

// ------------------------------------------------------------------------------------------------------------------------
void myDisplay::Gui8(int curlevel1, int curlevel2, sensor_data_struct sData)
{
  // gaugeFull(0, 4000, curlevel1);
  gaugeLeft(0, 4000, curlevel1);
  myTFT.drawLine(120, 30, 120, 120, TFT_WHITE);
  gaugeRight(0, 8000, curlevel2);
} // end of function

// ------------------------------------------------------------------------------------------------------------------------
void myDisplay::gaugeFull(int min, int max, int curlevel)
{
  int x = 120, y = 102, start_angle = 270, seg_count_max = 30, rx = 88, ry = 76, w = 19;

  gaugeDraw(min, max, curlevel, x, y, start_angle, seg_count_max, rx, ry, w);

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
// test for fill arc
void myDisplay::Gui7(int x, int y, int start_angle, int seg_count, int rx, int ry, int w, unsigned int colour)
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
void myDisplay::wfiSignal(int my_x, int my_y, int my_max, int level) // 100,100,22
{
  // int my_x = 100;
  // int my_y = 100;
  int my_w = 3;
  int my_step = my_w + 5;
  // int my_i = 4; // 3 * 4 + 10 = 12+10 // my_i = (max -10) / 3
  int my_i = (my_max - 10) / 3;

  if (level > 0)
    myTFT.fillRect(my_x + 0 * my_step, my_y - 10, my_w, 10, TFT_WHITE); // width / height of rectangle in pixels
  if (level > 1)
    myTFT.fillRect(my_x + 1 * my_step, my_y - 10 - 1 * my_i, my_w, 10 + 1 * my_i, TFT_WHITE); // width / height of rectangle in pixels
  if (level > 2)
    myTFT.fillRect(my_x + 2 * my_step, my_y - 10 - 2 * my_i, my_w, 10 + 2 * my_i, TFT_WHITE); // width / height of rectangle in pixels
  if (level > 3)
    myTFT.fillRect(my_x + 3 * my_step, my_y - 10 - 3 * my_i, my_w, 10 + 3 * my_i, TFT_WHITE); // width / height of rectangle in pixels

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
