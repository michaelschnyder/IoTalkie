#include "AzureIoTMqttClient.h"

AzureIoTMqttClient::AzureIoTMqttClient() { 
  
  mqttClient.setClient(AzureIoTMqttClient::wifiClient);
  mqttClient.setBufferSize(512);

  AzureIoTMqttClient::wifiClient.setInsecure();

  // Required to make signature of member function (that comes with a implict *this) match
  // with expected signature. See: https://stackoverflow.com/a/46489820
  mqttClient.setCallback([this](char* a, uint8_t* b, unsigned int c) { this->callback(a, b, c); });
}

void AzureIoTMqttClient::connect(const char* hubName, const char* deviceId, const char* token) {
  
  setStatus(INITIALIZING);

  char buff[256];

  // Host
  sprintf(buff, AZIOT_DOMAIN_TEMPLATE, hubName);
  this->mqttHostname = buff;

  // Device Id
  this->mqttDeviceId = deviceId;

  // User
  sprintf(buff, AZIOT_USER_TEMPLATE, mqttHostname.c_str(), this->mqttDeviceId.c_str());
  this->mqttUser = buff;

  // Password
  this->mqttPassword = token;

  if (!this->mqttPassword.startsWith(AZIOT_PASSWORD_VALID_PREFIX)) {
    logger.error(F("Invalid Azure IoT Hub SharedAccessSignature detected. Please check your configuration."));
  }

  int maxAttempts = 5;

  for(int i = 0; i < maxAttempts; i++) {
    if(this->connectInternal()) {
      clientReady = true;
      return;
    }

    if (i+1 < maxAttempts) {
      logger.warning(F("Unable to connect to MQTT Server in attempt %d/%d. Retry in %ds..."), i+1, maxAttempts, retryTimoutInMs/1000);
      delay(retryTimoutInMs);
    }
  }

  logger.error(F("Initial connection to MQTT not successful. Will continue trying..."));
  lastReconnectAttempt = millis();
}

void AzureIoTMqttClient::disconnect() 
{
  enableReconnect = false;
  clientReady = false;

  this->mqttClient.disconnect();
}

void AzureIoTMqttClient::callback(char* topic, byte* payload, unsigned int length) {
  
  char* buffer = (char*)payload;
  payload[length] = '\0'; // Manually add null-termination at given lenght since PubSub is reusing the buffer;

  String topicString = String(topic);

  if (handleReportedPropertyUpdateResponse(topicString)) {
    return;
  }

  if (handleDesiredPropertiesUpdate(topicString, buffer, length)) {
    return;
  }

  if (handleCloudToDeviceCommand(topicString, buffer, length)) {
    return;
  }

  logger.warning(F("Recieved message not handled: '%s'"), buffer);
}

String AzureIoTMqttClient::describeConnectionState(int state) {
  
  switch (state) {
      case MQTT_CONNECTION_TIMEOUT:      return F("MQTT_CONNECTION_TIMEOUT");
      case MQTT_CONNECTION_LOST:         return F("MQTT_CONNECTION_LOST");
      case MQTT_CONNECT_FAILED:          return F("MQTT_CONNECT_FAILED");
      case MQTT_DISCONNECTED:            return F("MQTT_DISCONNECTED");
      case MQTT_CONNECTED:               return F("MQTT_CONNECTED");
      case MQTT_CONNECT_BAD_PROTOCOL:    return F("MQTT_CONNECT_BAD_PROTOCOL");
      case MQTT_CONNECT_BAD_CLIENT_ID:   return F("MQTT_CONNECT_BAD_CLIENT_ID");
      case MQTT_CONNECT_UNAVAILABLE:     return F("MQTT_CONNECT_UNAVAILABLE");
      case MQTT_CONNECT_BAD_CREDENTIALS: return F("MQTT_CONNECT_BAD_CREDENTIALS");
      case MQTT_CONNECT_UNAUTHORIZED:    return F("MQTT_CONNECT_UNAUTHORIZED");
      default:                           return F("UNKNOWN/OTHER");
    }
}

void AzureIoTMqttClient::setStatus(AzIoTConnStatus newStatus) {

  if (this->status != newStatus) {
    this->status = newStatus;
    if (onConnectionStatusChangeCallback != NULL) {
      onConnectionStatusChangeCallback(newStatus);
    }
  };  
}

bool AzureIoTMqttClient::connectInternal() {

  setStatus(INITIALIZING);

  logger.trace(F("Attempting to connect to MQTT server..."));
  logger.verbose(F("URL: %s:%d, BufferSize: %d"), mqttHostname.c_str(), port, mqttClient.getBufferSize());  
  
  mqttClient.setServer(mqttHostname.c_str(), port);

  logger.verbose(F("Credentials: DeviceId: %s, User: %s, Pass: %s"), mqttDeviceId.c_str(), mqttUser.c_str(), this->mqttPassword.c_str());

  if (!mqttClient.connect(mqttDeviceId.c_str(), mqttUser.c_str(), this->mqttPassword.c_str(), 0, 0, 0, 0, false)) {
    
    char lastSslError[64];
    int errorNo = wifiClient.lastError(lastSslError, 64);
    
    logger.fatal(F("Connection to MQTT failed!. Client state: %s (%d). Maybe SSL Error?: %d '%s'. Next try in 5s"), describeConnectionState(mqttClient.state()).c_str(), mqttClient.state(), errorNo, lastSslError);  
    setStatus(ERROR);

    return false;    
  }

  char inboundMessagesTopicName[255];
  char outboundMessagesTopicName[255];

  sprintf(inboundMessagesTopicName, AZIOT_MESSAGING_INBOUND_TOPIC_TMPL, this->mqttDeviceId.c_str());
  sprintf(outboundMessagesTopicName, AZIOT_MESSAGING_OUTBOUND_TOPIC_TMPL, this->mqttDeviceId.c_str());

  outboundTopicName = outboundMessagesTopicName;

  logger.trace(F("Connection established to '%s:%d'. Subscribing to topics: '%s', '%s', '%s'"), mqttHostname.c_str(), port, inboundMessagesTopicName, AZIOT_REPORT_PROPERTY_CONFIRM_TOPIC_SUBSCRIBE, AZIOT_DESIRED_PROPERY_INBOUND_TOPIC_SUBSCROBE);      
  setStatus(REGISTERING);
    
  if(!mqttClient.subscribe(inboundMessagesTopicName)) {
    logger.fatal(F("Subscribe to event topic failed"));
    return false;
  }

  if(!mqttClient.subscribe(AZIOT_REPORT_PROPERTY_CONFIRM_TOPIC_SUBSCRIBE)) {
    logger.error(F("Unable to subscribe to Reported Properties topic on '%s'"), AZIOT_REPORT_PROPERTY_CONFIRM_TOPIC_SUBSCRIBE);
    return false;
  }

  if(!mqttClient.subscribe(AZIOT_DESIRED_PROPERY_INBOUND_TOPIC_SUBSCROBE)) {
    logger.error(F("Unable to subscribe to Desired Properties topic on '%s'"), AZIOT_DESIRED_PROPERY_INBOUND_TOPIC_SUBSCROBE);
    return false;
  }

  if (onConnectionStatusChangeCallback != NULL) {
      onConnectionStatusChangeCallback(CONNECTED);
  }

  enableReconnect = true;

  setStatus(CONNECTED);
  return true;
}

void AzureIoTMqttClient::loop() {
  mqttClient.loop();
  
  if (enableReconnect) {
    reconnectIfNecessary();
  }
}

bool AzureIoTMqttClient::send(const char* message) 
{
  return mqttClient.publish(outboundTopicName.c_str(), message);
}

bool AzureIoTMqttClient::handleReportedPropertyUpdateResponse(String topic) {
  
  if (!topic.startsWith(AZIOT_REPORT_PROPERTY_CONFIRM_TOPIC_PREFIX)) {
    return false;
  }

  String responseCode = topic.substring(strlen(AZIOT_REPORT_PROPERTY_CONFIRM_TOPIC_PREFIX) + 1, strlen(AZIOT_REPORT_PROPERTY_CONFIRM_TOPIC_PREFIX) + 4);
  int ridPosStart = topic.indexOf("$rid") + 5;
  int ridPosEnd = topic.indexOf("&", ridPosStart);
  int versionPosStart = topic.indexOf("$version") + 9;
  
  String ridString = topic.substring(ridPosStart, ridPosEnd);
  String versionString = topic.substring(versionPosStart);

  int requestId = ridString.toInt();
  int version = versionString.toInt();

  if (responseCode && responseCode.toInt() == AZIOT_REPORT_PROPERTY_CONFIRM_CODE_SUCCESS) {
    logger.verbose(F("Reported property update (Request: %i) accepted by broker. Response Code: %s. New Version: %i"), requestId, responseCode.c_str(), version);
  }
  else {
    logger.warning(F("Reported Property (Request: %i) got rejected by broker. Response Code: %s"), requestId, responseCode.c_str());
  }
  
  return true;
}

bool AzureIoTMqttClient::handleDesiredPropertiesUpdate(String topic, char* payload, unsigned int length)
{
  if (topic.length() == 0) {
    return false;
  }

  if (!topic.startsWith(AZIOT_DESIRED_PROPERY_INBOUND_TOPIC_PREFIX)) {
    return false;
  }

  logger.trace(F("Desired property change update"));

  DynamicJsonBuffer jsonBuffer;
  JsonObject& jsonMessage = jsonBuffer.parseObject((char*)payload);

  if (onDesiredPropertyChangeCallback != NULL) {
    onDesiredPropertyChangeCallback(jsonMessage, 0);
  }

  return true;
}

bool AzureIoTMqttClient::handleCloudToDeviceCommand(String topic, char* payload, unsigned int length) {
  
  if (topic.length() == 0) {
    return false;
  }

  String message = String(payload);
  logger.verbose(F("New Message from Broker. Topic: '%s', Lenght: %d, Content: '%s'"), topic.c_str(), length, message.c_str());

  DynamicJsonBuffer jsonBuffer;
  JsonObject& jsonMessage = jsonBuffer.parseObject((char*)payload);

  if(!jsonMessage.success()) {
    return false;
  }

  if (!jsonMessage.containsKey("cmd")) {
    return false;
  }

  String commandName = jsonMessage["cmd"];

  if (onCommandCallback != NULL) {
    logger.trace(F("Dispatching execution of command '%s'"), commandName.c_str());
    onCommandCallback(commandName, jsonMessage);
    return true;
  }
  else {
    logger.trace(F("No callback registered for command '%s'"), commandName.c_str());
    return false;
  }
}

void AzureIoTMqttClient::reconnectIfNecessary() {

  if (clientReady && mqttClient.connected()) {
    return;
  }

  unsigned long currentMillis = millis();
  if (!(currentMillis - lastReconnectAttempt >= retryTimoutInMs * 2)) {
    return;
  }

  if(clientReady && !mqttClient.connected()) {
    // The initial state was valid, means the connection did reset
    setStatus(RECONNECTING);
    
    clientReady = false;
    logger.warning(F("MQTT client got disconnected. Trying to reconnect."));
  }

  // Start a new connection attempt
  logger.verbose(F("Starting new reconnect attempt."));
  lastReconnectAttempt = millis();
   
  if(this->connectInternal()) {
    clientReady = true;
    logger.trace(F("Successfully re-established connection MQTT Server"));
    return;
  }

  logger.error(F("Re-establishing connection and initializing client failed."));
}

void AzureIoTMqttClient::send(JsonObject& data) {
  char buffer[512];
  data.printTo(buffer);

  if(!mqttClient.publish(outboundTopicName.c_str(), buffer)) {
    logger.error(F("Unable to publish message '%s'"), buffer);
    }
}

void AzureIoTMqttClient::report(String propertyName, int value) {
  String patch = "{\"" + propertyName + "\": " + value + "}";
  report(mqttClient, logger, patch);  
}
void AzureIoTMqttClient::report(String propertyName, String value) {
  String patch = "{\"" + propertyName + "\": \"" + value + "\"}";
  report(mqttClient, logger, patch);  
}
void AzureIoTMqttClient::report(JsonObject& value) {
  char patch[512];
  value.printTo(patch);
  report(mqttClient, logger, patch);  
}
void AzureIoTMqttClient::report(String propertyName, float value) {
  String patch = "{\"" + propertyName + "\": " + value + "}";
  report(mqttClient, logger, patch);  
}

void AzureIoTMqttClient::report(PubSubClient &client, log4Esp::Logger &logger, String patch) {
  int rid = millis();
    
  char topic[100] = "";
  sprintf(topic, AZIOT_REPORT_PROPERTY_OUTBOUND_TOPIC_TMPL, rid);

  if (!client.publish(topic, patch.c_str())) {
    logger.error(F("Unable to publish Reported Property update to '%s'. Update was: '%s'"), topic, patch.c_str());
  }
}

void AzureIoTMqttClient::onConnectionStatusChange(ONCONNECTIONSTATUSCHANGED_CALLBACK_SIGNATURE callback) {
    this->onConnectionStatusChangeCallback = callback;
}

void AzureIoTMqttClient::onCommand(ONCOMMAND_CALLBACK_SIGNATURE callback) {
    this->onCommandCallback = callback;
}

void AzureIoTMqttClient::onDesiredPropertyChange(DESIREDPROPERTYCHANGE_CALLBACK_SIGNATURE callback) {
    this->onDesiredPropertyChangeCallback = callback;
}