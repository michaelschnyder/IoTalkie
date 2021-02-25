#ifndef __AUDIORECORDER_H__
#define __AUDIORECORDER_H__

#include <Log4Esp.h>

#include <functional>
#include <FS.h>

#include "driver/i2s.h"

#define I2S_READ_CHUNK_SIZE (2 * 1024) // How much to allocate for the read buffer to copy from DMA
#define I2S_DMA_BUFFER_COUNT (32) // relates somehow to resolution
#define I2S_DMA_BUFFER_SIZE (128) // maximum: 1024

#define WAVE_FORMAT_HEADER_SIZE 44

class AudioRecorder {

    log4Esp::Logger logger = log4Esp::Logger("AudioRecorder");
    
    int serialClockPin;
    int wordSelectPin;
    int dataPin;
    
    i2s_port_t port = I2S_NUM_1;
    int samplingRate = 16000; // 16KHz
    int samplingBits = 32;
    int numberOfChannels = 1;
    unsigned int bytesPerSecond;

    i2s_config_t driverConfig;
    i2s_pin_config_t pinConfig;

    bool isRecording = false;
    bool hasStopped = false;
    File targetFile;

    void writeHeader(File*, int);
    void updateHeader(File*, int);
    void fillHeader(byte* header, int wavSize);

    public:
        AudioRecorder(int sckPin, int wsPin, int dataPin);
        void setup();
        void record(File*);
        void _recordInternal(File*);
        long stop();
        long duration();
};
#endif // __AUDIORECORDER_H__