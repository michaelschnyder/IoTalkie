#include <Arduino.h>
#include "hardware.h"

#include "Application.h"

BUTTON_PANEL_TYPE panel;

Screen screen;
UserInterface ui(&panel, POT_IN, LDR_PIN);
AudioRecorder recorder(MIC_PIN_BCLK, MIC_PIN_LRCL, MIC_PIN_SD);
AudioPlayer player(AMP_PIN_BCLK, AMP_PIN_LRC, AMP_PIN_DIN);
FileUploader uploader;

Application app(&ui, &recorder, &player, &uploader);

void stats() {
    Serial.println();
    Serial.printf("Pot: %i, Ldr: %i, PwrOn: %i", analogRead(POT_IN), analogRead(LDR_PIN), ui.buttonPanel()->isPowerButtonOn());
    Serial.printf("\nHeap size: %d\n", ESP.getHeapSize());
    Serial.printf("Free Heap: %d\n", esp_get_free_heap_size());
    Serial.printf("Min Free Heap: %d\n", esp_get_minimum_free_heap_size());
    Serial.printf("Largest Free block: %d\n", heap_caps_get_largest_free_block(MALLOC_CAP_8BIT));
}

void setup() {
  
  screen.setup();
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

