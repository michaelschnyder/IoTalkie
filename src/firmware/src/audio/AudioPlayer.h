#ifndef __AUDIOPLAYER_H__
#define __AUDIOPLAYER_H__

#include "hardware.h"

#include <functional>
#include <FS.h>
#include <Log4Esp.h>

#include <AudioFileSourcePROGMEM.h>
#include <AudioFileSourceSD.h>

#include <AudioGeneratorMP3.h>
#include <AudioGeneratorWAV.h>
#include <AudioGeneratorOpus.h>

#include <AudioOutputI2S.h>

#include <WiFiClient.h>
#include <SD.h>

#include "AudioPlayerBase.h"
#include "file/FileInfo.h"

const unsigned char emptyMp3[] PROGMEM = {
    0x49, 0x44, 0x33, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x50, 0x54, 0x45, 0x4E, 0x43, 0x00, 0x00,
    0x00, 0x15, 0x20, 0x00, 0x00, 0x53, 0x6F, 0x75, 0x6E, 0x64, 0x20, 0x47, 0x72, 0x69, 0x6E, 0x64,
    0x65, 0x72, 0x20, 0x34, 0x2E, 0x30, 0x2E, 0x33, 0x00, 0x41, 0x50, 0x49, 0x43, 0x00, 0x00, 0x00,
    0x04, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x54, 0x43, 0x4F, 0x50, 0x00, 0x00, 0x00, 0x19, 0x20,
    0x00, 0x00, 0x43, 0x6F, 0x70, 0x79, 0x72, 0x69, 0x67, 0x68, 0x74, 0x20, 0x41, 0x6C, 0x61, 0x6E,
    0x20, 0x4D, 0x63, 0x4B, 0x69, 0x6E, 0x6E, 0x65, 0x79, 0x00, 0xFF, 0xFB, 0x94, 0x68, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x58, 0x69,
    0x6E, 0x67, 0x00, 0x00, 0x00, 0x0F};

class AudioPlayer : public AudioPlayerBase {

    AudioFileSourcePROGMEM *progmemSoundSource;
    AudioFileSourceSD *sdCardSoundSource;
    AudioGenerator *currentAudioGenerator;

    AudioGeneratorMP3 *audioGeneratorMp3;
    AudioGeneratorOpus *audioGeneratorOpus;
    AudioGeneratorWAV *audioGeneratorWAV;

    AudioOutputI2S *out;

    bool playing = false;
    log4Esp::Logger logger = log4Esp::Logger("AudioPlayer");
    
    char filename[128] = "";

    int clockPin;
    int lrcPin;
    int dataPin;

    public:
        AudioPlayer(int clockPin, int lrcPin, int dataPin);
        void setup();
        void play(const char* filename);
        void stop();
        void loop();
        void setGain(float gain);
        bool isPlaying();
        const char* getFilename();
};
#endif // __AUDIOPLAYER_H__
