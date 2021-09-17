#ifndef __HEALTHREPORTER_H__
#define __HEALTHREPORTER_H__

#include <AzureIoTMqttClient.h>
#include "core/BuildInfo.h"
#include "core/Diagnostics.h"

#define SEND_STATS_INTERVAL 300 * 1000
#define PRINT_STATS_INTERVAL 10 * 1000

class HealthReporter {

private:
    AzureIoTMqttClient* client;
    long lastPrint, lastSent;

public:
    void setup(AzureIoTMqttClient*);
    void loop();

    void sendStats();
    void sayHello();
    void printStats();
};

#endif // __HEALTHREPORTER_H__