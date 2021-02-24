#include "UserInterface.h"
#include "pins.h"

OneButton button1 = OneButton(BUTTON1_IN, false, false);
OneButton button2 = OneButton(BUTTON2_IN, false, false);
OneButton button3 = OneButton(BUTTON3_IN, false, false);

void raiseButtonEvent(ButtonContext *ctx, Action action)
{
	ButtonEvent evt = {ctx->buttonId, action, ctx->button.getPressedTicks()};

	UserInterface *uiCast = (UserInterface *)ctx->ui;

	if (uiCast->buttonActionCallback != NULL)
	{
		uiCast->buttonActionCallback(evt);
	}
}
void fClicked(void *s)
{
	ButtonContext *ctx = (ButtonContext *)s;
	raiseButtonEvent(ctx, Clicked);
}

void fLongPressStart(void *s)
{
	ButtonContext *ctx = (ButtonContext *)s;
	raiseButtonEvent(ctx, LongPressStart);
}

void fLongPressEnd(void *s)
{
	ButtonContext *ctx = (ButtonContext *)s;
	raiseButtonEvent(ctx, LongPressEnd);
}

void UserInterface::setup()
{
	pinMode(LED_BUILTIN, OUTPUT);

	pinMode(BUTTON1_LED, OUTPUT);
	pinMode(BUTTON2_LED, OUTPUT);
	pinMode(BUTTON3_LED, OUTPUT);

	btnCtx1 = (ButtonContext){1, button1, this};
	btnCtx2 = (ButtonContext){2, button2, this};
	btnCtx3 = (ButtonContext){3, button3, this};

	button1.attachClick(fClicked, &btnCtx1);
	button1.attachLongPressStart(fLongPressStart, &btnCtx1);
	button1.attachLongPressStop(fLongPressEnd, &btnCtx1);

	button2.attachClick(fClicked, &btnCtx2);
	button2.attachLongPressStart(fLongPressStart, &btnCtx2);
	button2.attachLongPressStop(fLongPressEnd, &btnCtx2);

	button3.attachClick(fClicked, &btnCtx3);
	button3.attachLongPressStart(fLongPressStart, &btnCtx3);
	button3.attachLongPressStop(fLongPressEnd, &btnCtx3);

	ledRing.setup();
}

unsigned long previousMillis;
#define interval 50

void UserInterface::loop()
{

	button1.tick();
	button2.tick();
	button3.tick();

	ledRing.loop();

	digitalWrite(BUTTON1_LED, buttonStatus[0] ? HIGH : LOW);
	digitalWrite(BUTTON2_LED, buttonStatus[1] ? HIGH : LOW);
	digitalWrite(BUTTON3_LED, buttonStatus[2] ? HIGH : LOW);
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

void UserInterface::showHasNewMessageAt(int buttonIdx)
{
	this->buttonStatus[buttonIdx] = true;
}

void UserInterface::showAudioPlaying(int)
{
	ledRing.show(&WarmGlowAnimiation);
}