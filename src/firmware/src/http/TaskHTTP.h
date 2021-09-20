#ifndef __TASKHTTP_H__
#define __TASKHTTP_H__

#include <Log4Esp.h>

#include <WiFiClient.h>
#include <HTTPClient.h>
#include <SD.h>

#include "HTTPClientEx.h"

#define DOWNLOAD_PROGRESS_CB std::function<void(unsigned long int, unsigned long int)> 
#define DOWNLOAD_COMPLETED_CB std::function<void(bool)>

#define UPLOAD_PROGRESS_CB std::function<void(unsigned long int, unsigned long int)> 
#define UPLOAD_COMPLETED_CB std::function<void(bool)>

// Increase this value if you get exceptions similar to "Debug exception reason: Stack canary watchpoint triggered (_FileDownloaderBackgroundTask)"
// HTTPS connections seemd to need around 8kb
#define TASK_STACK_DEPT 8 * 1024

// Size of chuncks that are downloaded, careful when increasing to make sure stack is big enough
#define DOWNLOAD_BUFFER_SIZE 128
#define UPLOAD_BUFFER_SIZE 128

class TaskHTTPImpl {

    log4Esp::Logger logger = log4Esp::Logger("TaskHTTP");

public:

    void download(const char *, File *, DOWNLOAD_COMPLETED_CB completedDb = NULL, DOWNLOAD_PROGRESS_CB progressCb = NULL);
    void download(const char *, const char *, DOWNLOAD_COMPLETED_CB completedDb = NULL, DOWNLOAD_PROGRESS_CB progressCb = NULL);
    void __downloadInternal(const char *, File *);
    void __downloadInternal(void*);

    void upload(File *, const char *, UPLOAD_COMPLETED_CB completedDb = NULL, UPLOAD_PROGRESS_CB progressCb = NULL);
    void upload(const char *, const char *, UPLOAD_COMPLETED_CB completedDb = NULL, UPLOAD_PROGRESS_CB progressCb = NULL);
    void __uploadInternal(const char *, File *);
    void __uploadInternal(void*);
};

// extern TaskHTTPImpl TaskHTTP;

class DownloadTask
{

public:
    DownloadTask(TaskHTTPImpl* instance, const char* url, const char* filePath, DOWNLOAD_COMPLETED_CB completedCb, DOWNLOAD_PROGRESS_CB progressCb) {

        this->openedFile = SD.open(filePath, "w");

        this->instance = instance;
        this->file = &(this->openedFile);
        this->completedCb = completedCb;
        this->progressCb = progressCb;
        this->url = (char*)malloc(strlen(url) +1);
        strcpy(this->url, url);
    };

    DownloadTask(TaskHTTPImpl* instance, const char* url, File* file, DOWNLOAD_COMPLETED_CB completedCb, DOWNLOAD_PROGRESS_CB progressCb) {

        this->instance = instance;
        this->file = file;
        this->completedCb = completedCb;
        this->progressCb = progressCb;
        this->url = (char*)malloc(strlen(url) +1);
        strcpy(this->url, url);
    };

    ~DownloadTask() {
        free(this->url);
    };

    char* getUrl() { return this->url; }
    File* getFile() { return this->file; }
    TaskHTTPImpl* getInstance() { return this->instance; }

    void reportProgress(unsigned long int completed, unsigned long int total) {
        if (this->progressCb != NULL) this->progressCb(completed, total);
    };

    void reportCompleted(bool result) {
        if (this->openedFile) {
            this->openedFile.close();
        }

        if (this->completedCb != NULL) this->completedCb(result); };

private:
    File openedFile;
    File *file;
    char *url;
    TaskHTTPImpl *instance;
    DOWNLOAD_PROGRESS_CB progressCb = NULL;
    DOWNLOAD_COMPLETED_CB completedCb = NULL;
};

class UploadTask
{

public:
    UploadTask(TaskHTTPImpl* instance, const char* url, const char* filePath, UPLOAD_COMPLETED_CB completedCb, UPLOAD_PROGRESS_CB progressCb) {

        this->openedFile = SD.open(filePath, "r");

        this->instance = instance;
        this->file = &(this->openedFile);
        this->completedCb = completedCb;
        this->progressCb = progressCb;
        this->url = (char*)malloc(strlen(url) +1);
        strcpy(this->url, url);
    };

    UploadTask(TaskHTTPImpl* instance, const char* url, File* file, DOWNLOAD_COMPLETED_CB completedCb, DOWNLOAD_PROGRESS_CB progressCb) {

        this->instance = instance;
        this->file = file;
        this->completedCb = completedCb;
        this->progressCb = progressCb;
        this->url = (char*)malloc(strlen(url) +1);
        strcpy(this->url, url);
    };

    ~UploadTask() {
        free(this->url);
    };

    char* getUrl() { return this->url; }
    File* getFile() { return this->file; }
    TaskHTTPImpl* getInstance() { return this->instance; }

    void reportProgress(unsigned long int completed, unsigned long int total) {
        if (this->progressCb != NULL) this->progressCb(completed, total);
    };

    void reportCompleted(bool result) {
        if (this->openedFile) {
            this->openedFile.close();
        }

        if (this->completedCb != NULL) this->completedCb(result); };

private:
    TaskHTTPImpl *instance;

    File openedFile;
    File *file;
    char *url;

    UPLOAD_PROGRESS_CB progressCb = NULL;
    UPLOAD_COMPLETED_CB completedCb = NULL;
};


#endif // __AsyncHTTP_H__