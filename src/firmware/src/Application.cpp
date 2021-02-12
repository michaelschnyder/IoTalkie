#include "Application.h"
#include "SD.h"
#include "WiFi.h"

Application::Application(UserInterface* ui, AudioRecorder* recorder, AudioPlayer* player) : Application()
{
    this->ui = ui;
    this->recoder = recoder;
    this->player = player;

    this->fsm.add_transition(&state_startup, &state_idle, SYSTEM_READY, nullptr);

    this->fsm.add_transition(&state_idle, &state_record1, BUTTON1_LONGSTART, nullptr);
    this->fsm.add_transition(&state_idle, &state_record2, BUTTON2_LONGSTART, nullptr);
    this->fsm.add_transition(&state_idle, &state_record3, BUTTON3_LONGSTART, nullptr);
    this->fsm.add_transition(&state_record1, &state_idle, BUTTON1_LONG_RELEASE, nullptr);
    this->fsm.add_transition(&state_record2, &state_idle, BUTTON2_LONG_RELEASE, nullptr);
    this->fsm.add_transition(&state_record3, &state_idle, BUTTON3_LONG_RELEASE, nullptr);

    this->fsm.add_transition(&state_idle, &state_play1, BUTTON1_CLICK, nullptr);
    this->fsm.add_transition(&state_idle, &state_play2, BUTTON2_CLICK, nullptr);
    this->fsm.add_transition(&state_idle, &state_play3, BUTTON3_CLICK, nullptr);
    this->fsm.add_transition(&state_play1, &state_idle, MESSAGE_PLAYED, nullptr);
    this->fsm.add_transition(&state_play2, &state_idle, MESSAGE_PLAYED, nullptr);
    this->fsm.add_transition(&state_play3, &state_idle, MESSAGE_PLAYED, nullptr);

    this->ui->onButtonEvent([this](ButtonEvent evt) {
    
        if (evt.action == Action::Clicked) {
            switch (evt.buttonId)
            {
            case 1: this->fsm.trigger(Event::BUTTON1_CLICK); break;
            case 2: this->fsm.trigger(Event::BUTTON2_CLICK); break;
            case 3: this->fsm.trigger(Event::BUTTON3_CLICK); break;
            default:
                break;
            }
        }

        if (evt.action == Action::LongPressStart) {
            switch (evt.buttonId)
            {
            case 1: this->fsm.trigger(Event::BUTTON1_LONGSTART); break;
            case 2: this->fsm.trigger(Event::BUTTON2_LONGSTART); break;
            case 3: this->fsm.trigger(Event::BUTTON3_LONGSTART); break;
            default:
                break;
            }
        }

        if (evt.action == Action::LongPressEnd) {
            switch (evt.buttonId)
            {
            case 1: this->fsm.trigger(Event::BUTTON1_LONG_RELEASE); break;
            case 2: this->fsm.trigger(Event::BUTTON2_LONG_RELEASE); break;
            case 3: this->fsm.trigger(Event::BUTTON3_LONG_RELEASE); break;
            default:
                break;
            }
        }
     });
}

void Application::start() 
{
    config.load();

    WiFi.mode(WIFI_STA);    // Station Mode, i.e. connect to a WIFI and don't serve as AP
    WiFi.persistent(false); // Do not store WIFI information in EEPROM.

    logger.trace(F("Connecting to WLAN with SSID '%s'. This may take some time..."), config.getWifiSSID().c_str());

    WiFi.begin(config.getWifiSSID().c_str(), config.getWifiKey().c_str());
}

void Application::run() {    
    fsm.run_machine();
}

File* currRec;

void Application::recordMessageFor(int buttonId) 
{
    char filename[30];
    sprintf(filename, "/to_btn%i_at_%i.wav\0", buttonId, millis());

    Serial.print("Recording to ");
    Serial.println(filename);

    File recording = SD.open(filename, FILE_WRITE);
    currRec = &recording;
    this->recoder->record(&recording);    
}

void Application::completeRecording() 
{
    Serial.println("Finishing recording");
    this->recoder->stop();
    
}

void Application::whileMessageRecording() 
{
}

void Application::playMessageFrom(int buttonId) 
{
    
}

void Application::whileMessagePlaying() 
{
    if (millis() % 100 == 0) {
        this->fsm.trigger(Event::MESSAGE_PLAYED);
    }
}

void Application::whileStarting() 
{
    if (WiFi.isConnected()) {
        this->fsm.trigger(Event::SYSTEM_READY);
    }
}
