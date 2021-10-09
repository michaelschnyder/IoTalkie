#ifndef Settings_h
#define Settings_h

#include <stdint.h>
#include <SD.h>
#include <ArduinoJson.h>
#include <Log4Esp.h>

class Settings {
private:
    const String filename = "/settings.jsn";

    char wifiSSID[33];
    char wifiKey[33]; 

    log4Esp::Logger logger = log4Esp::Logger("Settings");
public:
    bool load();
    bool update(String& content);

    String getWifiSSID();
    String getWifiKey();
};

#endif