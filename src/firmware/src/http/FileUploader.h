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
    bool successful;
    File* currentFile;

public:


    void send(File *, const char *);
    void _sendInternal(File *, const char *);
    bool isCompleted();
    bool isSuccessful();
    
    long getBytesSent();
    long getBytesTotal();
};

#endif // __FILEUPLOADER_H__
