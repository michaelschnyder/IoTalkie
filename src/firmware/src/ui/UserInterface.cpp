#include "UserInterface.h"
#include "pins.h"

void UserInterface::setup()
{
	pinMode(LED_BUILTIN, OUTPUT);

	ledRing.setup();
	ledRing.reset();

	buttonPanelPtr->setup();
}

void UserInterface::loop()
{
	ledRing.loop();

	buttonPanelPtr->loop();

	if (millis() - lastInputScan >= inputScanInterval || lastInputScan == 0) {
    	lastInputScan = millis();
		volume = analogRead(POT_IN) / 4096.0f;
	}
}

float UserInterface::getVolume() 
{
	return volume;
}

bool UserInterface::isPowerButtonOn() {
	return buttonPanelPtr->isPowerButtonOn();
}

void UserInterface::isBusy(bool isBusy)
{
	if (isBusy)
	{
		ledRing.show(&BlueProgressAnimation);
	}
	else
	{
		ledRing.hide(&BlueProgressAnimation);
	}
}

void UserInterface::showRecordingProgress(int value)
{
	ledRing.progress(value);
}

void UserInterface::showSuccess()
{
	ledRing.show(&SucessGlowAnimiation);
}

void UserInterface::showWarning()
{
	ledRing.show(&WarningGlowAnimiation);
}

void UserInterface::showError()
{
	ledRing.show(&ErrorGlowAnimiation);
}

void UserInterface::showWelcome()
{
	ledRing.show(&WarmGlowAnimiation);
}

void UserInterface::showHasNewMessageAt(int buttonIdx, bool hasMessages)
{
	this->buttonPanelPtr->showHasNewMessageAt(buttonIdx, hasMessages);
}

void UserInterface::showAudioPlaying()
{
	ledRing.show(&WarmGlowAnimiation);
}