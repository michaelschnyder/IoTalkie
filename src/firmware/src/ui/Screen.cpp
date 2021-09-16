#include "Screen.h"
#include "display_selection.h"

void Screen::setup() {
    
  display.init(115200, SCRN_SCK, SCRN_BUSY, SCRN_SDI, SCRN_CS, otherSPI);

  // comment out next line to have no or minimal Adafruit_GFX code
  display.setTextColor(GxEPD_BLACK);
  display.firstPage();
  do
  {
    display.fillScreen(GxEPD_WHITE);
    // comment out next line to have no or minimal Adafruit_GFX code
    display.print("Hello World!");
  }
  while (display.nextPage());
  
  // display.hibernate();
}