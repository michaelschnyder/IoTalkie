#include "Inbox.h"

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

    auto insertSql = "INSERT INTO messages (messageId, timestamp, senderId, size, remoteUrl) VALUES('%s', %i, '%s', %i, '%s')";
    if(conn.execute(insertSql, messageId.c_str(), timestamp, senderId.c_str(), size, remoteUrl.c_str())) {
        checkForPendingDownloads = true;
        return true;
    }
}

void Inbox::loop() 
{
    auto next = getNextDownloadTask();

    if (!next) {
        return;
    }

    if (!download(next)) {
        return;
    }

    setAvailable(next);
    free(next);
}

bool Inbox::hasNewMessages(int slotId) 
{
    return this->hasNewMessage[slotId];
}

void Inbox::onNewMessage(ONNEWMESSAGE_CALLBACK_SIGNATURE callback) 
{
    this->onNewMessageCallback = callback;
}

MessageDownloadTask* Inbox::getNextDownloadTask() 
{
    if (!checkForPendingDownloads) {
        return nullptr;
    }

    SQLiteConnection conn(&db);
    auto resultSet = conn.query("SELECT messageId, senderId, remoteUrl from messages WHERE localFile Is NULL ORDER BY timestamp ASC LIMIT 1");

    if (!resultSet->read()) {
        checkForPendingDownloads = false;
        return nullptr;
    }

    char* messageId = resultSet->getString(0);
    char* senderId = resultSet->getString(1);
    char* remoteUrl = resultSet->getString(2);

    logger.trace("Found pending message %s to be downloaded from %s", messageId, remoteUrl);

    char localFile[256];
    sprintf(localFile, "/from_%s-%s.mp3", senderId, messageId);

    return new MessageDownloadTask(messageId, senderId, remoteUrl, localFile);
}

bool Inbox::download(MessageDownloadTask* task) 
{
    HTTPClient http;
    http.begin(task->getDownloadUrl());

    int httpCode = http.GET();

    if (!(httpCode > 0)) {
        logger.error("failed to download file. Error: %s\n", http.errorToString(httpCode).c_str());
        
        // TODO record attempt and retry again
        checkForPendingDownloads = false;
        return false;
    }

    if(httpCode != HTTP_CODE_OK) {
        logger.error("failed to download file. Server responded with error: %s\n", http.errorToString(httpCode).c_str());

        // TODO record attempt and retry again
        checkForPendingDownloads = false;
        return false;
    }

    int totalSize = http.getSize();

    WiFiClient * stream = http.getStreamPtr();

    File f = SD.open(task->getStorageLocation(), FILE_WRITE);
    uint8_t buff[1024] = { 0 };

    int remaining = totalSize;
    while(http.connected() && (remaining > 0 || remaining == -1)) {
        // get available data size
        size_t size = stream->available();

        if(size) {

            int c = stream->readBytes(buff, ((size > sizeof(buff)) ? sizeof(buff) : size));
            f.write(buff, c);

            if(remaining > 0) {
                remaining -= c;
            }
        }

        if (remaining) {
            long bytesStored = f.position();
            int percent = (int)((bytesStored * 100.0f) / totalSize);
            logger.verbose("Download progress: %i/%i, %i%%", bytesStored, totalSize, percent);
        }
        else {
            logger.verbose("Download progress: %i");
        }

        delay(10);
    }
    
    http.end();
    f.close();
    return true;
}

bool Inbox::setAvailable(MessageDownloadTask* task) 
{
    SQLiteConnection conn(&db);
    if (conn.execute("UPDATE messages SET localFile = '%s' WHERE messageId = '%s'", task->getStorageLocation(), task->getMessageId())) {
        checkForPendingDownloads = false;
        
        Serial.print("SenderId: ");
        Serial.print(task->getSenderId());
        Serial.println();

        // Find slot
        Contact* c = contacts->findByUserId(task->getSenderId());
        if (c) {
            
            logger.trace("New message is available from sender '%s'. (UserId: %i, Slot: %i)", c->name, c->userId, c->slot);
            this->hasNewMessage[c->slot] = true;

            if (onNewMessageCallback != NULL) {
                onNewMessageCallback(c);
            }
        }
    }
    else {
        // TODO Handle this
    }
}
