#ifndef AppConfig_h
#define AppConfig_h

#include <stdint.h>
#include <FS.h>
#include <ArduinoJson.h>
#include <Log4Esp.h>

class DeviceConfig {
private:
    const String filename = "/config.json";

    // Name max lenght: 50 (according to azure)
    char azIoTHubName[51];

    // Unknown max lenght, taking 256
    char azIoTSASToken[257]; 

    char deviceId[33];

    char postMessageUrl[257];

    log4Esp::Logger logger = log4Esp::Logger("DeviceConfig");
public:
    bool load();
    bool update(String& content);

    String getAzIoTHubName();
    String getAzIoTSASToken();
    String getDeviceId();
    String getPostMessageUrl();
};

#endif