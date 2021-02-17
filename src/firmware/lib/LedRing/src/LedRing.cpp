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
	if (this->currentAnimation == NULL) {
		return;
	}

	if (!this->currentAnimation->run()) {
		this->currentAnimation = NULL;
	}
}

void LedRing::progress(int numberOfLeds) 
{
    this->currentAnimation = NULL;
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

void LedRing::hide(LedAnimation* animation) 
{
	if (currentAnimation != animation) {
		return;
	}

	reset();

}