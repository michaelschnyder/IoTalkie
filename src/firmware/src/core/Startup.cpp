#include "Startup.h"

String file_size(uint64_t bytes){
  String fsize = "";
  if (bytes < 1024)                 fsize = String((long)bytes)+" B";
  else if(bytes < (1024*1024))      fsize = String(bytes/1024.0,3)+" KB";
  else if(bytes < (1024*1024*1024)) fsize = String(bytes/1024.0/1024.0,3)+" MB";
  else                              fsize = String(bytes/1024.0/1024.0/1024.0,3)+" GB";
  return fsize;
}

void Startup::run() 
{
    this->fsm.run_machine();
    yield();
}

void Startup::onCompleted(ONCOMPLETED_CALLBACK_SIGNATURE callback) 
{
    this->onCompletedCallback = callback;
}

void Startup::post() 
{
    Serial.begin(115200);
    Serial.println("");
    Serial.println("System started");

    if (ui->isButtonPowerOff()) {
        // this->ui->isBusy(false);
        fsm.trigger(Event::PowerOff);
    }
    else {
        this->ui->isBusy(true);
        fsm.trigger(Event::Continue);
    }
}

void Startup::whilePowerOff() 
{
    ESP.deepSleep(1000);

    if (!ui->isButtonPowerOff()) {
        ESP.restart();
    }
}

void Startup::checkSPIFFS() 
{
    if (SPIFFS.begin()) {
        Serial.printf("Internal Storage: %s of %s used", file_size(SPIFFS.usedBytes()).c_str(), file_size(SPIFFS.totalBytes()).c_str());
        Serial.println();
        fsm.trigger(Event::Continue);
    }
}

void Startup::loadConfig()
{
    if(config->load()) {
        fsm.trigger(Event::Continue);
    }
}

void Startup::checkSDCardFS() 
{
    if(SD.begin(SS)) {
        Serial.printf("External SD card:  %s of %s used", file_size(SD.usedBytes()).c_str(), file_size(SD.totalBytes()).c_str());
        Serial.println();    
    
        fsm.trigger(Event::Continue);
    };    
}

void Startup::loadSettings() 
{
    if(settings->load()) {
        fsm.trigger(Event::Continue);
    }
}

void Startup::loadContacts() 
{
    if(contacts->load()) {
        fsm.trigger(Event::Continue);
    }    
}

void Startup::startWifi() 
{
    WiFi.mode(WIFI_STA);    // Station Mode, i.e. connect to a WIFI and don't serve as AP
    WiFi.persistent(false); // Do not store WIFI information in EEPROM.

    logger.trace(F("Connecting to WLAN with SSID '%s'. This may take some time..."), settings->getWifiSSID().c_str());
    WiFi.begin(settings->getWifiSSID().c_str(), settings->getWifiKey().c_str());
}

void Startup::waitForWifi() 
{
    if (WiFi.isConnected()) {
        fsm.trigger(Event::Continue);
    }
}

void Startup::connectToMqtt() 
{
    client->connect(config->getAzIoTHubName().c_str(), config->getDeviceId().c_str(), config->getAzIoTSASToken().c_str());   
    fsm.trigger(Event::Continue);
}

void Startup::startInbox() 
{
    inbox->load();
    inbox->hasPendingDownloads(true);

    fsm.trigger(Event::Continue);
}

void Startup::whenReady() 
{
    logger.trace("Startup completed!");

    if (this->onCompletedCallback) {
        this->onCompletedCallback(0);
    }
}