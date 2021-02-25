#ifndef AzureIoTMqttClient_h
#define AzureIoTMqttClient_h

#include <stdint.h>
#include <functional>

#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include <Log4Esp.h>

#include <PubSubClient.h>

#define AZIOT_DOMAIN_TEMPLATE ("%s.azure-devices.net") // %s = IoTHub Name
#define AZIOT_MQTT_PORT 8883

#define AZIOT_USER_TEMPLATE ("%s/%s/?api-version=2018-06-30") // %s = Domain, %s = deviceId
#define AZIOT_PASSWORD_VALID_PREFIX "SharedAccessSignature sr="
#define AZIOT_MESSAGING_INBOUND_TOPIC_TMPL ("devices/%s/messages/devicebound/#") //%s = deviceId
#define AZIOT_MESSAGING_OUTBOUND_TOPIC_TMPL ("devices/%s/messages/events/") //%s = deviceId

#define AZIOT_REPORT_PROPERTY_OUTBOUND_TOPIC_TMPL "$iothub/twin/PATCH/properties/reported/?$rid=%d"
#define AZIOT_REPORT_PROPERTY_CONFIRM_TOPIC_SUBSCRIBE "$iothub/twin/res/#"
#define AZIOT_REPORT_PROPERTY_CONFIRM_TOPIC_PREFIX    "$iothub/twin/res"
#define AZIOT_REPORT_PROPERTY_CONFIRM_CODE_SUCCESS 204

#define AZIOT_DESIRED_PROPERY_INBOUND_TOPIC_SUBSCROBE "$iothub/twin/PATCH/properties/desired/#"
#define AZIOT_DESIRED_PROPERY_INBOUND_TOPIC_PREFIX    "$iothub/twin/PATCH/properties/desired"

#define ONCOMMAND_CALLBACK_SIGNATURE std::function<void(String, JsonObject&)> 
#define DESIREDPROPERTYCHANGE_CALLBACK_SIGNATURE std::function<void(JsonObject&, int)>

class AzureIoTMqttClient {
private:
    
    
    String mqttHostname;
    int port = AZIOT_MQTT_PORT;
    String mqttDeviceId;
    String mqttUser;
    String mqttPassword; 

    String outboundTopicName;

    boolean clientReady = false;
    boolean enableReconnect = false;
    
    long lastReconnectAttempt = 0;
    int retryTimoutInMs = 5000;

    WiFiClientSecure wifiClient;
    PubSubClient mqttClient;
    
    ONCOMMAND_CALLBACK_SIGNATURE onCommandCallback;
    DESIREDPROPERTYCHANGE_CALLBACK_SIGNATURE onDesiredPropertyChangeCallback;
    
    log4Esp::Logger logger = log4Esp::Logger("AzureIoTMqttClient");

    String describeConnectionState(int);
    void report(PubSubClient &client, log4Esp::Logger &logger, String path);
    boolean connectInternal();
    void reconnectIfNecessary();
    bool handleReportedPropertyUpdateResponse(String topic);
    bool handleDesiredPropertiesUpdate(String topic, char *payload, unsigned int length);
    bool handleCloudToDeviceCommand(String topic, char *payload, unsigned int length);

public:
    AzureIoTMqttClient();   
    void callback(char*, uint8_t*, unsigned int);
    
    void connect(const char* hubName, const char* deviceId, const char* token);
    
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
