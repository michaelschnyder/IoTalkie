#ifndef __NOOPSAUDIOPLAYER_H__
#define __NOOPSAUDIOPLAYER_H__

#include <string.h>
#include "AudioPlayerBase.h"
class NoOpsAudioPlayer : public AudioPlayerBase {
    
    char filename[128] = "";

    public:
        NoOpsAudioPlayer(int clockPin, int lrcPin, int dataPin) {};

        void setup() {};
        void play(const char* filename) { strncpy(this->filename, filename, sizeof(this->filename) - 1); };
        void stop() {};
        void loop() {};
        void setGain(float gain) {};
        bool isPlaying() { return false; };
        const char* getFilename() { return filename; };

};
#endif // __NOOPSAUDIOPLAYER_H__