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
    db.open();

    int tables = db.queryInt("SELECT count(*) FROM sqlite_master WHERE type ='table' AND name NOT LIKE 'sqlite_%';");
    if (tables == 0){
        logger.trace("Database is empty, will create initial database structure", tables);
    }

    db.close();

    return true;
}
