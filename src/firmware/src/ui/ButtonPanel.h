#ifndef  __BUTTONPANEL_H__
#define __BUTTONPANEL_H__

#include <Wire.h>
#include "Adafruit_MCP23017.h"

#include <functional>
#include <OneButton.h>

#define BUTTON_PRESS_CALLBACK_TYPE std::function<void(ButtonEvent)>
#define POWEROFF_STATE_CALLBACK_TYPE std::function<void()>

enum Action {
    Clicked,
    LongPressStart,
    LongPressEnd
};

struct ButtonEvent
{
    int buttonId;
    Action action;
    long duration;
};

struct ButtonContext {
    int buttonId;
    OneButton button;
    void* ui;
};

class ButtonPanel {

    unsigned long lastInputScan;
    unsigned long inputScanInterval = 100;
    Adafruit_MCP23017 mcp;

    ButtonContext btnCtx1;
    ButtonContext btnCtx2;
    ButtonContext btnCtx3;

    bool buttonStatus[3];
    bool isPowerOff = false;

    public:

        virtual void setup();
        virtual void loop();
    
        BUTTON_PRESS_CALLBACK_TYPE buttonActionCallback;
        void onButtonEvent(BUTTON_PRESS_CALLBACK_TYPE callback) {
            this->buttonActionCallback = callback;    
        }

        POWEROFF_STATE_CALLBACK_TYPE powerOffCallback;
        void onPowerOff(POWEROFF_STATE_CALLBACK_TYPE callback) {
            this->powerOffCallback = callback;    
        }

        virtual bool isPowerButtonOn();
        void showHasNewMessageAt(int, bool);
};

#endif //__BUTTONPANEL_H_LL
