#include <Arduino.h>

#include "UserInterface.h"
#include "Application.h"
#include "Fsm.h"
#include "FS.h"
#include "SD.h"
#include "SPIFFS.h"

UserInterface ui;
AudioRecorder recorder;
AudioPlayer player;

Application app(&ui, &recorder, &player);

void setup() {

  Serial.begin(115200);
  Serial.println();
  
  SD.begin();
  Serial.printf("Card total bytes: %i", SD.totalBytes());
  Serial.println();
  Serial.printf("Card total used:  %i", SD.usedBytes());
  Serial.println();

  SPIFFS.begin();
  Serial.printf("Data total bytes: %i", SPIFFS.totalBytes());
  Serial.println();
  Serial.printf("Data total used:  %i", SPIFFS.usedBytes());
  Serial.println();

  ui.setup();
  player.setup();
  recorder.setup();

  app.start();
}

void loop() {
  
  ui.loop();
  recorder.loop();
  player.loop();

  app.run();
}

