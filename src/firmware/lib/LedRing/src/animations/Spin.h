#ifndef __SPIN_H__
#define __SPIN_H__

#include <FastLED.h>
#include "animations/LedAnimation.h"

class Spinner : public LedAnimation {

    #define MAX_INT_VALUE 65536

    uint16_t frame = 0;    
    uint16_t animateSpeed = 100;
    uint16_t hue = 150;

    CRGB* strip;
    uint8_t stripLength;

    void initialize(CRGB targetStrip[], int size) {
        this->strip = targetStrip;
        this->stripLength = size;
        this->frame = 0;
    }

    void run() {
        FastLED.clear();

        int pos16 = map(frame, 0, MAX_INT_VALUE, 0, ((stripLength) * 16));
        drawFractionalBar(strip, pos16, 4, hue, true);

        FastLED.show();	
        frame += animateSpeed;
    }   

    //Anti-aliasing code care of Mark Kriegsman Google+: https://plus.google.com/112916219338292742137/posts/2VYNQgD38Pw
    void drawFractionalBar(CRGB targetStrip[], int pos16, int width, uint8_t hue, bool wrap)
    {        
        uint8_t i = pos16 / 16; // convert from pos to raw pixel number

        uint8_t frac = pos16 & 0x0F; // extract the 'factional' part of the position
        uint8_t firstpixelbrightness = 255 - (frac * 16);
        
        uint8_t lastpixelbrightness = 255 - firstpixelbrightness;

        uint8_t bright;
        for (int n = 0; n <= width; n++) {
            if (n == 0) {
                // first pixel in the bar
                bright = firstpixelbrightness;
            }
            else if (n == width) {
                // last pixel in the bar
                bright = lastpixelbrightness;
            }
            else {
                // middle pixels
                bright = 255;
            }

            targetStrip[i] += CHSV(hue, 255, bright );
            i++;
            if (i == stripLength)
            {
                if (wrap == 1) {
                    i = 0; // wrap around
                }
                else{
                    return;
                }
            }
        }
    }
 
};


#endif // __SPIN_H__