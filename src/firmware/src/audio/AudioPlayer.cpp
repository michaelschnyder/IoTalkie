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


	sdCardSoundSource = new AudioFileSourceSD();

	audioGeneratorOpus = new AudioGeneratorOpus();
	audioGeneratorWAV = new AudioGeneratorWAV();
	audioGeneratorMp3 = new AudioGeneratorMP3();
	
	currentAudioGenerator = audioGeneratorMp3;
	progmemSoundSource = new AudioFileSourcePROGMEM(emptyMp3, sizeof(emptyMp3));
	playing = currentAudioGenerator->begin(progmemSoundSource, out);
}

void AudioPlayer::play(const char *filename)
{
	if (playing) {
		logger.error("Player is still busy with playing previous file.");
		return;
	}

	String extension = FileInfo::getExtension(filename);

	logger.trace("Identifying audio player for '%s' with extension '%s'", filename, extension.c_str());
	if (extension == ".mp3") {
		currentAudioGenerator = audioGeneratorMp3;
	}

	else if (extension == ".wav") {
		currentAudioGenerator = audioGeneratorWAV;
	}

	else if (extension == ".ogg") {
		currentAudioGenerator = audioGeneratorOpus;
	}
	else {
		logger.error("Unsupported file extension '%s'", extension);
		return;
	}

	logger.trace("Playing audio from '%s'", filename);
	sdCardSoundSource->open(filename);
	playing = currentAudioGenerator->begin(sdCardSoundSource, out);
	strncpy(this->filename, filename, sizeof(this->filename) - 1);
}

void AudioPlayer::stop() 
{
	if (playing) {
		currentAudioGenerator->stop();
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

	bool stillRunning = currentAudioGenerator->isRunning() && currentAudioGenerator->loop();

	if (!stillRunning) {
		
		// Audio Generator has stopped because of file end, or there was an error during looping
		if (strlen(this->filename) > 0) {
			logger.trace("Finished playing audio from '%s'", filename);
		}
		else {
			logger.trace("Finished playing audio.");
		}

		playing = false;
		currentAudioGenerator->stop();
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