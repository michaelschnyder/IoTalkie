#include "Screen.h"
#include "display_selection.h"
#include "core/BuildInfo.h"

void Screen::setup() {    
    display.init(SCRN_SCK, SCRN_BUSY, SCRN_SDI, SCRN_CS, otherSPI);

    const char HelloWorld[] = "IoTalkie";

    display.setRotation(0);
    display.setTextColor(GxEPD_BLACK);

    // Hello Text
    display.setFont(&FreeSansBold12pt7b);      
    int16_t tbx, tby; uint16_t tbw, tbh;
    display.getTextBounds(HelloWorld, 0, 0, &tbx, &tby, &tbw, &tbh);
    uint16_t hello_x = ((display.width() -6 - tbw) / 2) - tbx;
    uint16_t hello_y = (display.height() - tbh - 15);

    display.setPartialWindow(0, (display.height() / 4) * 3, display.width(), display.height() / 4);
    display.firstPage();
    
    do
    {
        display.setFont(&FreeSansBold12pt7b);
        display.setTextColor(GxEPD_BLACK);
        display.setCursor(hello_x, hello_y);
        display.print(HelloWorld);
    }

    while (display.nextPage());
}

void Screen::post() {

    char info[20];
    sprintf(info, "%s-%s", BuildInfo::buildDateVersion(), BuildInfo::gitCommit());

    // Version Info
    display.setFont(0);      
    int16_t ibx, iby; uint16_t ibw, ibh;
    display.getTextBounds(info, 0, 0, &ibx, &iby, &ibw, &ibh);
    uint16_t info_x = ((display.width() -6 - ibw) / 2) - ibx;
    uint16_t info_y = (display.height() - ibh - 5);

    display.setPartialWindow(info_x, info_y, ibw, ibh);
    display.firstPage();
    
    do
    {
        display.setFont(0);
        display.setTextColor(GxEPD_BLACK);
        display.setCursor(info_x, info_y);
        display.print(info);
    }
    while (display.nextPage());    
}