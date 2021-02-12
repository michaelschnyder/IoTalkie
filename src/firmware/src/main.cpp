#include <Arduino.h>

#include "UserInterface.h"
#include "Application.h"
#include "Fsm.h"
#include "FS.h"
#include "SD.h"

UserInterface ui;
AudioRecorder recorder;
AudioPlayer player;

Application app(&ui, &recorder, &player);

void setup() {

  Serial.begin(115200);
  SD.begin();
  
  ui.setup();
  player.setup();
  recorder.setup();

}

void loop() {
  
  ui.loop();
  recorder.loop();
  player.loop();

  app.run();
}

