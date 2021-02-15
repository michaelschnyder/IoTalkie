#ifndef __AUDIORECORDER_H__
#define __AUDIORECORDER_H__

#include <functional>
#include <FS.h>

#include "driver/i2s.h"

class AudioRecorder {

    File* currentFile;

    public:
        void loop();
        void setup();
        void record(File*);
        long stop();
        long duration();
};
#endif // __AUDIORECORDER_H__