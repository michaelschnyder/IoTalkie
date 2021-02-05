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

#include "driver/i2s.h"

#define NUM_LEDS 24


#define I2S_PORT I2S_NUM_1
#define I2S_SAMPLE_RATE   (16000)
#define I2S_SAMPLE_BITS   (32)
#define I2S_READ_LEN      (16 * 1024)
#define RECORD_TIME       (10) //Seconds
#define I2S_CHANNEL_NUM   (1)
#define FLASH_RECORD_SIZE (I2S_CHANNEL_NUM * I2S_SAMPLE_RATE * I2S_SAMPLE_BITS / 8 * RECORD_TIME)

File file;
const char filename[] = "/recording.wav";
const int headerSize = 44;

void wavHeader(byte* header, int wavSize){
  header[0] = 'R';
  header[1] = 'I';
  header[2] = 'F';
  header[3] = 'F';
  unsigned int fileSize = wavSize + headerSize - 8;
  header[4] = (byte)(fileSize & 0xFF);
  header[5] = (byte)((fileSize >> 8) & 0xFF);
  header[6] = (byte)((fileSize >> 16) & 0xFF);
  header[7] = (byte)((fileSize >> 24) & 0xFF);
  header[8] = 'W';
  header[9] = 'A';
  header[10] = 'V';
  header[11] = 'E';
  header[12] = 'f';
  header[13] = 'm';
  header[14] = 't';
  header[15] = ' ';
  header[16] = 0x10; // linear PCM
  header[17] = 0x00;
  header[18] = 0x00;
  header[19] = 0x00;
  header[20] = 0x01; // linear PCM
  header[21] = 0x00;
  header[22] = 0x01; // monoral
  header[23] = 0x00;
//  header[24] = 0x44;  // sampling rate 44100
//  header[25] = 0xAC;  // sampling rate 44100
  header[24] = 0x80; // sampling rate 16000
  header[25] = 0x3E; // sampling rate 16000
  header[26] = 0x00;
  header[27] = 0x00;
  header[28] = 0x00; // Bytes/s = 16000 (Sampling) * (32 bit / 8) x 1 channel = 
  header[29] = 0xFA;
  header[30] = 0x00;
  header[31] = 0x00;
//  header[28] = 0x00;
//  header[29] = 0x7D;
//  header[30] = 0x00;
//  header[31] = 0x00;
  header[32] = 0x02; // 16bit monoral
  header[33] = 0x00;
  header[34] = 0x20; // 32bit resolution
  header[35] = 0x00;
  header[36] = 'd';
  header[37] = 'a';
  header[38] = 't';
  header[39] = 'a';
  header[40] = (byte)(wavSize & 0xFF);
  header[41] = (byte)((wavSize >> 8) & 0xFF);
  header[42] = (byte)((wavSize >> 16) & 0xFF);
  header[43] = (byte)((wavSize >> 24) & 0xFF);
  
}


// Define the array of leds
CRGB leds[NUM_LEDS];

const int LED_RING = 12; // White (Input does not work on this PIN)
const int POT_IN = 13;   // Grey 

const int SD_SPI_CS = 5;    // White (Slave Select)
const int SD_SPI_CLK = 18;  // Violett (Clock)
const int SD_SPI_MISO = 19; // Green (Input)
const int SD_SPI_MOSI = 23; // Brown (Output)

const int AMP_PIN_BCLK = 25; // Orange (Serial Clock)
const int AMP_PIN_LRC = 33;  // Green (Word / LeftRight / Channel Select)
const int AMP_PIN_DIN = 32; // Blue (Serial Data)

const int MIC_PIN_BCLK = 17; // Yellow (Serial Clock)
const int MIC_PIN_LRCL = 21; // Green (Word / Channel Select)
const int MIC_PIN_SD = 22; // Blue (Serial Data)

int potValue; // do not change
float voltage = 0;// do not change

int freq = 5000;
int ledChannel = 0;
int resolution = 8;


AudioFileSourcePROGMEM *in;
AudioGeneratorMP3 *audio;
AudioOutputI2S *out;
long start = 0;

void i2s_adc(void *arg)
{
    
    int i2s_read_len = I2S_READ_LEN;
    int flash_wr_size = 0;
    size_t bytes_read;

    char* i2s_read_buff = (char*) calloc(i2s_read_len, sizeof(char));
    
    Serial.println(" *** Recording Start *** ");
    while (flash_wr_size < FLASH_RECORD_SIZE) {

        i2s_read(I2S_PORT, (void*) i2s_read_buff, i2s_read_len, &bytes_read, portMAX_DELAY);       
        file.write((const byte*) i2s_read_buff, i2s_read_len);
        flash_wr_size += i2s_read_len;
        ets_printf("Sound recording %u%%\n", flash_wr_size * 100 / FLASH_RECORD_SIZE);
        ets_printf("Never Used Stack Size: %u\n", uxTaskGetStackHighWaterMark(NULL));
    }
    file.close();

    free(i2s_read_buff);
    i2s_read_buff = NULL;

    vTaskDelete(NULL);
}

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

  if (SD.exists(filename)) {
    SD.remove(filename);
  }
  file = SD.open(filename, FILE_WRITE);
  if(!file){
    Serial.println("File is not available!");
  }

  byte header[headerSize];
  wavHeader(header, FLASH_RECORD_SIZE);
  file.write(header, headerSize);
  
  esp_err_t err;

  // The I2S config as per the example
  const i2s_config_t i2s_config = {
      .mode = i2s_mode_t(I2S_MODE_MASTER | I2S_MODE_RX), // Receive, not transfer
      .sample_rate = I2S_SAMPLE_RATE,                         // 16KHz
      .bits_per_sample = i2s_bits_per_sample_t(I2S_SAMPLE_BITS), // could only get it to work with 32bits
      .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT, // use both channel
      .communication_format = i2s_comm_format_t(I2S_COMM_FORMAT_I2S | I2S_COMM_FORMAT_I2S_MSB),
      //.intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,     // Interrupt level 1
      .intr_alloc_flags = 0,     
      .dma_buf_count = 32,               // relates somehow to resolution
      .dma_buf_len = 1024,                // maximum
      .use_apll = 1
  };

    // The pin config as per the setup
  const i2s_pin_config_t pin_config = {
      .bck_io_num = MIC_PIN_BCLK,   // Serial Clock (SCK)
      .ws_io_num = MIC_PIN_LRCL,    // Word Select (WS)
      .data_out_num = I2S_PIN_NO_CHANGE, // not used (only for speakers)
      .data_in_num = MIC_PIN_SD   // Serial Data (SD)
  };

   // Configuring the I2S driver and pins.
  // This function must be called before any I2S driver read/write operations.
  err = i2s_driver_install(I2S_PORT, &i2s_config, 0, NULL);
  if (err != ESP_OK) {
    Serial.printf("Failed installing driver: %d\n", err);
    while (true);
  }
  err = i2s_set_pin(I2S_PORT, &pin_config);
  if (err != ESP_OK) {
    Serial.printf("Failed setting pin: %d\n", err);
    while (true);
  }
  Serial.println("I2S driver installed.");
  start = millis();

  xTaskCreate(i2s_adc, "i2s_adc", 1024 * 2, NULL, 1, NULL);
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
    Serial.println("Audio");
  }
}