#ifndef __APPLICATION_H__
#define __APPLICATION_H__

#include <Log4Esp.h>
#include <AzureIoTMqttClient.h>
#include <FunctionFSM.h>

#include "core/Startup.h"
#include "core/Shutdown.h"
#include "core/DeviceConfig.h"
#include "core/TimeService.h"

#include "core/Settings.h"
#include "core/HealthReporter.h"
#include "core/FirmwareUpdater.h"

#include "ui/UserInterface.h"

#include "mailbox/Contacts.h"
#include "mailbox/Mailbox.h"

#include "audio/AudioRecorder.h"
#include "audio/AudioPlayerBase.h"

class Application
{
    log4Esp::Logger logger = log4Esp::Logger("Application");

    enum Event {
        SYSTEM_READY,
        SYSTEM_SHUTDOWN,
        DOWNLOAD_FIRMWARE,
        UPDATE_FAILED,
        
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
    TimeService timeService;

    HealthReporter healthReporter;
    FirmwareUpdater updater;
    AzureIoTMqttClient client;
    UserInterface *ui;
    AudioRecorder *recorder;
    TaskHTTPImpl taskHttp;
    
    Contacts contacts;
    Mailbox mailbox;
    AudioPlayerBase *player;
    
    Startup startup;
    Shutdown shutdown;

    FunctionState state_startup;
    void whileStarting();
    void afterStarting();

    FunctionState state_shutdown;
    void whileShuttingDown();

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

    FunctionState state_downloadFirmware;
    void whileDownloadingFirmware();
    String pendingFirmwareDownloadUrl;

    FunctionFsm fsm;

    bool isAppRunning = false;
    
    MessageRecord* currentMessage;

    void dispatchCloudCommand(String, JsonObject&);
    void connectionStatusChangeHandler(AzIoTConnStatus);
    void showNewMessageFrom(Contact*);
    
public:
    Application(UserInterface*, AudioRecorder*, AudioPlayerBase*);

    void setup();
    void run();
};

#endif // __APPLICATION_H__