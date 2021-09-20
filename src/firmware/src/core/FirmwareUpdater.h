#ifndef __FIRMWAREUPDATER_H__
#define __FIRMWAREUPDATER_H__

#include <functional>
#include <SD.h>
#include <HTTPClient.h>
#include <Log4Esp.h>
#include <Update.h>

#include "http/TaskHTTP.h"

#define FW_DOWNLOAD_FILENAME "/update.tmp"
#define FW_READY_FILENAME "/update.bin"

#define UPDATEPROGRESS_CALLBACK_SIGNATURE std::function<void(int)> 
class FirmwareUpdater {
private:
    log4Esp::Logger logger = log4Esp::Logger("FirmwareUpdater");
    
    TaskHTTPImpl taskHttp;
    bool downloadCompleted;
    
public:
    void download(const char* firmwareUrl, UPDATEPROGRESS_CALLBACK_SIGNATURE);
    bool isUpdateDownloaded() { return downloadCompleted; }
    bool hasPendingUpdate() { return SD.exists(FW_READY_FILENAME); }
    void flashPendingUpdate(UPDATEPROGRESS_CALLBACK_SIGNATURE);
};

#endif // __FIRMWAREUPDATER_H__