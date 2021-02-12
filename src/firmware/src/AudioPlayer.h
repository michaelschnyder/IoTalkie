#ifndef __AUDIOPLAYER_H__
#define __AUDIOPLAYER_H__

#include <functional>
#include <FS.h>

#define AUDIO_ENDED_CALLBACK_TYPE std::function<void()>

class AudioPlayer {

    public:
        void setup();
        void play(File* file);
        void loop();

        AUDIO_ENDED_CALLBACK_TYPE audioEndedCallback;

        void onButtonEvent(AUDIO_ENDED_CALLBACK_TYPE callback) {
            this->audioEndedCallback = callback;    
        }

};
#endif // __AUDIOPLAYER_H__