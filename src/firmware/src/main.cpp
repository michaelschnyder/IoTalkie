#include <Arduino.h>

#include "pins.h"

#include "UserInterface.h"
#include "Application.h"
#include "Fsm.h"
#include "FS.h"
#include "SD.h"
#include "SPIFFS.h"

UserInterface ui;
AudioRecorder recorder(MIC_PIN_BCLK, MIC_PIN_LRCL, MIC_PIN_SD);
AudioPlayer player;
FileUploader uploader;

Application app(&ui, &recorder, &player, &uploader);

String file_size(uint64_t bytes){
  String fsize = "";
  if (bytes < 1024)                 fsize = String((long)bytes)+" B";
  else if(bytes < (1024*1024))      fsize = String(bytes/1024.0,3)+" KB";
  else if(bytes < (1024*1024*1024)) fsize = String(bytes/1024.0/1024.0,3)+" MB";
  else                              fsize = String(bytes/1024.0/1024.0/1024.0,3)+" GB";
  return fsize;
}

void setup() {

  Serial.begin(115200);
  Serial.println();
  
  SPIFFS.begin();
  Serial.printf("Storage: %s of %s used", file_size(SPIFFS.usedBytes()).c_str(), file_size(SPIFFS.totalBytes()).c_str());
  Serial.println();

  while(!SD.begin()) {
    Serial.print('.');
    delay(250);
  };

  Serial.printf("SD card:  %s of %s used", file_size(SD.usedBytes()).c_str(), file_size(SD.totalBytes()).c_str());
  Serial.println();

  ui.setup();
  player.setup();
  recorder.setup();

  app.start();
}

long lastPrint = 0;

void loop() {

  if (millis() - lastPrint >= 1000) {
    lastPrint = millis();

  //   Serial.printf("Pot: %i, Ldr: %i", analogRead(POT_IN), analogRead(LDR_PIN));
  //   Serial.println();
    // Serial.printf("\nHeap size: %d\n", ESP.getHeapSize());
    // Serial.printf("Free Heap: %d\n", esp_get_free_heap_size());
    // Serial.printf("Min Free Heap: %d\n", esp_get_minimum_free_heap_size());
    // Serial.printf("Largest Free block: %d\n", heap_caps_get_largest_free_block(MALLOC_CAP_8BIT));
  }

  ui.loop();
  player.loop();

  app.run();
}

