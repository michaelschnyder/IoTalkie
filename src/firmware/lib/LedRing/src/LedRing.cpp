// Source: https://gist.github.com/hsiboy/f9ef711418b40e259b06

#include "LedRing.h"

#define BRIGHTNESS 128 

void LedRing::setup() 
{
    FastLED.addLeds<WS2811, LED_RING_PIN, GRB>(strip, NUM_LEDS).setCorrection(TypicalLEDStrip);
	
    FastLED.setBrightness(BRIGHTNESS);
 	FastLED.clear(true);
}

void LedRing::loop() 
{
	if (animationCompleted) {
		return;
	}

	if (this->currentAnimation != NULL) {
		bool isRunning = this->currentAnimation->run();
		this->animationCompleted = !isRunning;
	}
}

void LedRing::progress(int numberOfLeds) 
{
    this->currentAnimation = NULL;
	this->animationCompleted = false;
	int progressHue = 150;
	
	FastLED.clear();

	for(int i = 0; i < numberOfLeds; i++) {
		strip[i] = CHSV(progressHue, 255, 255);
	}

	FastLED.show();
}

void LedRing::reset() 
{
	this->currentAnimation = NULL;
	this->animationCompleted = false;
	FastLED.clear();
	FastLED.show();	
}

void LedRing::show(LedAnimation* animation) 
{
	if (currentAnimation == animation) {
		return;
	}

	animation->initialize(strip, NUM_LEDS);
	this->currentAnimation = animation;	
}