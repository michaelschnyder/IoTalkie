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
#include "Inbox/Contacts.h"
#include "Inbox/Inbox.h"

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
        MESSAGE_FOUND,
        MESSAGE_NOTFOUND,
        MESSAGE_PLAYED,
        SEND_MESSAGE,
        DISCARD_MESSAGE,
        MESSAGE_SENT,
    };

    DeviceConfig config;
    Settings settings;

    AzureIoTMqttClient client;
    UserInterface *ui;
    AudioRecorder *recorder;
    FileUploader* uploader;

    Contacts contacts;
    Inbox inbox;
    AudioPlayer *player;
    
    FunctionState state_startup;
    FunctionState state_idle;
    FunctionState state_record1;
    FunctionState state_record2;
    FunctionState state_record3;
    FunctionState state_validate;
    FunctionState state_send;

    FunctionState state_tryPlay1;
    FunctionState state_tryPlay2;
    FunctionState state_tryPlay3;
    FunctionState state_play;

    FunctionFsm fsm;

    void whileStarting();
    
    void whenIdle();
    void whileIdling();

    void recordMessageFor(int buttonId);
    void whileMessageRecording();
    void validateRecording();
    void sendLastMessage();
    void whileMessageSending();

    void tryPlayMessageFrom(int buttonId);
    void whileMessagePlaying();
    void messagePlayingEnded();

    void dispatchCloudCommand(String, JsonObject&);
    void showNewMessageFrom(Contact*);

    Application() : inbox(&contacts),
    
                    state_startup(nullptr, [this]() { whileStarting(); }, nullptr),
                    state_idle([this]() { whenIdle(); }, [this]() { whileIdling(); }, nullptr),

                    state_record1([this]() { recordMessageFor(1); }, [this]() { whileMessageRecording(); }, nullptr),
                    state_record2([this]() { recordMessageFor(2); }, [this]() { whileMessageRecording(); }, nullptr),
                    state_record3([this]() { recordMessageFor(3); }, [this]() { whileMessageRecording(); }, nullptr),
                    state_validate(nullptr, [this]() { validateRecording(); }, nullptr),
                    state_send([this]() { sendLastMessage(); }, [this]() { whileMessageSending(); }, [this]() {}),

                    state_tryPlay1(nullptr, [this]() { tryPlayMessageFrom(1); }, nullptr),
                    state_tryPlay2(nullptr, [this]() { tryPlayMessageFrom(2); }, nullptr),
                    state_tryPlay3(nullptr, [this]() { tryPlayMessageFrom(3); }, nullptr),
                    state_play(nullptr, [this]() { whileMessagePlaying(); }, [this]() { messagePlayingEnded(); }),

                    fsm(&state_startup)
    {
    }

public:
    Application(UserInterface*, AudioRecorder*, AudioPlayer*, FileUploader*);

    void start();
    void run();
};

#endif // __APPLICATION_H__