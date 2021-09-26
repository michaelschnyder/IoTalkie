#include "TaskHTTP.h"

void _FileDownloaderBackgroundTask(void *arg)
{
    DownloadTask* task = (DownloadTask*)arg;
    task->getInstance()->__downloadInternal(arg);

    delete task;
    vTaskDelete(NULL);
}

void _FileUploaderBackgroundTask(void *arg)
{
    UploadTask* task = (UploadTask*)arg;
    task->getInstance()->__uploadInternal(arg);

    delete task;
    vTaskDelete(NULL);
}

void TaskHTTPImpl::download(const char * url, const char * file, DOWNLOAD_COMPLETED_CB completedCb, DOWNLOAD_PROGRESS_CB progressCb) {
    auto task = new DownloadTask(this, url, file, completedCb, progressCb);
    xTaskCreate(_FileDownloaderBackgroundTask, "_FileDownloaderBackgroundTask", TASK_STACK_DEPT, task, 1, NULL);
}

void TaskHTTPImpl::download(const char * url, File * file, DOWNLOAD_COMPLETED_CB completedCb, DOWNLOAD_PROGRESS_CB progressCb) {
    auto task = new DownloadTask(this, url, file, completedCb, progressCb);   
    xTaskCreate(_FileUploaderBackgroundTask, "_FileDownloaderBackgroundTask", TASK_STACK_DEPT, task, 1, NULL);
}

void TaskHTTPImpl::upload(const char * file, const char * url, UPLOAD_COMPLETED_CB completedCb, UPLOAD_PROGRESS_CB progressCb) {
    auto task = new UploadTask(this, url, file, completedCb, progressCb);
    xTaskCreate(_FileUploaderBackgroundTask, "_FileUploaderBackgroundTask", TASK_STACK_DEPT, task, 1, NULL);
}

void TaskHTTPImpl::upload(File * file, const char * url, UPLOAD_COMPLETED_CB completedCb, UPLOAD_PROGRESS_CB progressCb) {
    auto task = new UploadTask(this, url, file, completedCb, progressCb);   
    xTaskCreate(_FileUploaderBackgroundTask, "_FileUploaderBackgroundTask", TASK_STACK_DEPT, task, 1, NULL);
}

char TaskHTTPImpl::_userAgent[128] = "IoTalkie/Unknown";
char TaskHTTPImpl::_clientId[128] = "Unknown";


void TaskHTTPImpl::setUserAgent(const char* userAgent) {
    strncpy(_userAgent, userAgent, sizeof(_userAgent));
}

void TaskHTTPImpl::setClientId(const char* clientId) {
    strncpy(_clientId, clientId, sizeof(_clientId));
}

void TaskHTTPImpl::__downloadInternal(void *arg) {
    DownloadTask* task = (DownloadTask*)arg;

    auto url = task->getUrl();
    auto file = task->getFile();

    logger.trace(F("Starting to download from '%s'"), url);

    HTTPClient http;
    http.setUserAgent(_userAgent);
    http.addHeader("clientId", _clientId);

    http.begin(url);
    
    int httpCode = http.GET();

    if (!(httpCode > 0)) {
        logger.error(F("failed to download file. Error: %s\n"), http.errorToString(httpCode).c_str());
        task->reportCompleted(false);
        return;
    }

    if(httpCode != HTTP_CODE_OK) {
        logger.error(F("failed to download file. Server responded with error: %s\n"), http.errorToString(httpCode).c_str());
        task->reportCompleted(false);
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

        long bytesStored = file->position();
        task->reportProgress(bytesStored, totalSize);

        int percent = (int)((bytesStored * 100.0f) / totalSize);

        if (percent > previousDownloadPercent) {
            previousDownloadPercent = percent;
            logger.verbose(F("Download progress: %i/%i, %i%%."), bytesStored, totalSize, percent);
        }
    }

    http.end();
    task->reportCompleted(!remaining);
}

void TaskHTTPImpl::__uploadInternal(void *arg) {
    UploadTask* task = (UploadTask*)arg;

    auto file = task->getFile();
    auto size = file->size();

    logger.trace("Upload Url: %s, Size: %i", task->getUrl(), size);

    HTTPClientEx client;

    client.begin(task->getUrl());
    
    client.setUserAgent(_userAgent);
    client.addHeader("clientId", _clientId);

    int previousUploadPercent = 0;

    int responseCode = client.sendRequest("POST", file, size, [task, &previousUploadPercent, this](long sent, long total) {
        task->reportProgress(sent, total);

        int percent = (int)((sent * 100.0f) / total);

        if (percent > previousUploadPercent) {
            previousUploadPercent = percent;
            logger.verbose(F("Upload progress: %i/%i, %i%%."), sent, total, percent);
        }
    });

    client.end();
    
    logger.verbose("Upload completed with result: %i", responseCode);
    delay(1);

    task->reportCompleted((responseCode >= 200 && responseCode < 300));
}