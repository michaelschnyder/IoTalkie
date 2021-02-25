#include "DeviceConfig.h"

bool DeviceConfig::load() {

    logger.verbose(F("Attempting to load configuration from '%s'"), filename.c_str());

    if (!SPIFFS.exists(filename)) {
        
        logger.warning(F("Configuration file '%s' does not exist."), filename.c_str());
        return false;
    }

    File jsonFile = SPIFFS.open(DeviceConfig::filename, "r");

    if (!jsonFile) {
        logger.error(F("Cannot open configuration file '%s'"), filename.c_str());
        return false;
    }

    // Allocate a buffer to store contents of the file.
    StaticJsonBuffer<512> jsonBuffer;
    JsonObject &root = jsonBuffer.parseObject(jsonFile);
    jsonFile.close();

    if (!root.success()) {
        logger.error(F("failed to load application configuration."));
        return false;
    }

    strcpy(azIoTHubName, root["azIoTHubName"]);
    strcpy(azIoTSASToken, root["azIoTSASToken"]);
    strcpy(deviceId, root["deviceId"]);
    strcpy(postMessageUrl, root["postMessageUrl"]);

    logger.trace(F("Application configuration loaded."));
    return true;
}

String DeviceConfig::getAzIoTHubName() {
    return azIoTHubName;
}

String DeviceConfig::getAzIoTSASToken() {
    return azIoTSASToken;
}

String DeviceConfig::getDeviceId() 
{
    return deviceId;
}

String DeviceConfig::getPostMessageUrl() 
{
    return postMessageUrl;
}