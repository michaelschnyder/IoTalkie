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

void Screen::showPostScreen() {

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

    // Add Header
    display.setPartialWindow(0, 0, display.width(), topRowHeight);
    display.firstPage();

    do
    {
        // Line on top
        display.drawFastHLine(0, topRowHeight - 1, display.width(), GxEPD_BLACK);
    }
    while (display.nextPage());
}

void Screen::showUpdateScreen() {

    const char Message[] = "Updating...";

    int16_t tbx, tby; uint16_t tbw, tbh;

    display.getTextBounds(Message, 0, 0, &tbx, &tby, &tbw, &tbh);
    uint16_t x = (display.width() - tbw) / 2;
    uint16_t y = (display.height() - tbh) / 2;

    display.setPartialWindow(0, 0, display.width(), display.height());
    display.firstPage();
    
    do
    {
        display.writeFillRect(0, 0, display.width(), display.height(), GxEPD_BLACK);
        display.setTextColor(GxEPD_WHITE);
        display.setFont(&FreeSansBold9pt7b);      
        display.setCursor(x, y);
        display.print(Message);
    }

    while (display.nextPage());

}

void Screen::showHomeScreen() {
    
    // Clear screen
    display.setPartialWindow(0, topRowHeight + 1, display.width(), display.height() - topRowHeight - 1);
    display.firstPage();
    
    do
    {
        display.fillScreen(GxEPD_WHITE);
    }
    while (display.nextPage());    


    // Display contacts
    display.setTextColor(GxEPD_BLACK);

    uint16_t card_height = (display.height() - topRowHeight) / contacts->size();

    for (size_t i = 0; i < contacts->size(); i++)
    {
        display.setFont(&FreeSansBold12pt7b);      

        Contact* c = contacts->get(i);

        int16_t tbx, tby; uint16_t tbw, tbh;

        display.getTextBounds(c->name, 0, 0, &tbx, &tby, &tbw, &tbh);
        
        if (tbh > 18) {
            display.setFont(&FreeSansBold9pt7b);      
            display.getTextBounds(c->name, 0, 0, &tbx, &tby, &tbw, &tbh);
        }

        int16_t card_y_start, card_y_center, card_y_end;

        uint16_t x = 4;
        card_y_start = topRowHeight + (i * card_height);
        card_y_center = card_y_start + card_height / 2;
        card_y_end = topRowHeight + ((i + 1) * card_height) -1;

        uint16_t text_y_start = card_y_center + tbh / 2;

        display.setPartialWindow(0, topRowHeight + (i * card_height), display.width(), card_height);
        display.firstPage();
        
        do
        {
            if (i > 0)
            {
                display.drawFastHLine(0, card_y_start, display.width(), GxEPD_BLACK);
            }

            // display.drawFastHLine(x, card_y_center, display.width(), GxEPD_BLACK);
            // display.drawFastHLine(x, card_y_end, display.width(), GxEPD_BLACK);

            display.setCursor(x, text_y_start);
            display.print(c->name);
        }

        while (display.nextPage());
    }
}

void Screen::setWifiSSID(const char * name) {
    
    display.setFont(0);      
    display.setPartialWindow(0, 0, display.width() / 2, topRowHeight - 1);
    display.firstPage();
    
    do
    {
        display.setTextColor(GxEPD_BLACK);
        display.setCursor(1, 4);
        
        if (strlen(name)) {
            display.print(name);
        }
        else {
            display.print("...");
        }
    }
    while (display.nextPage());
}

void Screen::setConnected(bool status) {

    display.setFont(0);      
    display.setPartialWindow(display.width() / 2, 0, display.width() / 2, topRowHeight - 1);
    display.firstPage();
    
    do
    {
        display.setTextColor(GxEPD_BLACK);
        display.setCursor(102, 4);
        display.print(status ? "<=>": "->X");
    }
    while (display.nextPage());    
}

void Screen::setContacts(Contacts* contacts) {
    Serial.printf("Number of contacts %i\n", contacts->size());

    this->contacts = contacts;
}
