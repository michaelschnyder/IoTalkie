#include "Settings.h"

bool Settings::load() {

    logger.verbose(F("Attempting to load settings from '%s'"), filename.c_str());

    if (!SD.exists(filename)) {
        
        logger.warning(F("Settings file '%s' does not exist."), filename.c_str());
        return false;
    }

    File jsonFile;
    jsonFile = SD.open(filename, "r");

    if (!jsonFile) {
        logger.error(F("Cannot open settings file '%s'"), filename.c_str());
        return false;
    }

    // Allocate a buffer to store contents of the file.
    StaticJsonBuffer<512> jsonBuffer;
    JsonObject &root = jsonBuffer.parseObject(jsonFile);
    jsonFile.close();

    if (!root.success()) {
        logger.error(F("failed to load settings."));
        return false;
    }

    strcpy(wifiSSID, root["wifiSSID"]);
    strcpy(wifiKey, root["wifiKey"]);

    logger.trace(F("Application settings loaded."));
    return true;
}

bool Settings::update(String& content) {

    StaticJsonBuffer<512> jsonBuffer;
    JsonObject &root = jsonBuffer.parseObject(content);

    if (root.containsKey("wifiSSID") && root.containsKey("wifiKey")) {
        File jsonFile = SD.open(filename, "w");
        jsonFile.print(content);
        jsonFile.close();

        logger.trace(F("Settings updated."));

        load();

        return true;
    } 

    logger.error(F("Configuration update failed!"));
    return false;
}

String Settings::getWifiSSID() {
    return wifiSSID;
}

String Settings::getWifiKey() {
    return wifiKey;
}