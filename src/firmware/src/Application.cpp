#include "Application.h"

Application::Application(UserInterface* ui) : Application()
{
    this->ui = ui;
    
    this->fsm.add_transition(&state_idle, &state_record, BUTTON_LONGSTART, nullptr);
    this->fsm.add_transition(&state_record, &state_idle, BUTTON_LONG_RELEASE, nullptr);

    this->fsm.add_transition(&state_idle, &state_play, BUTTON_CLICK, nullptr);
    this->fsm.add_transition(&state_play, &state_idle, MESSAGE_PLAYED, nullptr);


    this->ui->onButtonEvent([this](ButtonEvent evt) {
    
        if (evt.action == Action::Clicked) {
            this->fsm.trigger(Event::BUTTON_CLICK);
        }

        if (evt.action == Action::LongPressStart) {
            this->fsm.trigger(Event::BUTTON_LONGSTART);
        }

        if (evt.action == Action::LongPressEnd) {
            this->fsm.trigger(Event::BUTTON_LONG_RELEASE);
        }
     });

    //this->fsm->add_transition(&Application::state_idle)
}

void Application::run() {
    ui->loop();
    fsm.run_machine();
}