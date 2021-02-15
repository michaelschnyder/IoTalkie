#include "Application.h"
#include "SD.h"
#include "WiFi.h"

Application::Application(UserInterface* ui, AudioRecorder* recorder, AudioPlayer* player, FileUploader* uploader) : Application()
{
    this->ui = ui;
    this->recoder = recoder;
    this->player = player;
    this->uploader = uploader;

    this->fsm.add_transition(&state_startup, &state_idle, SYSTEM_READY, nullptr);

    this->fsm.add_transition(&state_idle, &state_record1, BUTTON1_LONGSTART, nullptr);
    this->fsm.add_transition(&state_idle, &state_record2, BUTTON2_LONGSTART, nullptr);
    this->fsm.add_transition(&state_idle, &state_record3, BUTTON3_LONGSTART, nullptr);
    this->fsm.add_transition(&state_record1, &state_validate, BUTTON1_LONG_RELEASE, nullptr);
    this->fsm.add_transition(&state_record2, &state_validate, BUTTON2_LONG_RELEASE, nullptr);
    this->fsm.add_transition(&state_record3, &state_validate, BUTTON3_LONG_RELEASE, nullptr);
    this->fsm.add_transition(&state_validate, &state_send, SEND_MESSAGE, nullptr);
    this->fsm.add_transition(&state_send, &state_idle, MESSAGE_SENT, nullptr);
    this->fsm.add_transition(&state_validate, &state_idle, DISCARD_MESSAGE, nullptr);

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

char currentRecordingName[30];
long currentBytesSent = 0;

void Application::recordMessageFor(int buttonId) 
{
    sprintf(currentRecordingName, "/to_btn%i_at_%i.wav\0", buttonId, millis());

    Serial.print("Recording to ");
    Serial.println(currentRecordingName);

    File recording = SD.open(currentRecordingName, FILE_WRITE);
    this->recoder->record(&recording);    
}

void Application::validateRecording() 
{
    Serial.println("Finishing recording");
    this->recoder->stop();
    this->fsm.trigger(Event::SEND_MESSAGE);
}

File f;

void Application::sendLastMessage() 
{
    logger.trace(F("Sending message '%s' to '%s'"), currentRecordingName, config.getPostMessageUrl().c_str());

    f = SD.open(currentRecordingName, FILE_READ);
    logger.trace(F("Message size: %i bytes"), f.size());

    uploader->send(&f, config.getPostMessageUrl().c_str());
}


void Application::whileMessageSending() 
{
    if (uploader->isCompleted()) {
        currentBytesSent = 0;
        logger.trace(F("Message is sent."));
        f.close();

        this->fsm.trigger(Event::MESSAGE_SENT);    
    }
    else {

        if (this->uploader->getBytesSent() != currentBytesSent) {
            currentBytesSent = this->uploader->getBytesSent();
            long total = this->uploader->getBytesTotal();
            int percent = (int)((currentBytesSent * 100.0f) / total);
            logger.verbose("Upload progress: %i/%i, %i%%", currentBytesSent, total, percent);
        }
    }
}

void Application::whileMessageRecording() 
{
    // TODO: Update ui if below 24s and otherwise stop
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
        logger.trace(F("WiFi connection established. IP address: %s"), WiFi.localIP().toString().c_str());
    }
}
