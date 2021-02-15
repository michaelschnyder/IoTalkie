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
    args->instance->sendInternal(args->file, args->url);

    free(args->url);
    free(arg);
    vTaskDelete(NULL);
}

void FileUploader::sendInternal(File* file , const char* url) 
{
    logger.trace("Upload Url: %s, Size: %i", url, file->size());

    HTTPClient client;

    client.begin(url);
    client.setUserAgent("IoTalkie/Arduino");
    client.addHeader("clientId", "abcdefg");

    client.sendRequest("POST", file, file->size());
    client.end();
    logger.verbose("Upload completed");
    delay(1);
    this->completed = true;
}

void FileUploader::send(File* file, const char* url) 
{
    this->completed = false;
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

long FileUploader::getBytesSent() 
{
    return this->currentFile->position();
}

long FileUploader::getBytesTotal() 
{
    return this->currentFile->size();
}
