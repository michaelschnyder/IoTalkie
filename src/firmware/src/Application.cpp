#include "Application.h"

Application::Application(UserInterface* ui) : Application()
{
    this->ui = ui;
    
    this->fsm.add_transition(&state_idle, &state_record1, BUTTON1_LONGSTART, nullptr);
    this->fsm.add_transition(&state_record1, &state_idle, BUTTON1_LONG_RELEASE, nullptr);
    this->fsm.add_transition(&state_idle, &state_record2, BUTTON2_LONGSTART, nullptr);
    this->fsm.add_transition(&state_record2, &state_idle, BUTTON2_LONG_RELEASE, nullptr);
    this->fsm.add_transition(&state_idle, &state_record2, BUTTON3_LONGSTART, nullptr);
    this->fsm.add_transition(&state_record3, &state_idle, BUTTON3_LONG_RELEASE, nullptr);

    this->fsm.add_transition(&state_idle, &state_play, BUTTON_CLICK, nullptr);
    this->fsm.add_transition(&state_play, &state_idle, MESSAGE_PLAYED, nullptr);


    this->ui->onButtonEvent([this](ButtonEvent evt) {
    
        if (evt.action == Action::Clicked) {
            this->fsm.trigger(Event::BUTTON_CLICK);
        }

        if (evt.action == Action::LongPressStart) {
            switch (evt.buttonId)
            {
            case 1: this->fsm.trigger(Event::BUTTON1_LONGSTART); break;
            case 2: this->fsm.trigger(Event::BUTTON2_LONGSTART); break;
            case 3: this->fsm.trigger(Event::BUTTON3_LONGSTART); break;
            default:
                break;
            }
        }

        if (evt.action == Action::LongPressEnd) {
            switch (evt.buttonId)
            {
            case 1: this->fsm.trigger(Event::BUTTON1_LONG_RELEASE); break;
            case 2: this->fsm.trigger(Event::BUTTON2_LONG_RELEASE); break;
            case 3: this->fsm.trigger(Event::BUTTON3_LONG_RELEASE); break;
            default:
                break;
            }
        }
     });

    //this->fsm->add_transition(&Application::state_idle)
}

void Application::run() {
    ui->loop();
    fsm.run_machine();
}