/*********************************************
  __version__ = '2023.08.18.01'
  __author__ = 'Muthukumar Subramanian'
  OLED Display Basics
  ESP32-WROOM-DA Module support code
  GitHub: https://github.com/kumarmuthu/ESP32
*********************************************/
#include <SPI.h>
#include <Wire.h>

/*************** Support 128x64 0.96" (7 pins) ********************/
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_MOSI   23
#define OLED_CLK    18
#define OLED_DC     16
#define OLED_CS     5
#define OLED_RESET  17

/*************** Configure 128x64 0.96" (7 pins) ********************/
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT,
                         OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);

/*************** MuthuAutomation Logo in 0.96" Dispaly ********************/
const unsigned char MuthuAutomation [] PROGMEM = {
0xff,0xff,0xfc,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3f,0xff,0xff
,0xff,0xff,0xf8,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1f,0xff,0xff
,0xff,0xff,0xf0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0f,0xff,0xff
,0xff,0xff,0xe0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,0xff,0xff
,0xff,0xff,0xc0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0xff,0xff
,0xff,0xff,0xc0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0xff,0xff
,0xff,0xff,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0xff,0xff
,0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xff,0xff
,0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xff,0xff
,0xff,0xfe,0x00,0x00,0x00,0x00,0x00,0x3f,0xff,0x00,0x00,0x00,0x00,0x00,0x7f,0xff
,0xff,0xfc,0x00,0x00,0x00,0x00,0x03,0xff,0xff,0xf0,0x00,0x00,0x00,0x00,0x3f,0xff
,0xff,0xfc,0x00,0x00,0x00,0x00,0x0f,0xe0,0x00,0xfc,0x00,0x00,0x00,0x00,0x3f,0xff
,0xff,0xf8,0x00,0x00,0x00,0x00,0x3f,0x00,0x00,0x1e,0x00,0x00,0x00,0x00,0x1f,0xff
,0xff,0xf8,0x00,0x00,0x00,0x00,0x7e,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1f,0xff
,0xff,0xf0,0x00,0x00,0x00,0x00,0xfc,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0f,0xff
,0xff,0xf0,0x00,0x00,0x00,0x01,0xf0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0f,0xff
,0xff,0xf0,0x00,0x00,0x00,0x03,0xc0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0f,0xff
,0xff,0xe0,0x00,0x00,0x00,0x07,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,0xff
,0xff,0xe0,0x00,0x00,0x00,0x0f,0x01,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x07,0xff
,0xff,0xe0,0x00,0x00,0x00,0x1e,0x7f,0xc0,0x00,0x00,0x00,0x00,0x00,0x00,0x07,0xff
,0xff,0xc0,0x00,0x00,0x00,0x3e,0x7f,0xc0,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0xff
,0xff,0xc0,0x00,0x00,0x00,0x3c,0x7f,0xe0,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0xff
,0xff,0xc0,0x00,0x00,0x00,0x7c,0x7f,0xf0,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0xff
,0xff,0xc0,0x00,0x00,0x00,0x78,0x7f,0xf8,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0xff
,0xff,0x80,0x00,0x00,0x00,0xf8,0x7f,0xf8,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0xff
,0xff,0x80,0x00,0x00,0x00,0xf8,0x7f,0xfc,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0xff
,0xff,0x80,0x00,0x00,0x00,0xf0,0x7f,0xfe,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0xff
,0xff,0x80,0x00,0x00,0x01,0xf0,0x7f,0xfc,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0xff
,0xff,0x80,0x00,0x00,0x01,0xf0,0x7f,0xf8,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0xff
,0xff,0x80,0x00,0x00,0x01,0xf0,0x7f,0xf0,0x00,0x00,0x02,0x00,0x00,0x00,0x01,0xff
,0xff,0x80,0x00,0x00,0x01,0xf0,0x7f,0x60,0x00,0x00,0x02,0x00,0x00,0x00,0x01,0xff
,0xff,0x80,0x00,0x00,0x01,0xf0,0x7f,0x00,0x00,0x00,0x02,0x00,0x00,0x00,0x01,0xff
,0xff,0x80,0x00,0x00,0x01,0xf0,0x7f,0x00,0x00,0x00,0x02,0x00,0x00,0x00,0x01,0xff
,0xff,0x80,0x00,0x00,0x01,0xf8,0x7f,0x00,0x00,0x08,0x06,0x00,0x00,0x00,0x01,0xff
,0xff,0x80,0x00,0x00,0x01,0xf8,0x7f,0x00,0x00,0x08,0x06,0x00,0x00,0x00,0x01,0xff
,0xff,0x80,0x00,0x00,0x00,0xf8,0x7f,0x00,0x00,0x08,0x04,0x00,0x00,0x00,0x01,0xff
,0xff,0x80,0x00,0x00,0x00,0xf8,0x7f,0x00,0x00,0x08,0x0c,0x00,0x00,0x00,0x01,0xff
,0xff,0x80,0x00,0x00,0x00,0xfc,0x7e,0x00,0x00,0x08,0x1c,0x00,0x00,0x00,0x01,0xff
,0xff,0x80,0x00,0x00,0x00,0xfc,0x7f,0x00,0x00,0x08,0x38,0x00,0x00,0x00,0x01,0xff
,0xff,0x80,0x00,0x00,0x00,0x7e,0x7f,0x00,0x00,0x08,0x78,0x00,0x00,0x00,0x01,0xff
,0xff,0xc0,0x00,0x00,0x00,0x7f,0x7f,0x00,0x00,0x08,0x70,0x00,0x00,0x00,0x03,0xff
,0xff,0xc0,0x00,0x00,0x00,0x3f,0xff,0x00,0x00,0x08,0xe0,0x00,0x00,0x00,0x03,0xff
,0xff,0xc0,0x00,0x00,0x00,0x1f,0xff,0x00,0x00,0x09,0xc4,0x00,0x00,0x00,0x03,0xff
,0xff,0xc0,0x00,0x00,0x00,0x1f,0xfe,0x00,0x00,0x1b,0x88,0x00,0x00,0x00,0x03,0xff
,0xff,0xe0,0x00,0x00,0x00,0x0f,0xfc,0x00,0x00,0x07,0x10,0x00,0x00,0x00,0x07,0xff
,0xff,0xe0,0x00,0x00,0x00,0x07,0xf0,0x00,0x00,0x0e,0x20,0x00,0x00,0x00,0x07,0xff
,0xff,0xe0,0x00,0x00,0x00,0x03,0xf8,0x00,0x00,0x18,0xc0,0x00,0x00,0x00,0x07,0xff
,0xff,0xf0,0x00,0x00,0x00,0x01,0xf8,0x00,0x00,0x73,0x80,0x00,0x00,0x00,0x0f,0xff
,0xff,0xf0,0x00,0x00,0x00,0x00,0xfc,0x00,0x01,0xce,0x00,0x00,0x00,0x00,0x0f,0xff
,0xff,0xf0,0x00,0x00,0x00,0x00,0x3f,0x80,0x04,0x3c,0x00,0x00,0x00,0x00,0x0f,0xff
,0xff,0xf8,0x00,0x00,0x00,0x00,0x0f,0xf8,0x3f,0xf0,0x00,0x00,0x00,0x00,0x1f,0xff
,0xff,0xf8,0x00,0x00,0x00,0x00,0x03,0xff,0xff,0xc0,0x00,0x00,0x00,0x00,0x1f,0xff
,0xff,0xfc,0x00,0x00,0x00,0x00,0x00,0x7f,0xfc,0x00,0x00,0x00,0x00,0x00,0x3f,0xff
,0xff,0xfc,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3f,0xff
,0xff,0xfe,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x7f,0xff
,0xff,0xfe,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xff,0xff
,0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xff,0xff
,0xff,0xff,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0xff,0xff
,0xff,0xff,0xc0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0xff,0xff
,0xff,0xff,0xc0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0xff,0xff
,0xff,0xff,0xe0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,0xff,0xff
,0xff,0xff,0xf0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0f,0xff,0xff
,0xff,0xff,0xf8,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1f,0xff,0xff
,0xff,0xff,0xfc,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3f,0xff,0xff

};

void printArrayContents(const unsigned char *array, size_t size) {
  Serial.println("Array Contents:");
  for (size_t i = 0; i < size; ++i) {
    Serial.print("0x");
    Serial.print(array[i], HEX);
    Serial.print(" ");
    if ((i + 1) % 16 == 0) {
      Serial.println();
    }
  }
  Serial.println();
}


void AllPixels()
{
  int i;
  int j;
  display.clearDisplay();
  for (i = 0; i < 64; i++)
  {
    for (j = 0; j < 128; j++)
    {
      display.drawPixel(j, i, SSD1306_WHITE);

    }
    display.display();
    delay(30);
  }

  for (i = 0; i < 64; i++)
  {
    for (j = 0; j < 128; j++)
    {
      display.drawPixel(j, i, SSD1306_BLACK);

    }
    display.display();
    delay(30);
  }

}


void TextDisplay()
{
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(5, 28);
  display.println("MuthuAutomation");
  display.display();
  delay(3000);
}


void InvertedTextDisplay()
{
  display.clearDisplay();
  display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
  display.setCursor(5, 28);
  display.println("MuthuAutomation");
  display.display();
  delay(3000);
}


void ScrollText()
{
  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.println("This is a");
  display.println("Scrolling");
  display.println("Text!");
  display.display();
  delay(100);
  display.startscrollright(0x00, 0x0F);
  delay(2000);
  display.startscrollleft(0x00, 0x0F);
  delay(2000);
  display.startscrolldiagright(0x00, 0x0F);
  delay(2000);
  display.startscrolldiagleft(0x00, 0x0F);
  delay(2000);
  display.stopscroll();
}


void DisplayChars()
{
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.cp437(true);

  for (int16_t i = 0; i < 256; i++)
  {
    if (i == '\n')
    {
      display.write(' ');
    }
    else
    {
      display.write(i);
    }
  }

  display.display();
  delay(4000);
}


void TextSize()
{
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println(F("Size: 1"));
  display.println(F("ABC"));
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.println("Size: 2");
  display.println(F("ABC"));
  display.display();
  delay(3000);
}


void DrawRectangle()
{
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("Rectangle");
  display.drawRect(0, 15, 90, 45, SSD1306_WHITE);
  display.display();
  delay(2000);
}


void DrawFilledRectangle()
{
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("Filled Rectangle");
  display.fillRect(0, 15, 90, 45, SSD1306_WHITE);
  display.display();
  delay(2000);

}


void DrawRoundRectangle()
{
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Round Rectangle");
  display.drawRoundRect(0, 15, 90, 45, 10, SSD1306_WHITE);
  display.display();
  delay(2000);
}


void DrawFilledRoundRectangle()
{
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Filled Round Rect");
  display.fillRoundRect(0, 15, 90, 45, 10, SSD1306_WHITE);
  display.display();
  delay(2000);

}


void DrawCircle()
{
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Circle");
  display.drawCircle(30, 36, 25, SSD1306_WHITE);
  display.display();
  delay(2000);
}


void DrawFilledCircle()
{
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Filled Circle");
  display.fillCircle(30, 36, 25, SSD1306_WHITE);
  display.display();
  delay(2000);

}


void DrawTriangle()
{
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Triangle");
  display.drawTriangle(30, 15, 0, 60, 60, 60, SSD1306_WHITE);
  display.display();
  delay(2000);
}


void DrawFilledTriangle()
{
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Filled Triangle");
  display.fillTriangle(30, 15, 0, 60, 60, 60, SSD1306_WHITE);
  display.display();
  delay(2000);
}


void setup()
{
  Serial.begin(115200);
  if (!display.begin(SSD1306_SWITCHCAPVCC))
  {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }

  display.clearDisplay();
  display.display();
  delay(1000);
/*************** Call MuthuAutomation Logo ********************/
  display.clearDisplay();
  printArrayContents(MuthuAutomation, sizeof(MuthuAutomation));
  display.drawBitmap(0, 0, MuthuAutomation, SCREEN_WIDTH, SCREEN_HEIGHT, SSD1306_WHITE);
  display.display();
  delay(5000);

}


void loop()
{
  AllPixels();
  TextDisplay();
  InvertedTextDisplay();
  ScrollText();
  DisplayChars();
  TextSize();
  DrawRectangle();
  DrawFilledRectangle();
  DrawRoundRectangle();
  DrawFilledRoundRectangle();
  DrawCircle();
  DrawFilledCircle();
  DrawTriangle();
  DrawFilledTriangle();
}
