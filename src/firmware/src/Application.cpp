#include "Application.h"
#include "SD.h"
#include "WiFi.h"

#define MINIMAL_MESSAGE_LENGTH_IN_MS 1000
#define MAXIMAL_MESSAGE_LENGTH_IN_MS 24000

Application::Application(UserInterface* ui, AudioRecorder* recorder, AudioPlayer* player, FileUploader* uploader) : Application()
{
    this->ui = ui;
    this->recorder = recorder;
    this->player = player;
    this->uploader = uploader;

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

void Application::run() {    
    fsm.run_machine();

    client.loop();
    float maxVolume = 0.5;

    player->setGain(ui->getVolume() * 4.0f * maxVolume);
}

void Application::start() 
{
    settings.load();

    WiFi.mode(WIFI_STA);    // Station Mode, i.e. connect to a WIFI and don't serve as AP
    WiFi.persistent(false); // Do not store WIFI information in EEPROM.

    logger.trace(F("Connecting to WLAN with SSID '%s'. This may take some time..."), settings.getWifiSSID().c_str());

    WiFi.begin(settings.getWifiSSID().c_str(), settings.getWifiKey().c_str());

    config.load();

    contacts.load();

    inbox.load();

    client.onCommand(std::bind(&Application::dispatchCloudCommand, this, std::placeholders::_1, std::placeholders::_2));
    inbox.onNewMessage(std::bind(&Application::showNewMessageFrom, this, std::placeholders::_1));
}

void Application::whileStarting() 
{
    this->ui->isBusy(true);

    if (WiFi.isConnected()) {
        logger.trace(F("WiFi connection established. IP address: %s, MAC: %s"), WiFi.localIP().toString().c_str(), WiFi.macAddress().c_str());

        this->ui->showWelcome();
        this->fsm.trigger(Event::SYSTEM_READY);

        client.connect(config.getAzIoTHubName().c_str(), config.getDeviceId().c_str(), config.getAzIoTSASToken().c_str());
    }
}

void Application::beforeIdling() 
{
    this->ui->isBusy(false);

    for (size_t i = 0; i < contacts.size(); i++)
    {
        this->ui->showHasNewMessageAt(i, inbox.hasNewMessages(i));
    }
}

void Application::whileIdling() {
    
}

char currentRecordingName[64];
Contact* currentRecipient;
long currentBytesSent = 0;
File f;

void Application::recordMessageFor(int buttonId) 
{
    int position = buttonId - 1;
    currentRecipient = contacts.get(position);

    if (currentRecipient == NULL) {
        logger.warning(F("Unable to find contact for button %i (position: %i)"), buttonId, position);
        this->fsm.trigger(Event::DISCARD_MESSAGE);

        this->ui->showError();
        return;
    }

    sprintf(currentRecordingName, "/to_%i_at_%i.wav\0", currentRecipient->userId, millis());
    logger.trace(F("Capturing message for '%s' to '%s'"), currentRecipient->name, currentRecordingName);

    f = SD.open(currentRecordingName, FILE_WRITE);

    if (!f.write(1)) {
        logger.warning(F("Could not create file '%'. Write error: %i"), currentRecordingName, f.getWriteError());
        this->fsm.trigger(Event::DISCARD_MESSAGE);

        this->ui->showError();
        return;
    }
    f.seek(0);
    this->recorder->record(&f);    
}


void Application::whileMessageRecording() 
{
    int recordingDuration = this->recorder->duration();

    if (recordingDuration >= MAXIMAL_MESSAGE_LENGTH_IN_MS) {
        logger.trace(F("Stopped recording because current recording lenght (%ims) is above maximum of %ims"), recordingDuration, MAXIMAL_MESSAGE_LENGTH_IN_MS);
        this->fsm.trigger(RECORDING_LENGTH_EXCEEDED);
    }

    this->ui->showRecordingProgress((recordingDuration / 1000) + 1);
}

void Application::validateRecording() 
{
    long lenghtInMs = this->recorder->stop();
    f.close();

    logger.trace(F("Stopped recording. Validating recording length: %ims"), lenghtInMs);

    if (lenghtInMs >= MINIMAL_MESSAGE_LENGTH_IN_MS) {
        this->fsm.trigger(Event::SEND_MESSAGE);
    }
    else {
        logger.trace(F("Message length was below threshold of %ims, ignoring."), MINIMAL_MESSAGE_LENGTH_IN_MS);
        
        this->ui->showWarning();
        this->fsm.trigger(Event::DISCARD_MESSAGE);
    }
}

void Application::sendLastMessage() 
{
    this->ui->isBusy(true);

    logger.trace(F("Sending message '%s' to '%s'"), currentRecordingName, config.getPostMessageUrl().c_str());

    f = SD.open(currentRecordingName, FILE_READ);
    logger.trace(F("Message size: %i bytes"), f.size());

    String url = config.getPostMessageUrl();
    url.replace("{recipientId}", currentRecipient->userId);

    uploader->send(&f, url.c_str());
}

void Application::whileMessageSending() 
{
    if (uploader->isCompleted()) {
        currentBytesSent = 0;
        f.close();
        
        bool wasSuccessful = uploader->isSuccessful();
        logger.trace(F("Message is sent. Successful: %s"), wasSuccessful ? "yes" : "no");

        if (wasSuccessful) {
            this->ui->showSuccess();
        }
        else {
            this->ui->showError();
        }

        this->fsm.trigger(Event::MESSAGE_SENT);
        return;  
    }

    if (this->uploader->getBytesSent() != currentBytesSent) {
        currentBytesSent = this->uploader->getBytesSent();
        long total = this->uploader->getBytesTotal();
        int percent = (int)((currentBytesSent * 100.0f) / total);
        logger.verbose("Upload progress: %i/%i, %i%%", currentBytesSent, total, percent);
    }
}

void Application::tryPlayMessageFrom(int buttonId) 
{
    int slot = buttonId - 1;
    Contact* c = this->contacts.get(slot);

    String audioMessageFile = inbox.getAudioMessageFor((const char*)c->userId);

    if (audioMessageFile.isEmpty()) {
        logger.warning(F("No available audio message found from user '%s' on slot %i"), c->name, c->slot);
        this->fsm.trigger(Event::MESSAGE_NOTFOUND);
        return;
    }

    if (!SD.exists(audioMessageFile)) {
        logger.warning(F("File '%s' cannot be found to be played."), audioMessageFile);
        this->fsm.trigger(Event::MESSAGE_NOTFOUND);
        return;
    }

    logger.verbose("Playing file '%s' as message from '%s' (UserId: %s)", audioMessageFile.c_str(), c->name, c->userId);
    
    this->player->play(audioMessageFile.c_str());
    this->fsm.trigger(Event::MESSAGE_FOUND);
}

void Application::whileMessagePlaying() 
{
    if (player->isPlaying()) {
        this->ui->showAudioPlaying();
    }
    else {
        this->inbox.setPlayed(this->player->getFilename());
        this->fsm.trigger(Event::MESSAGE_PLAYED);
    }
}

void Application::messagePlayingEnded() {
    this->player->stop();
}

void Application::whileReceivingMessage() 
{
    inbox.downloadSingleMessage();
    fsm.trigger(Event::MESSAGE_RECEIVED);
}

void Application::dispatchCloudCommand(String commandName, JsonObject& value) 
{
    if (commandName == "newMessage") {

        inbox.handleNotification(value);
        fsm.trigger(Event::RECIEVING_MESSAGE);
    }
}

void Application::showNewMessageFrom(Contact* sender) 
{
    logger.trace("There is a new message on slot %i from sender '%s' (UserId: %s)", sender->slot, sender->name, sender->userId);
    this->ui->showHasNewMessageAt(sender->slot, true);
}