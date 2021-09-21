#ifndef __TIMESERVICE_H__
#define __TIMESERVICE_H__

#include <WiFiUdp.h>
#include <NTPClient.h>

class TimeService {

private:
    WiFiUDP ntpUDP;
    NTPClient timeClient;
    bool hasStarted = false;
public:
    TimeService() :
        timeClient(ntpUDP)
        {};

    void setup() {
        timeClient.begin();
        hasStarted = true;
    };

    void loop() {
        if (!hasStarted) return;

        timeClient.update();
    }

    void update() {
        timeClient.forceUpdate();
    };

    unsigned long getTimestamp() {
        if (!hasStarted) return -1;
        return timeClient.getEpochTime();
    };
};
#endif // __TIMESERVICE_H__