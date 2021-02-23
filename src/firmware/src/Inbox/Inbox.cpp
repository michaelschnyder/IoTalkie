#include "Inbox.h"

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
    SchemaMigrator schemaMigrator(&db);
    schemaMigrator.runIfMissing(new M_202102211710_Init());
    return true;
}

bool Inbox::handleNotification(JsonObject& notification) 
{
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
    
    SQLiteConnection conn(&db);
    bool exists = conn.queryInt("SELECT COUNT(*) FROM messages WHERE messageId = '%s'", messageId.c_str());

    if (exists) {
        logger.verbose("Message with id '%s' is already present in inbox. Skipping", messageId.c_str());
        return false;
    }

    char* insertSql = "INSERT INTO messages (messageId, timestamp, senderId, size, remoteUrl) VALUES('%s', %i, '%s', %i, '%s')";
    return conn.execute(insertSql, messageId.c_str(), timestamp, senderId, size, remoteUrl.c_str());
}

void Inbox::loop() 
{
    
}
