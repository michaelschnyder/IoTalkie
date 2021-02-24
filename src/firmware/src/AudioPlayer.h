#ifndef __AUDIOPLAYER_H__
#define __AUDIOPLAYER_H__

#include <functional>
#include <FS.h>
#include <Log4Esp.h>

class AudioPlayer {
    bool playing = false;
    log4Esp::Logger logger = log4Esp::Logger("AudioPlayer");

    public:
        void setup();
        void play(const char* filename);
        void loop();

        bool isPlaying();
};
#endif // __AUDIOPLAYER_H__