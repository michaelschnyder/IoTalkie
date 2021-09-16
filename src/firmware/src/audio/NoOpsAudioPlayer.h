#ifndef __NOOPSAUDIOPLAYER_H__
#define __NOOPSAUDIOPLAYER_H__

#include "AudioPlayerBase.h"

class NoOpsAudioPlayer : public AudioPlayerBase {

    public:
        NoOpsAudioPlayer(int clockPin, int lrcPin, int dataPin) {};

        void setup() {};
        void play(const char* filename) {};
        void stop() {};
        void loop() {};
        void setGain(float gain) {};
        bool isPlaying() { return false; };
        const char* getFilename() { return ""; };

};
#endif // __NOOPSAUDIOPLAYER_H__