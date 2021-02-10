#include "Application.h"

Application::Application(UserInterface ui) 
{
    this->ui = ui;
}

void Application::run() {
    ui.loop();
}