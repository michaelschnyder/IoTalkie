#ifndef __USERINTERFACE_H__
#define __USERINTERFACE_H__
#include <functional>

#include <OneButton.h>

#define BUTTON_PRESS_CALLBACK_TYPE std::function<void(ButtonEvent)>

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

void raiseEvent(int buttonId, Action action, long duration);

public:
    void setup();
    void loop();
  
    BUTTON_PRESS_CALLBACK_TYPE buttonActionCallback;
    void onButtonEvent(BUTTON_PRESS_CALLBACK_TYPE callback) {
        this->buttonActionCallback = callback;    
    }
};

#endif // __USERINTERFACE_H__