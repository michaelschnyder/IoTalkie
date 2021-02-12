#include "AudioRecorder.h"
#include "pins.h"

#define I2S_PORT I2S_NUM_1
#define I2S_SAMPLE_RATE   (16000)
#define I2S_SAMPLE_BITS   (32)
#define I2S_READ_LEN      (16 * 1024)
#define I2S_CHANNEL_NUM   (1)
#define BYTES_PER_SECOND (I2S_CHANNEL_NUM * I2S_SAMPLE_RATE * I2S_SAMPLE_BITS / 8)

const int headerSize = 44;

void AudioRecorder::loop() 
{
    
}

void AudioRecorder::setup() 
{
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
      .dma_buf_len = 512,                // maximum: 1024
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
  i2s_stop(I2S_PORT);
}

void fillHeader(byte* header, int wavSize, int samplingRate, uint8_t resolution){

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

  header[24] = (byte)(samplingRate & 0xFF);
  header[25] = (byte)((samplingRate >> 8) & 0xFF);
  header[26] = (byte)((samplingRate >> 16) & 0xFF);
  header[27] = (byte)((samplingRate >> 24) & 0xFF);
  
  unsigned int channels = 1;
  unsigned int bytesPerSecond = samplingRate * (resolution / 8) * channels;

  header[28] = (byte)(bytesPerSecond & 0xFF);
  header[29] = (byte)((bytesPerSecond >> 8) & 0xFF);
  header[30] = (byte)((bytesPerSecond >> 16) & 0xFF);
  header[31] = (byte)((bytesPerSecond >> 24) & 0xFF);
  header[32] = 0x02; // 16bit monoral
  header[33] = 0x00;
  header[34] = resolution; 
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

bool isRecording = false;
bool hasStopped = false;
File currentRecording;

void i2s_adc(void *arg)
{
    i2s_start(I2S_PORT);

    //File* file = reinterpret_cast<File*>(arg);
    currentRecording.getTimeout();

    int i2s_read_len = I2S_READ_LEN;
    int flash_wr_size = 0;
    size_t bytes_read;

    char* i2s_read_buff = (char*) calloc(i2s_read_len, sizeof(char));
    
    Serial.println(" *** Recording Start *** ");
    while (isRecording) {

        i2s_read(I2S_PORT, (void*) i2s_read_buff, i2s_read_len, &bytes_read, portMAX_DELAY);       
        currentRecording.write((const byte*) i2s_read_buff, i2s_read_len);
        flash_wr_size += i2s_read_len;
        ets_printf("Sound recording %i\n", flash_wr_size / BYTES_PER_SECOND);
        ets_printf("Never Used Stack Size: %u\n", uxTaskGetStackHighWaterMark(NULL));
    }

    Serial.println(" *** Recording End *** ");

    byte header[headerSize];
    fillHeader(header, flash_wr_size, I2S_SAMPLE_RATE, I2S_SAMPLE_BITS);
    currentRecording.seek(0);
    currentRecording.write(header, headerSize);
    currentRecording.close();

    free(i2s_read_buff);
    i2s_read_buff = NULL;
    i2s_stop(I2S_PORT);

    hasStopped = true;
    vTaskDelete(NULL);
}

void AudioRecorder::record(File* file) 
{
    byte header[headerSize];

    currentRecording = *file;

    fillHeader(header, 0, I2S_SAMPLE_RATE, I2S_SAMPLE_BITS);
    currentRecording.write(header, headerSize);

    isRecording = true;
    hasStopped = false;

    xTaskCreate(i2s_adc, "i2s_adc", 2 * 1024, file, 1, NULL);
}

void AudioRecorder::stop() 
{
    isRecording = false;
    while (!hasStopped) { }
}

