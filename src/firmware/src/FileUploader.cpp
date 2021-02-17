#include "FileUploader.h"

typedef struct UploadTaskParam
{
    File *file;
    char *url;
    FileUploader *instance;
};

void sendBackground(void *arg)
{
    UploadTaskParam* args = (UploadTaskParam*)arg;
    args->instance->_sendInternal(args->file, args->url);

    free(args->url);
    free(arg);
    vTaskDelete(NULL);
}

void FileUploader::_sendInternal(File* file , const char* url) 
{
    logger.trace("Upload Url: %s, Size: %i", url, file->size());

    HTTPClient client;

    client.begin(url);
    client.setUserAgent("IoTalkie/Arduino");
    client.addHeader("clientId", "abcdefg");

    int responseCode = client.sendRequest("POST", file, file->size());
    client.end();
    
    logger.verbose("Upload completed with result: %i", responseCode);
    delay(1);
    this->completed = true;
    this->successful = (responseCode >= 200 && responseCode < 300);
}

void FileUploader::send(File* file, const char* url) 
{
    this->completed = false;
    this->successful = false;
    this->currentFile = file;

    UploadTaskParam *struct1 = (UploadTaskParam*)malloc(sizeof(UploadTaskParam));
    struct1->file = file;
    struct1->url = (char*)malloc(strlen(url) +1);
    struct1->instance = this;

    strcpy(struct1->url, url);

    xTaskCreate(sendBackground, "sendBackground", 2 * 1024, struct1, 1, NULL);
}

bool FileUploader::isCompleted() 
{
    return this->completed;
}

bool FileUploader::isSuccessful() 
{
    return this->successful;
}

long FileUploader::getBytesSent() 
{
    return this->currentFile->position();
}

long FileUploader::getBytesTotal() 
{
    return this->currentFile->size();
}
