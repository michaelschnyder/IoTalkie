#ifndef __GLOWANIMATION_H__
#define __GLOWANIMATION_H__

#include <FastLED.h>
#include "animations/LedAnimation.h"

#define MAX_INT_VALUE 65536

class GlowAnimation : public LedAnimation
{
    uint16_t hue = 7;
    uint16_t repetitions;
    uint16_t animateSpeed = 100;

    uint16_t frame = 0;
    uint16_t remainingRepetitions;

    uint8_t oldValue = 0;

    CRGB *strip;
    uint8_t stripLength;

    void initialize(CRGB targetStrip[], int size)
    {
        this->strip = targetStrip;
        this->stripLength = size;
        this->frame = 0;
        this->remainingRepetitions = repetitions;
    }

    bool run()
    {
        FastLED.clear();
        uint8_t value = (sin16(frame) + (MAX_INT_VALUE / 2)) / 256;

        if (value == 0 && oldValue > 0) {
            remainingRepetitions--;

            if (remainingRepetitions == 0) {
                return false;
            }
        }

        for (uint8_t i = 0; i < stripLength; i++)
        {
            if (value >= 10)
            {
                strip[i] += CHSV(hue, 255, value);
            }
        }
        FastLED.show();

        oldValue = value;
        frame += animateSpeed;
        return true;
    }
public:
    GlowAnimation(uint8_t hue = HUE_BLUE, uint8_t speed = ANI_SPEED_NORMAL, uint8_t repetitions = ANI_REPEAT_FOREVER)
    {
        this->hue = hue;
        this->animateSpeed = speed;
        this->repetitions = repetitions;
        this->remainingRepetitions = repetitions;
    }
};

#endif // __GLOWANIMATION_H__