#include <ArduinoJson.h>
#include <GxEPD2_4G_4G.h>
#include <assets/GithubIcon.h>
#include <assets/WifiIcon.h>
#include <assets/WifiErrorIcon.h>
#include <assets/BrokenLinkIcon.h>
#include <assets/muMatrix8ptRegular.h>

#define BAT_TEST_PIN 35
GxEPD2_4G_4G<GxEPD2_213_GDEY0213B74, GxEPD2_213_GDEY0213B74::HEIGHT> display(GxEPD2_213_GDEY0213B74(/*CS=5*/ SS, /*DC=*/17, /*RST=*/16, /*BUSY=*/4)); // GDEY0213B74 122x250, SSD1680, (FPC-A002 20.04.08)

extern char username[50];
extern bool darkMode;

uint16_t GetColor(int level)
{
  switch (level)
  {
  case 0:
    return darkMode ? GxEPD_BLACK : GxEPD_WHITE;
  case 1:
    return darkMode ? GxEPD_DARKGREY : GxEPD_LIGHTGREY;
  case 2:
    return darkMode ? GxEPD_LIGHTGREY : GxEPD_DARKGREY;
  case 3:
    return darkMode ? GxEPD_WHITE : GxEPD_BLACK;
  case 4:
    return darkMode ? GxEPD_WHITE : GxEPD_BLACK;
  default:
    return darkMode ? GxEPD_BLACK : GxEPD_WHITE;
  }
}

void initScreen() {
  display.init(115200);
  display.setRotation(1); // Landscape
  display.setTextColor(GetColor(4));
  display.setFont(&muMatrix8ptRegular);
  display.setTextSize(1);

  // Set background
  display.fillRect(0, 0, display.width(), display.height(), GetColor(0));
}

void DrawHeader(bool isWifiError, bool isFetchError)
{
  // Draw Github icon
  display.drawBitmap(7, 8, GithubIcon, 16, 16, GetColor(4));

  // Draw username
  display.setCursor(29, 20);
  display.print("/" + String(username));

  // Draw battery percentage
  display.setCursor(display.width() - 25, 20);
  int adcValue = analogRead(BAT_TEST_PIN);
  float voltage = (adcValue / 4095.0) * 3.3 * 2;
  long percentage = map(voltage * 1000, 3200, 3900, 0, 100); // Use millivolts for mapping
  display.printf("%ld%%", percentage);

  // Draw error icons
  if (isWifiError || isFetchError)
  {
    display.drawBitmap(display.width() - 55, 8, isWifiError ? WifiErrorIcon : BrokenLinkIcon, 16, 16, GetColor(4));
  }
}

void drawCommitGraph(int contributions[], bool isWifiError, bool isFetchError)
{
  initScreen();

  int columns = 17;
  int rows = 7;
  int total = columns * rows;
  int squareMargin = 2;
  int gridSize = display.width() / columns;
  int squareSize = gridSize - squareMargin;
  int borderThickness = 3;
  int yOffset = 24;
  int xOffset = 5;

  DrawHeader(isWifiError, isFetchError);

  // Draw squares
  for (int column = 0; column < columns; column++)
  {
    for (int row = 0; row < rows; row++)
    {
      int squareX = column * gridSize + xOffset + squareMargin;
      int squareY = row * gridSize + yOffset + squareMargin;
      int index = rows * column + row;
      int level = contributions[index];

      display.fillRoundRect(squareX, squareY, squareSize, squareSize, borderThickness, GetColor(level));
      display.drawRoundRect(squareX, squareY, squareSize, squareSize, borderThickness, GetColor(level + 1));

      int isToday = level != -1 && (index == total - 1 || contributions[index + 1] == -1);
      if (isToday)
      {
        display.drawRoundRect(squareX, squareY, squareSize, squareSize, borderThickness, GetColor(4));
      }
    }
  }
  display.displayWindow(0, 0, display.width(), display.height());
  display.hibernate();
}

void drawConfigModeScreen(){
  initScreen();
  int margin = 25;
  int lineHeight = 12;
  display.drawBitmap(display.width()-margin-32, margin-8, WifiIcon, 32,32, GetColor(4));

  // Title
  display.setTextSize(2);
  display.setCursor(margin, margin+14);
  display.print("Config Mode");

  // Details
  display.setTextSize(1);
  const int paragraphY = margin+36;
  display.setCursor(margin, paragraphY);
  display.print("Connect to my wifi");
  display.setCursor(margin, paragraphY+lineHeight);
  display.print("'Contributions Screen'");
  display.setCursor(margin, paragraphY+lineHeight*3);
  display.print("Then press 'sign in' to open");
  display.setCursor(margin, paragraphY+lineHeight*4);
  display.print("the browser configuration");

  // Draw the updates
  display.displayWindow(0, 0, display.width(), display.height());
  display.hibernate();
}