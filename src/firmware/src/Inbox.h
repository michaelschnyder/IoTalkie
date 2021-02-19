#ifndef __INBOX_H__
#define __INBOX_H__

#include <Log4Esp.h>
#include <SD.h>
#include <ArduinoJson.h>
#include <sqlite3.h>

class Inbox {
    log4Esp::Logger logger = log4Esp::Logger("Inbox");
    
    sqlite3 *db = NULL;
    
    const String filename = "/sd/inbox.db";
    
public:
    bool load();
    // bool handleUpdate(JsonObject&);
    // bool handleUpdates(JsonArray&);

    // int getNumberOfMessagesFrom(String userId);
};

#endif // __INBOX_H__