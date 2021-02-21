#ifndef __INBOX_H__
#define __INBOX_H__

#include <Log4Esp.h>
#include <SD.h>
#include <ArduinoJson.h>
#include <SQLiteDatabase.h>
#include <SchemaMigrator.h>
#include <SchemaMigration.h>

#include "migrations/M_202102211710_Init.h"

class Inbox {
    log4Esp::Logger logger = log4Esp::Logger("Inbox");
    
    const char* filename = "/sd/inbox.db";
    
    SQLiteDatabase db;
    SchemaMigrator schemaMigrator;

public:
    Inbox() : db(filename), schemaMigrator(&db) { };
    bool load();
    // bool handleUpdate(JsonObject&);
    // bool handleUpdates(JsonArray&);

    // int getNumberOfMessagesFrom(String userId);
};

#endif // __INBOX_H__