#include "Shutdown.h"

Shutdown::Shutdown(UserInterface* userInterface, AzureIoTMqttClient* azClient) : 

    state_initShutdown(nullptr,  [this](){this->initShutdown(); }, nullptr),
    state_issueFarewell(nullptr, [this](){this->issueFarewell(); }, nullptr),
    state_stopWifi(nullptr,      [this](){this->stopWifi(); }, nullptr),
    state_unmountSdCard(nullptr, [this](){this->unmountSdCard(); }, nullptr),
    state_unmountSPIIFS(nullptr, [this](){this->unmountSPIIFS(); }, nullptr),
    state_reset(nullptr,         [this](){this->reset(); }, nullptr),

    fsm(&state_initShutdown)
{
    this->ui = userInterface;
    this->client = azClient;

    fsm.add_transition(&state_initShutdown, &state_issueFarewell, Event::Continue, nullptr);
    fsm.add_transition(&state_issueFarewell, &state_stopWifi, Event::Continue, nullptr);
    fsm.add_transition(&state_stopWifi, &state_unmountSdCard, Event::Continue, nullptr);
    fsm.add_transition(&state_unmountSdCard, &state_unmountSPIIFS, Event::Continue, nullptr);
    fsm.add_transition(&state_unmountSPIIFS, &state_reset, Event::Continue, nullptr);
}

void Shutdown::run() 
{
    ui->loop();
    client->loop();
    fsm.run_machine();
}

void Shutdown::initShutdown() 
{
    logger.verbose("Shutdown was initiated.");

    ui->isBusy(true);
    fsm.trigger(Event::Continue);    
}

void Shutdown::issueFarewell() 
{
    logger.verbose("Closing connection to MQTT");
    client->send("{\"type\": \"ClientBye\"}");

    client->disconnect();
    fsm.trigger(Event::Continue);    
}

void Shutdown::stopWifi() 
{
    logger.verbose("Disconnecting WiFi");

    WiFi.disconnect(true, true);

    while(WiFi.isConnected()) {
        Serial.print(".");
    }

    while(WiFi.getMode() != WIFI_MODE_NULL) {
        Serial.print("q");
    }

    fsm.trigger(Event::Continue);
}  


void Shutdown::unmountSdCard() 
{
    logger.verbose("Unmounting SD Card...");

    SD.end();
    fsm.trigger(Event::Continue);
}

void Shutdown::unmountSPIIFS() 
{
    logger.verbose("Unmounting SPIFFS...");

    SPIFFS.end();
    fsm.trigger(Event::Continue);
}

void Shutdown::reset() 
{
    logger.verbose("Reset.");
    Serial.print("~~~~~ RESET ESP ~~~~~");
    Serial.flush();

    ESP.restart();
}
