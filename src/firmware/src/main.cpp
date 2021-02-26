#include <Arduino.h>

#include "pins.h"

#include "UserInterface.h"
#include "Application.h"

UserInterface ui;
AudioRecorder recorder(MIC_PIN_BCLK, MIC_PIN_LRCL, MIC_PIN_SD);
AudioPlayer player;
FileUploader uploader;

Application app(&ui, &recorder, &player, &uploader);

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

    Serial.println();
    Serial.printf("Pot: %i, Ldr: %i, PwrOff: %i", analogRead(POT_IN), analogRead(LDR_PIN), digitalRead(BUTTON_OFF_IN));
    Serial.printf("\nHeap size: %d\n", ESP.getHeapSize());
    Serial.printf("Free Heap: %d\n", esp_get_free_heap_size());
    Serial.printf("Min Free Heap: %d\n", esp_get_minimum_free_heap_size());
    Serial.printf("Largest Free block: %d\n", heap_caps_get_largest_free_block(MALLOC_CAP_8BIT));
  }

  app.run();
}

