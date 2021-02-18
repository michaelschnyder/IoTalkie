#ifndef AzureIoTMqttClient_h
#define AzureIoTMqttClient_h

#include <stdint.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include <Log4Esp.h>

#include <PubSubClient.h>

#define ONCOMMAND_CALLBACK_SIGNATURE std::function<void(String, JsonObject&)> onCommandCallback
#define DESIREDPROPERTYCHANGE_CALLBACK_SIGNATURE std::function<void(JsonObject&, int)> onDesiredPropertyChangeCallback

class AzureIoTMqttClient {
private:
    
    String hubName;

    String token; 

    String deviceId;
    
    WiFiClientSecure wifiClient;
    PubSubClient client;
    
    ONCOMMAND_CALLBACK_SIGNATURE;
    DESIREDPROPERTYCHANGE_CALLBACK_SIGNATURE;
    // void loadCACert();
    boolean connect();
    
    log4Esp::Logger logger = log4Esp::Logger("AzureIoTMqttClient");
    void report(PubSubClient &client, log4Esp::Logger &logger, String path);
    void reconnectIfNecessary();
    bool handleReportedPropertyUpdateResponse(String topic);
    bool handleDesiredPropertiesUpdate(String topic, char *payload, unsigned int length);
    bool handleCloudToDeviceCommand(String topic, char *payload, unsigned int length);

public:
    AzureIoTMqttClient();   
    void callback(char*, uint8_t*, unsigned int);
    
    void setup(const char* hubName, const char* deviceId, const char* token);
    
    void onCommand(ONCOMMAND_CALLBACK_SIGNATURE);
    void onDesiredPropertyChange(DESIREDPROPERTYCHANGE_CALLBACK_SIGNATURE);
	void loop();
    void send(JsonObject& data);
    void report(String, int);
    void report(String, float);
    void report(String, String);
    void report(JsonObject&);
};

#endif
