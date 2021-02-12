#ifndef __AUDIORECORDER_H__
#define __AUDIORECORDER_H__

#include <functional>
#include <FS.h>

#include "driver/i2s.h"

class AudioRecorder {

    public:
        void loop();
        void setup();
        void record(File*);
        void stop();
};
#endif // __AUDIORECORDER_H__