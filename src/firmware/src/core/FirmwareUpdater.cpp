#include "FirmwareUpdater.h"

void FirmwareUpdater::download(const char* url, UPDATEPROGRESS_CALLBACK_SIGNATURE updateProgressCb) {
    
    logger.trace("Starting to download firmware update from '%s'", url);

    updateProgressCb(0);

    bool isCompleted = false, isSuccessful = false;
    int totalUpdateProgress = 0;

    downloader.download(url, FW_DOWNLOAD_FILENAME, 
        [&isCompleted, &isSuccessful](bool result) { isCompleted = true; isSuccessful = result; }, 
        [&totalUpdateProgress, updateProgressCb](ulong completed, ulong total) { 

            int percent = (int)((completed * 100.0f) / total);
            int newTotalUpdateProgress = percent / 2;

            if (newTotalUpdateProgress > totalUpdateProgress) {
                totalUpdateProgress = newTotalUpdateProgress;
                updateProgressCb(totalUpdateProgress);
            }
        });

    while (!isCompleted) {
        yield();
    }

    if (isSuccessful) {
        logger.verbose("Download completed. Renaming file and restarting...");
        SD.rename(FW_DOWNLOAD_FILENAME, FW_READY_FILENAME);
        this->downloadCompleted = true;
    }

    if (SD.exists(FW_DOWNLOAD_FILENAME)) {
        SD.remove(FW_DOWNLOAD_FILENAME);
    }
}

void FirmwareUpdater::flashPendingUpdate(UPDATEPROGRESS_CALLBACK_SIGNATURE updateProgressCb) {
    
    logger.verbose("Update found. Starting update process...");
    updateProgressCb(50);

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

            updateProgressCb(50 + progressInPercent / 2);
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