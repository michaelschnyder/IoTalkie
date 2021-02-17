#ifndef __LEDANIMATION_H__
#define __LEDANIMATION_H__

class LedAnimation {

public:
    virtual void initialize(CRGB targetStrip[], int size, int repetitions) = 0;
    virtual bool run() = 0;
};

#endif // __LEDANIMATION_H__