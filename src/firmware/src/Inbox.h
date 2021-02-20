#ifndef __INBOX_H__
#define __INBOX_H__

#include <Log4Esp.h>
#include <SD.h>
#include <ArduinoJson.h>
#include <SQLiteDatabase.h>

class Inbox {
    log4Esp::Logger logger = log4Esp::Logger("Inbox");
    
    const char* filename = "/sd/inbox.db";
    SQLiteDatabase db;
public:
    Inbox() : db(filename) {};
    bool load();
    // bool handleUpdate(JsonObject&);
    // bool handleUpdates(JsonArray&);

    // int getNumberOfMessagesFrom(String userId);
};

#endif // __INBOX_H__