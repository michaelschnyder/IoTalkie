#ifndef __APPLICATION_H__
#define __APPLICATION_H__

#include "FunctionFsm.h"
#include "UserInterface.h"

class Application
{

    enum Event {
        BUTTON_CLICK,
        BUTTON1_LONGSTART,
        BUTTON1_LONG_RELEASE,
        BUTTON2_LONGSTART,
        BUTTON2_LONG_RELEASE,
        BUTTON3_LONGSTART,
        BUTTON3_LONG_RELEASE,
        MESSAGE_PLAYED
    };

    UserInterface *ui;
    
    FunctionState state_idle;
    FunctionState state_record1;
    FunctionState state_record2;
    FunctionState state_record3;
    FunctionState state_play;

    FunctionFsm fsm;

public:
    Application() : state_idle([this]() { Serial.println("Enter Idle"); }, [this]() {}, [this]() {}),
                    state_record1([this]() {Serial.println("Enter Record1"); }, [this]() {}, [this]() {}),
                    state_record2([this]() {Serial.println("Enter Record2"); }, [this]() {}, [this]() {}),
                    state_record3([this]() {Serial.println("Enter Record3"); }, [this]() {}, [this]() {}),
                    state_play([this]() {Serial.println("Enter Play"); }, [this]() {}, [this]() {}),

                    fsm(&state_idle)
    {
    }

    Application(UserInterface *);

    void run();
};

#endif // __APPLICATION_H__