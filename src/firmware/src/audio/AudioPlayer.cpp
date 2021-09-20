#include "hardware.h"

#include "AudioPlayer.h"

AudioPlayer::AudioPlayer(int clockPin, int lrcPin, int dataPin) {
	this->clockPin = clockPin;
	this->lrcPin = lrcPin;
	this->dataPin = dataPin;	
}

void AudioPlayer::setup()
{
	audioLogger = &Serial;
	out = new AudioOutputI2S();
	out->SetGain(0.128);
	out->SetPinout(clockPin, lrcPin, dataPin);

	progmemSoundSource = new AudioFileSourcePROGMEM(emptyMp3, sizeof(emptyMp3));
	sdCardSoundSource = new AudioFileSourceSD();

	audioGenerator = new AudioGeneratorMP3();

	playing = audioGenerator->begin(progmemSoundSource, out);
}

void AudioPlayer::play(const char *filename)
{
	logger.trace("Playing audio from '%s'", filename);

	sdCardSoundSource->open(filename);
	playing = audioGenerator->begin(sdCardSoundSource, out);
	strncpy(this->filename, filename, sizeof(this->filename) - 1);
}

void AudioPlayer::stop() 
{
	if (playing) {
		audioGenerator->stop();
	}
}

/**
 * Between 0.0 and 4.0
 **/
void AudioPlayer::setGain(float gain) {
	out->SetGain(gain);
}

void AudioPlayer::loop()
{
	if (!playing)
	{
		return;
	}

	bool stillRunning = audioGenerator->isRunning() && audioGenerator->loop();

	if (!stillRunning) {
		
		// Audio Generator has stopped because of file end, or there was an error during looping
		if (strlen(this->filename) > 0) {
			logger.trace("Finished playing audio from '%s'", filename);
		}
		else {
			logger.trace("Finished playing audio.");
		}

		playing = false;
		audioGenerator->stop();
		if (sdCardSoundSource->isOpen())
		{
			sdCardSoundSource->close();
		}		
	}
}

bool AudioPlayer::isPlaying()
{
	return this->playing;
}

const char* AudioPlayer::getFilename() 
{
	return filename;
}