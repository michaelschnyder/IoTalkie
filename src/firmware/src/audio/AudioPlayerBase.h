#ifndef __AUDIOPLAYERBASE_H__
#define __AUDIOPLAYERBASE_H__

class AudioPlayerBase {

public:
    virtual void setup();
    virtual void play(const char* filename);
    virtual void stop();
    virtual void loop();
    virtual void setGain(float gain);
    virtual bool isPlaying();
    virtual const char* getFilename();

};
#endif // __AUDIOPLAYERBASE_H__