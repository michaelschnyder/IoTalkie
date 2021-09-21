#ifndef __HEALTHREPORTER_H__
#define __HEALTHREPORTER_H__

#include <AzureIoTMqttClient.h>
#include "core/TimeService.h"
#include "core/BuildInfo.h"
#include "core/Diagnostics.h"

#define SEND_STATS_INTERVAL 300 * 1000
#define PRINT_STATS_INTERVAL 10 * 1000

    static const char HELLO_TEMPLATE[] PROGMEM = 
    "{"
        "\"type\": \"ClientHello\","
        "\"fwVersion\": \"%s\","
        
        "\"startTime\": %lu,"
        "\"uptime\": %lu,"

        "\"chipModel\": \"%s\","
        "\"chipRevision\": %i,"
        "\"cpuFreqMhz\": %i,"
        
        "\"heapSize\": %u,"
        "\"heapFreeSize\": %u,"
        "\"heapLargestFreeBlockSize\": %u,"

        "\"spiffsTotal\": %llu,"
        "\"spiffsUsed\": %llu,"
        "\"spiffFree\": %llu,"

        "\"sdSize\": %llu,"
        "\"sdTotal\": %llu,"
        "\"sdUsed\": %llu,"
        "\"sdFree\": %llu,"

        "\"wifiSSID\": \"%s\","
        "\"wifiRSSI\": %i,"
        "\"wifiIP\": \"%s\""
    "}";

    static const char STATS_TEMPLATE[] PROGMEM = 
    "{"
        "\"type\": \"DiagMessage\","
        
        "\"uptime\": %lu,"
        "\"heapFreeSize\": %u,"
        "\"heapLargestFreeBlockSize\": %u,"
        "\"spiffsUsed\": %llu,"
        "\"spiffFree\": %llu,"
        "\"sdUsed\": %llu,"
        "\"sdFree\": %llu,"
        "\"wifiRSSI\": %i"
    "}";

class HealthReporter {

private:
    AzureIoTMqttClient* client;
    TimeService* timeService;
    long lastPrint, lastSent;
    void sendStats();
    void printStats();

public:
    void setup(AzureIoTMqttClient*, TimeService*);
    void loop();

    void sayHello();
};

#endif // __HEALTHREPORTER_H__