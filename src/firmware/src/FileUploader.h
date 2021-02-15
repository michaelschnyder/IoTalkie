#ifndef __FILEUPLOADER_H__
#define __FILEUPLOADER_H__

#include <Log4Esp.h>

#include "WiFiClient.h"
#include "FS.h"
#include "HTTPClient.h"

class FileUploader
{
    log4Esp::Logger logger = log4Esp::Logger("FileUploader");

    WiFiClient client;
    bool completed;
    File* currentFile;

public:


    void send(File *, const char *);
    void sendInternal(File *, const char *);
    bool isCompleted();
    long getBytesSent();
    long getBytesTotal();
};

#endif // __FILEUPLOADER_H__
