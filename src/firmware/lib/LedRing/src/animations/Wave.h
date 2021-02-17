#ifndef __WAVE_H__
#define __WAVE_H__
#include <FastLED.h>
#include "animations/LedAnimation.h"

#define MAX_INT_VALUE 65536

class Wave : public LedAnimation
{
    uint16_t frame = 0;
    uint16_t animateSpeed = 100;
    uint16_t hue = 150;

    CRGB *strip;
    uint8_t stripLength;

    void initialize(CRGB targetStrip[], int size, int repetitions)
    {
        this->strip = targetStrip;
        this->stripLength = size;
        this->frame = 0;
    }

    bool run()
    {
        WaveInt(strip, frame, 150);
        FastLED.show();
        frame += animateSpeed;
        return true;
    }

    void WaveInt(CRGB targetStrip[], uint16_t animationFrame, uint8_t hue)
    {
        FastLED.clear();
        uint8_t value;
        for (uint8_t i = 0; i < stripLength; i++)
        {
            value = (sin16(animationFrame + ((MAX_INT_VALUE / stripLength) * i)) + (MAX_INT_VALUE / 2)) / 256;
            if (value >= 10)
            {
                targetStrip[stripLength - 1 - i] += CHSV(hue, 255, value);
            }
        }
    }
};

#endif // __WAVE_H__