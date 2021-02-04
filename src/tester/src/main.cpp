#include <Arduino.h>

#include <WiFiClient.h>
#include <SD.h>

#include <FastLED.h>
//#include "AudioGeneratorAAC.h"
#include "AudioGeneratorMP3.h"
#include "AudioOutputI2S.h"
#include "AudioFileSourcePROGMEM.h"
#include "sampleaac.h"
#include "samplemp3.h"

#define NUM_LEDS 24

// Define the array of leds
CRGB leds[NUM_LEDS];

const int LED_RING = 12; // White (Input does not work on this PIN)
const int POT_IN = 13;   // Grey 

// const int AMP_PIN_DATA = 25; // Orange (Data)
// const int AMP_PIN_BCLK = 33; // Green (Serial Clock)
// const int AMP_PIN_LRC = 32; // Blue (Word / Channel Select)

const int SD_SPI_CS = 5;    // White (Slave Select)
const int SD_SPI_CLK = 18;  // Violett (Clock)
const int SD_SPI_MISO = 19; // Green (Input)
const int SD_SPI_MOSI = 23; // Brown (Output)

const int AMP_PIN_BCLK = 25; // Orange (Serial Clock)
const int AMP_PIN_LRC = 33;  // Green (Word / LeftRight / Channel Select)
const int AMP_PIN_DIN = 32; // Blue (Data in)

// const int MIC_PIN_DOUT = 21; // Yellow (Data)
// const int MIC_PIN_BCLK = 22; // Green (Serial Clock)
// const int MIC_PIN_LRCL = 23; // Blue (Word / Channel Select)

int potValue; // do not change
float voltage = 0;// do not change

int freq = 5000;
int ledChannel = 0;
int resolution = 8;

AudioFileSourcePROGMEM *in;
AudioGeneratorMP3 *audio;
AudioOutputI2S *out;

void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);


  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on 
  delay(500);                       // wait for half a second
  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off 
  delay(500);                       // wait for half a second
  


  ledcSetup(ledChannel, freq, resolution);
  ledcAttachPin(LED_BUILTIN, ledChannel);

  FastLED.addLeds<WS2811, LED_RING, GRB>(leds, NUM_LEDS);

  leds[0] = CRGB::Red;
  FastLED.show();
  delay(300);
  leds[0] = CRGB::Green;
  FastLED.show();
  delay(300);
  leds[0] = CRGB::Blue;
  FastLED.show();
  delay(300);
  FastLED.clear();

  // Serial.begin(115200);

  audioLogger = &Serial;
  // in = new AudioFileSourcePROGMEM(sampleaac, sizeof(sampleaac));
  in = new AudioFileSourcePROGMEM(samplemp3, sizeof(samplemp3));
  //audio = new AudioGeneratorAAC();
  audio = new AudioGeneratorMP3();
  out = new AudioOutputI2S();
  out -> SetGain(0.128);
  out -> SetPinout(AMP_PIN_BCLK, AMP_PIN_LRC, AMP_PIN_DIN);
  audio->begin(in, out);  


  // Initialize SD card
  SD.begin(SD_SPI_CS);  
  if(!SD.begin(SD_SPI_CS)) {
    Serial.println("Card Mount Failed");
    return;
  }
  uint8_t cardType = SD.cardType();
  if(cardType == CARD_NONE) {
    Serial.println("No SD card attached");
    return;
  }
  Serial.println("Initializing SD card...");
  if (!SD.begin(SD_SPI_CS)) {
    Serial.println("ERROR - SD card initialization failed!");
    return;    // init failed
  }

  Serial.println("SD Card ready");

}

void loop() {

  if (millis() % 10 == 0) {

    // read the input on analog pin potPin:
    potValue = analogRead(POT_IN);
    voltage = (3.3/4095.0) * potValue;
    
    
    // Serial.print("potValue:");
    // Serial.print(potValue);
    
    // Serial.print(" Voltage:");
    // Serial.print(voltage);
    // Serial.println("V");  

    int ledLevel = potValue / 16;
    ledcWrite(ledChannel, ledLevel);

    // static uint8_t hue = 0;
    //FastLED.showColor(CHSV(hue++, 128, ledLevel)); 
    FastLED.showColor(CHSV(ledLevel, 255, 255)); 
  }

  if (audio->isRunning()) {
    audio->loop();
  } else {
    audio -> stop();
  }
}