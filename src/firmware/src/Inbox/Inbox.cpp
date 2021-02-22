#include "Inbox.h"
#define NAMEOF(name) #name

typedef struct InboxItem {
    String messageId;
    long timestamp;
    String senderId;
    // String senderName;
    long size;
    String remoteUrl;
    String localFile;
    long playCount;
};

bool Inbox::load() 
{
    db.open();

    schemaMigrator.runIfMissing(new M_202102211710_Init());

    db.close();

    return true;
}

bool Inbox::handleNotification(JsonObject& notification) 
{
    char buff[500];
    auto mustHave = {"messageId", "timestamp", "senderId", "size", "remoteUrl"};
    std::initializer_list<const char*>::iterator key;
    
    for (key=mustHave.begin(); key!=mustHave.end(); ++key) {
        if (!notification.containsKey(*key)) {
            logger.error("invalid notification receieved. Missing key %s", *key);
            return false;
        }
    }

    String messageId = notification.get<String>("messageId");
    long timestamp = notification.get<long>("timestamp");
    String senderId = notification.get<String>("senderId");
    int size = notification.get<int>("size");
    String remoteUrl = notification.get<String>("remoteUrl");

    db.open();

    sprintf(buff, "SELECT COUNT(*) FROM messages WHERE messageId = '%s'", messageId.c_str());

    bool exists = db.queryInt(buff);

    if (exists) {
        logger.verbose("Message with id '%s' is already present in inbox. Skipping", messageId);
        db.close();
        return false;
    }
    
    sprintf(buff, "INSERT INTO messages (messageId, timestamp, senderId, size, remoteUrl) VALUES('%s', %il, '%s', %i, '%s')", 
                                         messageId.c_str(), timestamp, senderId, size, remoteUrl.c_str());

    if (db.execute(buff)) {
        db.close();
        return true;
    };

    db.close();
}
