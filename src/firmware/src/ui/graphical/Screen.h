#ifndef __SCREEN_H__
#define __SCREEN_H__

#include <Adafruit_GFX.h>
#include <GxEPD2_BW.h> // including both doesn't use more code or ram
#include <GxEPD2_3C.h> // including both doesn't use more code or ram
#include "hardware.h"
#include <Fonts/FreeSansBold9pt7b.h>
#include <Fonts/FreeSansBold12pt7b.h>
#include <Fonts/FreeSans9pt7b.h>

#include "mailbox/Contacts.h"

class Screen {

    const int topRowHeight = 16;

public:
    Screen() : otherSPI(SCRN_SPI_CHAN) {};
    void setup();

    void showPostScreen();
    void showSleepScreen();
    void showHomeScreen();
    void showUpdateScreen();
    void setUpdateProgress(int);
    void showErrorScreen(int code, const char* message);

    void setWifiSSID(const char * name);
    void setConnected(bool status);
    void setContacts(Contacts*);

private: 
    SPIClass otherSPI;
    Contacts* contacts;
};

#endif // __SCREEN_H__