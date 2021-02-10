#include <Arduino.h>

#include "UserInterface.h"
#include "Application.h"
#include "Fsm.h"

UserInterface ui;

Application app(&ui);

void setup() {

  Serial.begin(115200);
  
  ui.setup();

}

void loop() {
  
  app.run();
}

