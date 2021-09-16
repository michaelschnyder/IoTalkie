#include <Arduino.h>
#include "hardware.h"

#include "Application.h"

BUTTON_PANEL_TYPE panel;

UserInterface ui(&panel, POT_IN, LDR_PIN);
AudioRecorder recorder(MIC_PIN_BCLK, MIC_PIN_LRCL, MIC_PIN_SD);
AudioPlayer player(AMP_PIN_BCLK, AMP_PIN_LRC, AMP_PIN_DIN);
FileUploader uploader;

Application app(&ui, &recorder, &player, &uploader);

#include <Adafruit_GFX.h>
#include <GxEPD2_BW.h> // including both doesn't use more code or ram
#include <GxEPD2_3C.h> // including both doesn't use more code or ram
#include "display_selection.h"

SPIClass otherSPI(HSPI);

void setup() {

  // Separate SPI Bus
  // SPIClass spi2(HSPI);

  //same bus and Pins than SD
  // display.init(115200, spi2);

  // (swapped MISO/MOSI)
  // otherSPI.begin(14, 13, 12, 15);
  
  display.init(115200, 14, 26, 27, 15, otherSPI);

  // comment out next line to have no or minimal Adafruit_GFX code
  display.setTextColor(GxEPD_BLACK);
  display.firstPage();
  do
  {
    display.fillScreen(GxEPD_WHITE);
    // comment out next line to have no or minimal Adafruit_GFX code
    display.print("Hello World!");
  }
  while (display.nextPage());
  
  // display.hibernate();

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
    Serial.printf("Pot: %i, Ldr: %i, PwrOn: %i", analogRead(POT_IN), analogRead(LDR_PIN), ui.buttonPanel()->isPowerButtonOn());
    Serial.printf("\nHeap size: %d\n", ESP.getHeapSize());
    Serial.printf("Free Heap: %d\n", esp_get_free_heap_size());
    Serial.printf("Min Free Heap: %d\n", esp_get_minimum_free_heap_size());
    Serial.printf("Largest Free block: %d\n", heap_caps_get_largest_free_block(MALLOC_CAP_8BIT));
  }

  app.run();
}

