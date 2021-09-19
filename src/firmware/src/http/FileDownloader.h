#ifndef __FILEDOWNLOADER_H__
#define __FILEDOWNLOADER_H__

#include <Log4Esp.h>

#include <WiFiClient.h>
#include <HTTPClient.h>
#include <FS.h>

#define DOWNLOAD_PROGRESS_CB std::function<void(unsigned long int, unsigned long int)> 
#define DOWNLOAD_COMPLETED_CB std::function<void()> 
#define DOWNLOAD_FAILED_CB std::function<void()>

// Increase this value if you get exceptions similar to "Debug exception reason: Stack canary watchpoint triggered (_FileDownloaderBackgroundTask)"
// HTTPS connections seemd to need around 8kb
#define TASK_STACK_DEPT 8 * 1024

// Size of chuncks that are downloaded, careful when increasing to make sure stack is big enough
#define DOWNLOAD_BUFFER_SIZE 128

class FileDownloader {

    log4Esp::Logger logger = log4Esp::Logger("FileDownloader");

    WiFiClient client;
    bool completed;
    bool successful;
    File* currentFile;

public:
    void download(const char *, File *, DOWNLOAD_PROGRESS_CB, DOWNLOAD_COMPLETED_CB, DOWNLOAD_FAILED_CB);
//    void __downloadInternal(const char *, File *, DOWNLOAD_PROGRESS_CB, DOWNLOAD_COMPLETED_CB, DOWNLOAD_FAILED_CB);
    void __downloadInternal(const char *, File *);
    void __downloadInternal(void*);

};

class DownloadTask
{

public:
    DownloadTask(FileDownloader* instance, const char* url, File* file, DOWNLOAD_PROGRESS_CB progressCb, DOWNLOAD_COMPLETED_CB completedCb, DOWNLOAD_FAILED_CB failedCb) {

        this->downloader = instance;
        this->file = file;
        this->progressCb = progressCb;
        this->completedCb = completedCb;
        this->failedCb = failedCb;
        this->url = (char*)malloc(strlen(url) +1);
        strcpy(this->url, url);
    };

    ~DownloadTask() {
        free(this->url);
    };

    char* getUrl() { return this->url; }
    File* getFile() { return this->file; }
    FileDownloader* getDownloader() { return this->downloader; }
    
    void reportProgress(unsigned long int completed, unsigned long int total) {
        if (this->progressCb != NULL) this->progressCb(completed, total);
    };

    void reportCompleted() {
        if (this->completedCb != NULL) this->completedCb();
    };

    void reportFailed() {
        if (this->failedCb != NULL) this->failedCb();
    };

private:
    File *file;
    char *url;
    FileDownloader *downloader;
    DOWNLOAD_PROGRESS_CB progressCb = NULL;
    DOWNLOAD_COMPLETED_CB completedCb = NULL;
    DOWNLOAD_FAILED_CB failedCb = NULL;
};

#endif // __FILEDOWNLOADER_H__