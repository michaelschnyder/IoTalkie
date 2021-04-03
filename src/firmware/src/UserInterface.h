#ifndef __USERINTERFACE_H__
#define __USERINTERFACE_H__
#include <Wire.h>
#include "Adafruit_MCP23017.h"

#include <functional>
#include <OneButton.h>

#include "LedRing.h"
#include "animations/WaveAnimation.h"
#include "animations/GlowAnimation.h"

#define BUTTON_PRESS_CALLBACK_TYPE std::function<void(ButtonEvent)>
#define POWEROFF_STATE_CALLBACK_TYPE std::function<void()>

struct RecordingProgressData {
    int progress;
};

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

class UserInterface {

    unsigned long lastInputScan;
    unsigned long inputScanInterval = 100;
    Adafruit_MCP23017 mcp;

    ButtonContext btnCtx1;
    ButtonContext btnCtx2;
    ButtonContext btnCtx3;

    LedRing ledRing;
    WaveAnimation BlueProgressAnimation;
    GlowAnimation WarmGlowAnimiation;
    GlowAnimation SucessGlowAnimiation;
    GlowAnimation WarningGlowAnimiation;
    GlowAnimation ErrorGlowAnimiation;

    bool buttonStatus[3];
    bool isPowerOff = false;
    float volume = 0.25;

    public:

        UserInterface() : 
            WarmGlowAnimiation(HUE_YELLOW, ANI_SPEED_FAST, 1),
            SucessGlowAnimiation(HUE_GREEN, ANI_SPEED_FAST, 1),
            WarningGlowAnimiation(HUE_ORANGE, ANI_SPEED_FAST, 1),
            ErrorGlowAnimiation(HUE_RED, ANI_SPEED_FAST, 1)
            { };

        void setup();
        void loop();
    
        BUTTON_PRESS_CALLBACK_TYPE buttonActionCallback;
        void onButtonEvent(BUTTON_PRESS_CALLBACK_TYPE callback) {
            this->buttonActionCallback = callback;    
        }

        POWEROFF_STATE_CALLBACK_TYPE powerOffCallback;
        void onPowerOff(POWEROFF_STATE_CALLBACK_TYPE callback) {
            this->powerOffCallback = callback;    
        }

        float getVolume();
        bool isPowerButtonOn();
        void isBusy(bool);
        void showRecordingProgress(int);
        void showSuccess();
        void showWarning();
        void showError();
        void showWelcome();
        void showHasNewMessageAt(int, bool);
        void showAudioPlaying();
};

#endif // __USERINTERFACE_H__