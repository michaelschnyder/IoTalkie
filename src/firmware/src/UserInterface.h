#ifndef __USERINTERFACE_H__
#define __USERINTERFACE_H__
#include <functional>

#include <OneButton.h>
#include "ViewModel.h"

#include "LedRing.h"
#include "animations/Spin.h"

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

    ButtonContext btnCtx1;
    ButtonContext btnCtx2;
    ButtonContext btnCtx3;

    LedRing ledRing;
    Spinner SpinningAnimation;
    
    ViewModel* vm;

    public:
        void setup();
        void loop();
    
        BUTTON_PRESS_CALLBACK_TYPE buttonActionCallback;
        void onButtonEvent(BUTTON_PRESS_CALLBACK_TYPE callback) {
            this->buttonActionCallback = callback;    
        }

        void setVm(ViewModel*);

        
};

#endif // __USERINTERFACE_H__