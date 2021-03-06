#ifndef __LEDRING_H__
#define __LEDRING_H__

#include <FastLED.h>

#include "animations/LedAnimation.h"

// TODO: Move to ctor args
#define NUM_LEDS 24
const int LED_RING_PIN = 00; // White

class LedRing {

CRGB strip[NUM_LEDS];

LedAnimation* currentAnimation;

public:
    void setup();
    void loop();

    void progress(int);
    void reset();
    void show(LedAnimation*);
    void show(LedAnimation*, int repetitions);
    void hide(LedAnimation*);
};


#endif // __LEDRING_H__