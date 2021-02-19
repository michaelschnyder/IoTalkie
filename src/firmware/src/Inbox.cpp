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
    logger.verbose(F("Attempting to load inbox from '%s'"), filename.c_str());

    if (!SPIFFS.exists(filename)) {
        
        logger.warning(F("Inbox store '%s' does not exist."), filename.c_str());
    }
    
    int rc = sqlite3_open(filename.c_str(), &db);
    if (rc) {
        logger.error(F("Can't open database: %s"), sqlite3_errmsg(db));
        return false;
    }

    return true;
}
