#include "Startup.h"

String file_size(uint64_t bytes){
  String fsize = "";
  if (bytes < 1024)                 fsize = String((long)bytes)+" B";
  else if(bytes < (1024*1024))      fsize = String(bytes/1024.0,3)+" KB";
  else if(bytes < (1024*1024*1024)) fsize = String(bytes/1024.0/1024.0,3)+" MB";
  else                              fsize = String(bytes/1024.0/1024.0/1024.0,3)+" GB";
  return fsize;
}

// TODO Move to Header
SPIClass sdSPI(VSPI);

void Startup::run() 
{
    if (!fsm.is_in_state(state_halt)) {
        ui->loop();    
    }
    
    this->ui->isBusy(true);
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
    Serial.printf("IoTalkie %s (%s), %s@%s", BuildInfo::gitCommit(), BuildInfo::buildTimeGmt(), BuildInfo::buildUser(), BuildInfo::buildHost());
    Serial.println();

    this->ui->isBusy(false);

    if (!ui->isPowerButtonOn()) {
        fsm.trigger(Event::Halt);
    }
    else {
        this->ui->isBusy(true);
        fsm.trigger(Event::Continue);
    }
}

void Startup::whileHalt() 
{
    Serial.printf("System halt, wait for LOW on pin to exit deep sleep");
    esp_sleep_enable_ext0_wakeup((gpio_num_t)MCP23017_IRT, LOW);
    esp_deep_sleep_start();
}

void Startup::checkSPIFFS() 
{
    if (SPIFFS.begin()) {
        Serial.printf("Internal Storage: %s of %s used", file_size(SPIFFS.usedBytes()).c_str(), file_size(SPIFFS.totalBytes()).c_str());
        Serial.println();
        fsm.trigger(Event::Continue);
    }

    delay(50);
}

void Startup::loadConfig()
{
    if(config->load()) {
        fsm.trigger(Event::Continue);
    }
}

void Startup::checkSDCardFS() 
{
    if(SD.begin(SS, sdSPI)) {
        Serial.printf("External SD card:  %s of %s used", file_size(SD.usedBytes()).c_str(), file_size(SD.totalBytes()).c_str());
        Serial.println();    
    
        fsm.trigger(Event::Continue);
    }

    delay(50);
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

    logger.trace(F("Connecting to WLAN with SSID '%s' phy: %s. This may take some time..."), settings->getWifiSSID().c_str(), WiFi.macAddress().c_str());
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

void Startup::setError(int code, const char* message) 
{
    Serial.printf("ERROR %x: %s", code, message);

    bool firstBit =  ((code & 0b001) >> 0);
    bool secondBit = ((code & 0b010) >> 1);
    bool thirdBit =  ((code & 0b100) >> 2);

    // Code is displayed from bottom to top
    ui->showHasNewMessageAt(0, thirdBit);
    ui->showHasNewMessageAt(1, secondBit);
    ui->showHasNewMessageAt(2, firstBit);
}

void Startup::whileError() 
{
    ui->showError();

    if (!ui->isPowerButtonOn()) {
        ESP.restart();
    }
}

