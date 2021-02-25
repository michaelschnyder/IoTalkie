#ifndef __APPLICATION_H__
#define __APPLICATION_H__

#include <Log4Esp.h>
#include <AzureIoTMqttClient.h>

#include "FunctionFsm.h"
#include "UserInterface.h"
#include "audio/AudioRecorder.h"
#include "audio/AudioPlayer.h"
#include "core/DeviceConfig.h"
#include "core/Settings.h"
#include "inbox/Contacts.h"
#include "inbox/Inbox.h"
#include "http/FileUploader.h"

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
        DOWNLOAD_MESSAGE,
        MESSAGE_DOWNLOADED
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
    void whileStarting();
    void afterStarting();

    FunctionState state_idle;
    void beforeIdling();
    void whileIdling();

    FunctionState state_record1;
    FunctionState state_record2;
    FunctionState state_record3;
    void recordMessageFor(int buttonId);
    void whileMessageRecording();

    FunctionState state_validate;
    void validateRecording();

    FunctionState state_send;
    void sendLastMessage();
    void whileMessageSending();

    FunctionState state_tryPlay1;
    FunctionState state_tryPlay2;
    FunctionState state_tryPlay3;
    void tryPlayMessageFrom(int buttonId);

    FunctionState state_play;
    void whileMessagePlaying();
    void messagePlayingEnded();

    FunctionState state_receiveMessage;
    void whileReceivingMessage();

    FunctionFsm fsm;

    void dispatchCloudCommand(String, JsonObject&);
    void showNewMessageFrom(Contact*);

    Application() : inbox(&contacts),
    
                    state_startup(nullptr, [this]() { whileStarting(); }, [this]() { afterStarting(); }),
                    state_idle([this]() { beforeIdling(); }, [this]() { whileIdling(); }, nullptr),

                    state_record1([this]() { recordMessageFor(1); }, [this]() { whileMessageRecording(); }, nullptr),
                    state_record2([this]() { recordMessageFor(2); }, [this]() { whileMessageRecording(); }, nullptr),
                    state_record3([this]() { recordMessageFor(3); }, [this]() { whileMessageRecording(); }, nullptr),
                    state_validate(nullptr, [this]() { validateRecording(); }, nullptr),
                    state_send([this]() { sendLastMessage(); }, [this]() { whileMessageSending(); }, [this]() {}),

                    state_tryPlay1(nullptr, [this]() { tryPlayMessageFrom(1); }, nullptr),
                    state_tryPlay2(nullptr, [this]() { tryPlayMessageFrom(2); }, nullptr),
                    state_tryPlay3(nullptr, [this]() { tryPlayMessageFrom(3); }, nullptr),
                    state_play(nullptr, [this]() { whileMessagePlaying(); }, [this]() { messagePlayingEnded(); }),

                    state_receiveMessage(nullptr, [this]() { whileReceivingMessage(); }, nullptr),

                    fsm(&state_startup)
    {
        this->fsm.add_transition(&state_startup, &state_idle, SYSTEM_READY, nullptr);

        this->fsm.add_transition(&state_idle, &state_record1, BUTTON1_LONGSTART, nullptr);
        this->fsm.add_transition(&state_idle, &state_record2, BUTTON2_LONGSTART, nullptr);
        this->fsm.add_transition(&state_idle, &state_record3, BUTTON3_LONGSTART, nullptr);
        this->fsm.add_transition(&state_record1, &state_validate, BUTTON1_LONG_RELEASE, nullptr);
        this->fsm.add_transition(&state_record2, &state_validate, BUTTON2_LONG_RELEASE, nullptr);
        this->fsm.add_transition(&state_record3, &state_validate, BUTTON3_LONG_RELEASE, nullptr);
        this->fsm.add_transition(&state_record1, &state_validate, RECORDING_LENGTH_EXCEEDED, nullptr);
        this->fsm.add_transition(&state_record2, &state_validate, RECORDING_LENGTH_EXCEEDED, nullptr);
        this->fsm.add_transition(&state_record3, &state_validate, RECORDING_LENGTH_EXCEEDED, nullptr);
        this->fsm.add_transition(&state_validate, &state_send, SEND_MESSAGE, nullptr);
        this->fsm.add_transition(&state_send, &state_idle, MESSAGE_SENT, nullptr);
        this->fsm.add_transition(&state_validate, &state_idle, DISCARD_MESSAGE, nullptr);

        this->fsm.add_transition(&state_idle, &state_tryPlay1, BUTTON1_CLICK, nullptr);
        this->fsm.add_transition(&state_idle, &state_tryPlay2, BUTTON2_CLICK, nullptr);
        this->fsm.add_transition(&state_idle, &state_tryPlay3, BUTTON3_CLICK, nullptr);
        this->fsm.add_transition(&state_tryPlay1, &state_idle, MESSAGE_NOTFOUND, nullptr);
        this->fsm.add_transition(&state_tryPlay2, &state_idle, MESSAGE_NOTFOUND, nullptr);
        this->fsm.add_transition(&state_tryPlay3, &state_idle, MESSAGE_NOTFOUND, nullptr);
        this->fsm.add_transition(&state_tryPlay1, &state_play, MESSAGE_FOUND, nullptr);
        this->fsm.add_transition(&state_tryPlay2, &state_play, MESSAGE_FOUND, nullptr);
        this->fsm.add_transition(&state_tryPlay3, &state_play, MESSAGE_FOUND, nullptr);
        this->fsm.add_transition(&state_play, &state_idle, MESSAGE_PLAYED, nullptr);
        this->fsm.add_transition(&state_play, &state_idle, BUTTON1_CLICK, nullptr);
        this->fsm.add_transition(&state_play, &state_idle, BUTTON2_CLICK, nullptr);
        this->fsm.add_transition(&state_play, &state_idle, BUTTON3_CLICK, nullptr);

        this->fsm.add_transition(&state_idle, &state_receiveMessage, DOWNLOAD_MESSAGE, nullptr);
        this->fsm.add_transition(&state_receiveMessage, &state_idle, MESSAGE_DOWNLOADED, nullptr);
    }

public:
    Application(UserInterface*, AudioRecorder*, AudioPlayer*, FileUploader*);

    void start();
    void run();
};

#endif // __APPLICATION_H__