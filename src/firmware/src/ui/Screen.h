#ifndef __SCREEN_H__
#define __SCREEN_H__

#include <Adafruit_GFX.h>
#include <GxEPD2_BW.h> // including both doesn't use more code or ram
#include <GxEPD2_3C.h> // including both doesn't use more code or ram
#include "../hardware.h"

class Screen {

public:
    Screen() : otherSPI(SCRN_SPI_CHAN) {};
    void setup();

private: 
    SPIClass otherSPI;
};

#endif // __SCREEN_H__