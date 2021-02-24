#include "AudioPlayer.h"

void AudioPlayer::setup()
{
	audioLogger = &Serial;
	out = new AudioOutputI2S();
	out->SetGain(0.128);
	out->SetPinout(AMP_PIN_BCLK, AMP_PIN_LRC, AMP_PIN_DIN);

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
