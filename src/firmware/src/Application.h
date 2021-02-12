#ifndef __APPLICATION_H__
#define __APPLICATION_H__

#include "FunctionFsm.h"
#include "UserInterface.h"
#include "AudioRecorder.h"
#include "AudioPlayer.h"

class Application
{

    enum Event {
        BUTTON1_CLICK,
        BUTTON2_CLICK,
        BUTTON3_CLICK,
        BUTTON1_LONGSTART,
        BUTTON1_LONG_RELEASE,
        BUTTON2_LONGSTART,
        BUTTON2_LONG_RELEASE,
        BUTTON3_LONGSTART,
        BUTTON3_LONG_RELEASE,
        MESSAGE_PLAYED
    };

    UserInterface *ui;
    AudioRecorder *recoder;
    AudioPlayer *player;
    
    FunctionState state_idle;
    FunctionState state_record1;
    FunctionState state_record2;
    FunctionState state_record3;
    FunctionState state_play1;
    FunctionState state_play2;
    FunctionState state_play3;

    FunctionFsm fsm;

    void recordMessageFor(int buttonId);
    void completeRecording();
    void whileMessageRecording();
    
    void playMessageFrom(int buttonId);
    void whileMessagePlaying();

    Application() : state_idle([this]() { Serial.println("Enter Idle"); }, [this]() {}, [this]() {}),
                    state_record1([this]() { recordMessageFor(1); }, [this]() { whileMessageRecording(); }, [this]() { completeRecording(); }),
                    state_record2([this]() { recordMessageFor(2); }, [this]() { whileMessageRecording(); }, [this]() { completeRecording(); }),
                    state_record3([this]() { recordMessageFor(3); }, [this]() { whileMessageRecording(); }, [this]() { completeRecording(); }),

                    state_play1([this]() { playMessageFrom(1); }, [this]() { whileMessagePlaying(); }, [this]() {}),
                    state_play2([this]() { playMessageFrom(2); }, [this]() { whileMessagePlaying(); }, [this]() {}),
                    state_play3([this]() { playMessageFrom(3); }, [this]() { whileMessagePlaying(); }, [this]() {}),

                    fsm(&state_idle)
    {
    }

public:
    Application(UserInterface*, AudioRecorder*, AudioPlayer*);

    void run();
};

#endif // __APPLICATION_H__