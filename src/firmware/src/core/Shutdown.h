#ifndef __SHUTDOWN_H__
#define __SHUTDOWN_H__

#include "FunctionFsm.h"
#include <Log4Esp.h>

#include "../UserInterface.h"
#include <AzureIoTMqttClient.h>
#include "pins.h"
#include "SD.h"
#include "SPIFFS.h"

class Shutdown {

private:

    enum Event {
        Continue
    };

    log4Esp::Logger logger = log4Esp::Logger("Shutdown");

    UserInterface* ui;
    AzureIoTMqttClient* client;

    void initShutdown();
    FunctionState state_initShutdown;

    FunctionState state_issueFarewell;
    void issueFarewell();

    FunctionState state_stopWifi;
    void stopWifi();

    FunctionState state_unmountSdCard;
    void unmountSdCard();

    FunctionState state_unmountSPIIFS;
    void unmountSPIIFS();

    FunctionState state_reset;
    void reset();

    FunctionFsm fsm;
public:
    Shutdown(UserInterface* userInterface, AzureIoTMqttClient* azClient);
    void run();
};

#endif // __SHUTDOWN_H__