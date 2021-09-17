#include <Arduino.h>
#include "hardware.h"

#include "Application.h"

BUTTON_PANEL_TYPE panel;

UserInterface ui(&panel, POT_IN, LDR_PIN);
AudioRecorder recorder(MIC_PIN_BCLK, MIC_PIN_LRCL, MIC_PIN_SD);
AUDIO_PLAYER_TYPE player(AMP_PIN_BCLK, AMP_PIN_LRC, AMP_PIN_DIN);
FileUploader uploader;

Application app(&ui, &recorder, &player, &uploader);

void stats() {
    Serial.printf("\nPot: %i, Ldr: %i, PwrOn: %i\n", analogRead(POT_IN), analogRead(LDR_PIN), ui.buttonPanel()->isPowerButtonOn());
}

void setup() {
  
  ui.setup();
  player.setup();
  recorder.setup();

  app.setup();
}

long lastPrint = 0;

void loop() {

  if (millis() - lastPrint >= 5000) {
    lastPrint = millis();
    stats();
  }

  app.run();
}

