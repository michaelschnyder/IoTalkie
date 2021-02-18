#include "AudioRecorder.h"

struct RecorderTaskParam
{
    File *file;
    AudioRecorder *instance;
};

void AudioRecorder::setup() 
{
  driverConfig = {
      .mode = i2s_mode_t(I2S_MODE_MASTER | I2S_MODE_RX), // Receive, not transfer
      .sample_rate = samplingRate,                         
      .bits_per_sample = i2s_bits_per_sample_t(samplingBits), // could only get it to work with 32bits
      .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
      .communication_format = i2s_comm_format_t(I2S_COMM_FORMAT_I2S | I2S_COMM_FORMAT_I2S_MSB),
      .intr_alloc_flags = 0,     
      .dma_buf_count = I2S_DMA_BUFFER_COUNT,               // relates somehow to resolution
      .dma_buf_len = I2S_DMA_BUFFER_SIZE,                  // maximum: 1024
      .use_apll = 1
  };

  pinConfig = {
      .bck_io_num = serialClockPin,      // Serial Clock (SCK)
      .ws_io_num = wordSelectPin,        // Word Select (WS)
      .data_out_num = I2S_PIN_NO_CHANGE, // not used (only for speakers)
      .data_in_num = dataPin             // Serial Data (SD)
  };

  bytesPerSecond = samplingRate * (samplingBits / 8) * numberOfChannels;
}

void AudioRecorder::fillHeader(byte* header, int wavSize){

  header[0] = 'R';
  header[1] = 'I';
  header[2] = 'F';
  header[3] = 'F';

  unsigned int fileSize = wavSize + WAVE_FORMAT_HEADER_SIZE - 8;
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
  

  header[28] = (byte)(bytesPerSecond & 0xFF);
  header[29] = (byte)((bytesPerSecond >> 8) & 0xFF);
  header[30] = (byte)((bytesPerSecond >> 16) & 0xFF);
  header[31] = (byte)((bytesPerSecond >> 24) & 0xFF);
  header[32] = 0x02; // 16bit monoral
  header[33] = 0x00;
  header[34] = samplingBits; 
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

AudioRecorder::AudioRecorder(int sckPin, int wsPin, int dataPin) 
{
    this->serialClockPin = sckPin;
    this->wordSelectPin = wsPin;
    this->dataPin = dataPin;
}

void AudioRecorder::writeHeader(File* file, int size) {
    byte header[WAVE_FORMAT_HEADER_SIZE];
    fillHeader(header, size);
    file->seek(0);
    file->write(header, WAVE_FORMAT_HEADER_SIZE);
}

void AudioRecorder::updateHeader(File* file, int size) {
    long currentPosition = file->position();
    writeHeader(file, size);
    file->seek(currentPosition);
}

void AudioRecorder_unwrapAndDelegateRecording(void *arg)
{
    RecorderTaskParam* args = (RecorderTaskParam*)arg;
    args->instance->_recordInternal(args->file);

    free(arg);

    vTaskDelete(NULL);
}

void AudioRecorder::record(File* file) 
{
    isRecording = true;
    hasStopped = false;

    logger.verbose(F("Recording has started with %i-bit @ %ihz (%i bytes/s)"), samplingBits, samplingRate, bytesPerSecond);
    
    RecorderTaskParam *params = (RecorderTaskParam*)malloc(sizeof(RecorderTaskParam));
    params->file = file;
    params->instance = this;
    targetFile = *file;

    xTaskCreate(AudioRecorder_unwrapAndDelegateRecording, "recordBackground", 4 * 1024, params, 1, NULL);
}

void AudioRecorder::_recordInternal(File* file) 
{
    esp_err_t err;

    writeHeader(file, 0);

    err = i2s_driver_install(port, &driverConfig, 0, NULL);
    if (err != ESP_OK) {
        Serial.printf("Failed installing driver: %d\n", err);
    }

    err = i2s_set_pin(port, &pinConfig);
    if (err != ESP_OK) {
        Serial.printf("Failed setting pin: %d\n", err);
    }

    logger.trace("I2S driver successfully installed.");

    int i2s_read_len = I2S_READ_CHUNK_SIZE;
    int flash_wr_size = 0;
    size_t bytes_read;

    char* i2s_read_buff = (char*) calloc(i2s_read_len, sizeof(char));
    
    while (isRecording) {

        i2s_read(port, (void*) i2s_read_buff, i2s_read_len, &bytes_read, portMAX_DELAY);       
        file->write((const byte*) i2s_read_buff, i2s_read_len);
        flash_wr_size += i2s_read_len;
        
        float durationInS = flash_wr_size * 1.0f / bytesPerSecond;
        logger.verbose(F("Recording duration: %.3fs"), durationInS);
        logger.verbose(F("Untouched stack size: %i (bytes)"), uxTaskGetStackHighWaterMark(NULL));
    }

    i2s_stop(port);

    i2s_driver_uninstall(port);

    free(i2s_read_buff);
    i2s_read_buff = NULL;

    updateHeader(file, flash_wr_size);

    hasStopped = true;
    logger.verbose("Recording stopped");
}

long AudioRecorder::stop() 
{
    logger.verbose("Stopping recording");

    isRecording = false;
    while (!hasStopped) { delay(10); }

    return targetFile.position() / (bytesPerSecond / 1000);
}

long AudioRecorder::duration() 
{
    return targetFile.position() / (bytesPerSecond / 1000);
}