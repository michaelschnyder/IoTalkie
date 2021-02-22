#ifndef __INBOX_H__
#define __INBOX_H__

#include <Log4Esp.h>
#include <SD.h>
#include <ArduinoJson.h>
#include <SQLite.h>
#include <SchemaMigrator.h>
#include <SchemaMigration.h>

#include "migrations/M_202102211710_Init.h"

class Inbox {
    log4Esp::Logger logger = log4Esp::Logger("Inbox");
    
    const char* filename = "/sd/inbox.db";
    
    SQLiteDatabase db;

public:
    Inbox() : db(filename) { };
    bool load();
    bool handleNotification(JsonObject&);
    // bool handleUpdates(JsonArray&);

    // int getNumberOfMessagesFrom(String userId);
};

#endif // __INBOX_H__