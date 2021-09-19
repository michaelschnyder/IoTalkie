#include "FileDownloader.h"

void _FileDownloaderBackgroundTask(void *arg)
{
    DownloadTask* task = (DownloadTask*)arg;
    task->getDownloader()->__downloadInternal(arg);

    delete task;
    vTaskDelete(NULL);
}

void FileDownloader::download(const char * url, File * file, DOWNLOAD_PROGRESS_CB progressCb = NULL, DOWNLOAD_COMPLETED_CB onCompletedCb = NULL, DOWNLOAD_FAILED_CB onFailedCb = NULL) {
    
    auto task = new DownloadTask(this, url, file, progressCb, onCompletedCb, onFailedCb);   
    xTaskCreate(_FileDownloaderBackgroundTask, "_FileDownloaderBackgroundTask", TASK_STACK_DEPT, task, 1, NULL);
}

void FileDownloader::__downloadInternal(void *arg) {
    DownloadTask* task = (DownloadTask*)arg;

    auto url = task->getUrl();
    auto file = task->getFile();

    logger.trace(F("Starting to download from '%s'"), url);

    HTTPClient http;
    http.begin(url);

    int httpCode = http.GET();

    if (!(httpCode > 0)) {
        logger.error(F("failed to download file. Error: %s\n"), http.errorToString(httpCode).c_str());
        return;
    }

    if(httpCode != HTTP_CODE_OK) {
        logger.error(F("failed to download file. Server responded with error: %s\n"), http.errorToString(httpCode).c_str());
        return;
    }

    int totalSize = http.getSize();

    WiFiClient * stream = http.getStreamPtr();

    uint8_t buff[DOWNLOAD_BUFFER_SIZE] = { 0 };

    int remaining = totalSize;
    int previousDownloadPercent = 0;

    while(http.connected() && (remaining > 0 || remaining == -1)) {
        
        // get available data size
        size_t size = stream->available();

        if(size) {

            int c = stream->readBytes(buff, ((size > sizeof(buff)) ? sizeof(buff) : size));
            file->write(buff, c);

            if(remaining > 0) {
                remaining -= c;
            }
        }

        if (!remaining) {
            break;
        }

        long bytesStored = file->position();
        task->reportProgress(bytesStored, totalSize);

        int percent = (int)((bytesStored * 100.0f) / totalSize);

        if (percent > previousDownloadPercent) {
            previousDownloadPercent = percent;
            logger.verbose(F("Download progress: %i/%i, %i%%."), bytesStored, totalSize, percent);
        }
    }

    http.end();
    file->close();

    if (!remaining) {
        task->reportCompleted();
    }
    else {
        task->reportFailed();
    }
}