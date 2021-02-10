#ifndef __APPLICATION_H__
#define __APPLICATION_H__

#include "FunctionFsm.h"
#include "UserInterface.h"

class Application
{

    enum Event {
        BUTTON_CLICK,
        BUTTON_LONGSTART,
        BUTTON_LONG_RELEASE,
        MESSAGE_PLAYED
    };

    UserInterface *ui;

    FunctionState state_idle;
    FunctionState state_record;
    FunctionState state_play;

    FunctionFsm fsm;

public:
    Application() : state_idle([this]() { Serial.println("Enter Idle"); }, [this]() {}, [this]() {}),
                    state_record([this]() {Serial.println("Enter Record"); }, [this]() {}, [this]() {}),
                    state_play([this]() {Serial.println("Enter Play"); }, [this]() {}, [this]() {}),

                    fsm(&state_idle)
    {
    }

    Application(UserInterface *);

    void run();
};

#endif // __APPLICATION_H__