#include <Arduino.h>
#include "hardware.h"

#include "Application.h"

#include <SdFat.h>
SdFat sd;

BUTTON_PANEL_TYPE panel;

UserInterface ui(&panel, POT_IN, LDR_PIN);
AudioRecorder recorder(MIC_PIN_BCLK, MIC_PIN_LRCL, MIC_PIN_SD);
AUDIO_PLAYER_TYPE player(AMP_PIN_BCLK, AMP_PIN_LRC, AMP_PIN_DIN);

Application app(&ui, &recorder, &player);

void stats() {
    Serial.printf("\nPot: %i, Ldr: %i, PwrOn: %i\n", analogRead(POT_IN), analogRead(LDR_PIN), ui.buttonPanel()->isPowerButtonOn());
}

void setup() {
  
  Serial.begin(115200); 

  Serial.println("starting");

  if (!sd.begin(5)) {
    Serial.println("begin failed");
    return;
  }

  // Remove existing file.
   sd.remove("READTEST.TXT"); 
   
  // Create the file.
  auto file = sd.open("READTEST.TXT", 'w');
  if (!file) {
    Serial.println("open failed");
    return;
  }
  // Write test data.
  file.print(F("blblabla"));

  file.close();


  // sleep(36000000);

  // ui.setup();
  // player.setup();
  // recorder.setup();

  // app.setup();
}

long lastPrint = 0;

void loop() {

  if (millis() - lastPrint >= 5000) {
    lastPrint = millis();
    stats();
  }

  // app.run();
}

