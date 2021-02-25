#ifndef __USERINTERFACE_H__
#define __USERINTERFACE_H__
#include <functional>

#include <OneButton.h>

#include "LedRing.h"
#include "animations/WaveAnimation.h"
#include "animations/GlowAnimation.h"

#define BUTTON_PRESS_CALLBACK_TYPE std::function<void(ButtonEvent)>

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

    unsigned long lastAnalogScan;
    unsigned long analogScanInterval = 100;

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

        float getVolume();
        bool isButtonPowerOff();
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