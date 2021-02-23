#ifndef __AUDIOPLAYER_H__
#define __AUDIOPLAYER_H__

#include <functional>
#include <FS.h>

#define AUDIO_ENDED_CALLBACK_TYPE std::function<void()>

class AudioPlayer {

    public:
        void setup();
        void play(const char* filename);
        void loop();

        bool isPlaying();
};
#endif // __AUDIOPLAYER_H__