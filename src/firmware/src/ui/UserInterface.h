#ifndef __USERINTERFACE_H__
#define __USERINTERFACE_H__

#include "LedRing.h"
#include "ButtonPanel.h"
#include "Screen.h"

#include "animations/WaveAnimation.h"
#include "animations/GlowAnimation.h"

struct RecordingProgressData {
    int progress;
};

class UserInterface {
    unsigned long lastInputScan;
    unsigned long inputScanInterval = 100;

    ButtonPanel* buttonPanelPtr;
    LedRing ledRing;
    WaveAnimation BlueProgressAnimation;
    GlowAnimation WarmGlowAnimiation;
    GlowAnimation SucessGlowAnimiation;
    GlowAnimation WarningGlowAnimiation;
    GlowAnimation ErrorGlowAnimiation;

    float volume = 0.25;
    int volumePin;
    int ambiencePin;

    public:

        UserInterface(ButtonPanel* buttonPanel, int volumePin, int ambiencePin) : 
            WarmGlowAnimiation(HUE_YELLOW, ANI_SPEED_FAST, 1),
            SucessGlowAnimiation(HUE_GREEN, ANI_SPEED_FAST, 1),
            WarningGlowAnimiation(HUE_ORANGE, ANI_SPEED_FAST, 1),
            ErrorGlowAnimiation(HUE_RED, ANI_SPEED_FAST, 1)
            {
                this->buttonPanelPtr = buttonPanel;
                this->volumePin = volumePin;
                this->ambiencePin = ambiencePin;
            };

        void setup();
        void loop();

        ButtonPanel* buttonPanel() {
            return this->buttonPanelPtr;
        }

        bool isPowerButtonOn();
        void showHasNewMessageAt(int, bool);

        float getVolume();
        void isBusy(bool);
        void showRecordingProgress(int);
        void showSuccess();
        void showWarning();
        void showError();
        void showWelcome();
        void showAudioPlaying();
};

#endif // __USERINTERFACE_H__