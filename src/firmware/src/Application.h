#ifndef __APPLICATION_H__
#define __APPLICATION_H__

#include <Log4Esp.h>
#include <AzureIoTMqttClient.h>

#include "FunctionFsm.h"
#include "UserInterface.h"
#include "AudioRecorder.h"
#include "AudioPlayer.h"
#include "DeviceConfig.h"
#include "Settings.h"

#include "FileUploader.h"
class Application
{
    log4Esp::Logger logger = log4Esp::Logger("Application");

    enum Event {
        SYSTEM_READY,
        BUTTON1_CLICK,
        BUTTON2_CLICK,
        BUTTON3_CLICK,
        BUTTON1_LONGSTART,
        BUTTON1_LONG_RELEASE,
        BUTTON2_LONGSTART,
        BUTTON2_LONG_RELEASE,
        BUTTON3_LONGSTART,
        BUTTON3_LONG_RELEASE,
        RECORDING_LENGTH_EXCEEDED,
        MESSAGE_PLAYED,
        SEND_MESSAGE,
        DISCARD_MESSAGE,
        MESSAGE_SENT,
    };

    DeviceConfig config;
    Settings settings;
    AzureIoTMqttClient client;
    UserInterface *ui;
    AudioRecorder *recoder;
    FileUploader* uploader;

    AudioPlayer *player;
    
    FunctionState state_startup;
    FunctionState state_idle;
    FunctionState state_record1;
    FunctionState state_record2;
    FunctionState state_record3;
    FunctionState state_validate;
    FunctionState state_send;

    FunctionState state_play1;
    FunctionState state_play2;
    FunctionState state_play3;

    FunctionFsm fsm;

    void whileStarting();
    void whenIdle();

    void recordMessageFor(int buttonId);
    void whileMessageRecording();
    void validateRecording();
    void sendLastMessage();
    void whileMessageSending();

    void playMessageFrom(int buttonId);
    void whileMessagePlaying();

    Application() : state_startup(nullptr, [this]() { whileStarting(); }, nullptr),
                    state_idle([this]() { whenIdle(); }, nullptr, nullptr),

                    state_record1([this]() { recordMessageFor(1); }, [this]() { whileMessageRecording(); }, nullptr),
                    state_record2([this]() { recordMessageFor(2); }, [this]() { whileMessageRecording(); }, nullptr),
                    state_record3([this]() { recordMessageFor(3); }, [this]() { whileMessageRecording(); }, nullptr),
                    state_validate(nullptr, [this]() { validateRecording(); }, nullptr),
                    state_send([this]() { sendLastMessage(); }, [this]() { whileMessageSending(); }, [this]() {}),

                    state_play1([this]() { playMessageFrom(1); }, [this]() { whileMessagePlaying(); }, nullptr),
                    state_play2([this]() { playMessageFrom(2); }, [this]() { whileMessagePlaying(); }, nullptr),
                    state_play3([this]() { playMessageFrom(3); }, [this]() { whileMessagePlaying(); }, nullptr),

                    fsm(&state_startup)
    {
    }

public:
    Application(UserInterface*, AudioRecorder*, AudioPlayer*, FileUploader*);

    void start();
    void run();
};

#endif // __APPLICATION_H__