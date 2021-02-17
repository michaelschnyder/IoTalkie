#ifndef __LEDANIMATION_H__
#define __LEDANIMATION_H__

#define ANI_REPEAT_FOREVER -1
#define ANI_SPEED_NORMAL 100
#define ANI_SPEED_FAST 200 
#define ANI_SPEED_SUPERFAST 400 

class LedAnimation {


public:
    virtual void initialize(CRGB targetStrip[], int size) = 0;
    virtual bool run() = 0;
};

#endif // __LEDANIMATION_H__