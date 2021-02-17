#ifndef __LEDANIMATION_H__
#define __LEDANIMATION_H__

#define ANIMIATION_REPEAT_FOREVER -1

class LedAnimation {


public:
    virtual void initialize(CRGB targetStrip[], int size) = 0;
    virtual bool run() = 0;
};

#endif // __LEDANIMATION_H__