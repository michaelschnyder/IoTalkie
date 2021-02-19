#include "Inbox.h"

typedef struct InboxItem {
    String messageId;
    long timestamp;
    String senderId;
    String senderName;
    long size;

    String localFilename;
};

bool Inbox::load() 
{
    sqlite3_initialize();

    logger.verbose(F("Attempting to load inbox from '%s'"), filename.c_str());
    
    int rc = sqlite3_open(filename.c_str(), &db);
    if (rc) {
        logger.error(F("Can't open database: %s %s"), sqlite3_extended_errcode(db), sqlite3_errmsg(db));
        return false;
    }

    sqlite3_close(db);

    sqlite3_shutdown();

    return true;
}
