#include "FirmwareUpdater.h"

void FirmwareUpdater::download(const char* url, UPDATEPROGRESS_CALLBACK_SIGNATURE cb) {
    
    logger.trace("Starting to download firmware update from '%s'", url);
    cb(0);

    HTTPClient http;
    http.begin(url);

    int httpCode = http.GET();

    if (!(httpCode > 0)) {
        logger.error("failed to download file. Error: %s\n", http.errorToString(httpCode).c_str());
        return;
    }

    if(httpCode != HTTP_CODE_OK) {
        logger.error("failed to download file. Server responded with error: %s\n", http.errorToString(httpCode).c_str());
        return;
    }

    int totalSize = http.getSize();

    WiFiClient * stream = http.getStreamPtr();

    File f = SD.open(FW_DOWNLOAD_FILENAME, FILE_WRITE);
    uint8_t buff[1024] = { 0 };

    int remaining = totalSize;
    int previousDownloadPercent = 0;

    int totalUpdateProgress = 0;

    while(http.connected() && (remaining > 0 || remaining == -1)) {
        // get available data size
        size_t size = stream->available();

        if(size) {

            int c = stream->readBytes(buff, ((size > sizeof(buff)) ? sizeof(buff) : size));
            f.write(buff, c);

            if(remaining > 0) {
                remaining -= c;
            }
        }

        if (!remaining) {
            break;
        }

        long bytesStored = f.position();
        int percent = (int)((bytesStored * 100.0f) / totalSize);

        if (percent > previousDownloadPercent) {
            previousDownloadPercent = percent;
            logger.verbose("Download progress: %i/%i, %i%%.", bytesStored, totalSize, percent);
        }

        int newTotalUpdateProgress = percent / 2;
        if (newTotalUpdateProgress > totalUpdateProgress) {
            totalUpdateProgress = newTotalUpdateProgress;

            if (cb != NULL) {
                cb(newTotalUpdateProgress);
            }
        }
    }

    http.end();
    f.close();

    if (!remaining) {
        logger.verbose("Download completed. Renaming file and restarting...");
        SD.rename(FW_DOWNLOAD_FILENAME, FW_READY_FILENAME);
        this->downloadCompleted = true;
    }

    if (SD.exists(FW_DOWNLOAD_FILENAME)) {
        SD.remove(FW_DOWNLOAD_FILENAME);
    }
}

void FirmwareUpdater::flashPendingUpdate(UPDATEPROGRESS_CALLBACK_SIGNATURE cb) {
    
    logger.verbose("Update found. Starting update process...");
    cb(50);

    File file = SD.open(FW_READY_FILENAME, "r");
    bool canBegin = Update.begin(file.size(), U_FLASH, LED_BUILTIN);

    if (!canBegin) {
        Update.printError(Serial);
    }

    int progressInPercent = 0;

    int buffSize = 256;
    uint8_t ibuffer[buffSize];
    long start = millis();

    while (file.available()) {
        
        file.read((uint8_t *)ibuffer, buffSize);
        Update.write(ibuffer, sizeof(ibuffer));    

        int newProgress = file.position() * 100 / file.size();
        
        if (newProgress > progressInPercent) {
            progressInPercent = newProgress;

            cb(50 + progressInPercent / 2);
            logger.verbose("Flash progress: %i%% (%i/%i)", progressInPercent, file.position(), file.size());
        }
    }

    long duration = millis() - start;

    if(Update.end(true)) {
        logger.trace(F("Update successful. Time taken: %lims, buffer size: %i. Restarting system."), duration, buffSize);
    }

    file.close();
    SD.remove(FW_READY_FILENAME);
}