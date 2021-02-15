#ifndef __FILEUPLOADER_H__
#define __FILEUPLOADER_H__

#include <Log4Esp.h>

#include "WiFiClient.h"
#include "FS.h"
#include "HTTPClient.h"

class FileUploader {

    log4Esp::Logger logger = log4Esp::Logger("FileUploader");

    WiFiClient client;

    public:
    
    void send(File*, const char*);

};

#endif // __FILEUPLOADER_H__
