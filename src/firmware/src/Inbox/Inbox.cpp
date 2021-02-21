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

    schemaMigrator.runIfMissing(new M_202102211710_Init());

    db.close();

    return true;
}
