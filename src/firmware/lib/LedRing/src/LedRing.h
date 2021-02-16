#ifndef __LEDRING_H__
#define __LEDRING_H__

#include <FastLED.h>

#include "animations/LedAnimation.h"

// TODO: Move to ctor args
#define NUM_LEDS 24
const int LED_RING_PIN = 12; // White

class LedRing {

CRGB strip[NUM_LEDS];

public:
    void setup();
    void loop();

    void progress(int);
    void reset();
    void show(LedAnimation*);

};

#endif // __LEDRING_H__