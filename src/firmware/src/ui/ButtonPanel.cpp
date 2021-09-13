#include "hardware.h"

#include "ButtonPanel.h"

OneButton button1 = OneButton(-1, false, false);
OneButton button2 = OneButton(-1, false, false);
OneButton button3 = OneButton(-1, false, false);

void raiseButtonEvent(ButtonContext *ctx, Action action)
{
	ButtonEvent evt = {ctx->buttonId, action, ctx->button.getPressedTicks()};

	ButtonPanel *panelCast = (ButtonPanel *)ctx->ui;

	if (panelCast->buttonActionCallback != NULL)
	{
		panelCast->buttonActionCallback(evt);
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

void ButtonPanel::setup() 
{
    mcp.begin();
	mcp.setupInterrupts(true, false, LOW);
	mcp.setupInterruptPin(BUTTON_OFF_IN, FALLING); 

	mcp.pinMode(BUTTON1_LED, OUTPUT);
	mcp.pinMode(BUTTON2_LED, OUTPUT);
	mcp.pinMode(BUTTON3_LED, OUTPUT);


	mcp.pinMode(BUTTON1_IN, INPUT);
	mcp.pinMode(BUTTON2_IN, INPUT);
	mcp.pinMode(BUTTON3_IN, INPUT);

	mcp.pinMode(BUTTON_OFF_IN, INPUT);

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
}

void ButtonPanel::loop() 
{
	if (millis() - lastInputScan >= inputScanInterval || lastInputScan == 0) {

		button1.tick(mcp.digitalRead(BUTTON1_IN));
		button2.tick(mcp.digitalRead(BUTTON2_IN));
		button3.tick(mcp.digitalRead(BUTTON3_IN));

		mcp.digitalWrite(BUTTON1_LED, buttonStatus[0] ? HIGH : LOW);
		mcp.digitalWrite(BUTTON2_LED, buttonStatus[1] ? HIGH : LOW);
		mcp.digitalWrite(BUTTON3_LED, buttonStatus[2] ? HIGH : LOW);

    	lastInputScan = millis();

		bool previousState = isPowerOff;
		isPowerOff = mcp.digitalRead(BUTTON_OFF_IN);

		if (isPowerOff && !previousState) {
			powerOffCallback();
		}
	}    
}

bool ButtonPanel::isPowerButtonOn() 
{
    return !isPowerOff;
}

void ButtonPanel::showHasNewMessageAt(int buttonIdx, bool hasMessages) 
{
    	this->buttonStatus[buttonIdx] = hasMessages;
}