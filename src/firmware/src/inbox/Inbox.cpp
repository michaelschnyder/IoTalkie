#include "Inbox.h"

bool Inbox::load() 
{
    SchemaMigrator schemaMigrator(&db);
    schemaMigrator.runIfMissing(new M_202102211710_Init());
    
    findUnplayedMessagesForEachSlot();
    
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
    bool exists = conn.queryInt(QUERY_COUNT_MESSAGES_BY_MESSAGEID, messageId.c_str());

    if (exists) {
        logger.verbose("Message with id '%s' is already present in inbox. Skipping", messageId.c_str());
        return false;
    }

    if(conn.execute(QUERY_INSERT_NEW_MESSAGE, messageId.c_str(), timestamp, senderId.c_str(), size, remoteUrl.c_str())) {
        pendingDownloadsAvailable = true;
        return true;
    }

    return false;
}

bool Inbox::hasPendingDownloads(bool forceCheck) 
{
    if (forceCheck) {
        SQLiteConnection conn(&db);
        pendingDownloadsAvailable = conn.queryInt(QUERY_COUNT_PENDING_DOWNLOADS) > 0;
    }

    return pendingDownloadsAvailable;
}

void Inbox::downloadSingleMessage() 
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

void Inbox::findUnplayedMessagesForEachSlot() {
    
    SQLiteConnection conn(&db);
    for (size_t slot = 0; slot < contacts->size(); slot++)
    {
        Contact* c = contacts->get(slot);
        int numberOfUnplayed = conn.queryInt(QUERY_COUNT_UNPLAYED_MESSAGE_FOR_USERID, c->userId);
        logger.trace("Found %i new messages from user '%s' (UserId: '%s') on slot %i", numberOfUnplayed, c->name, c->userId, slot);
        
        this->hasNewMessage[slot] = numberOfUnplayed > 0;
    }
}

bool Inbox::hasNewMessages(int slotId) 
{
    return this->hasNewMessage[slotId];
}

const String Inbox::getAudioMessageFor(const char* userId) 
{
    SQLiteConnection conn(&db);
    String nextUnplayed = conn.queryString(QUERY_OLDEST_UNPLAYED_MESSAGE_FOR_USERID, userId);

    if (!nextUnplayed.isEmpty()) {
        logger.verbose("Found next unplayed message '%s' from senderId '%s'", nextUnplayed.c_str(), userId);
        return nextUnplayed;
    }

    logger.verbose("No unplayed message available, finding the newest one");
    String newestMessage = conn.queryString(QUERY_MOST_RECENT_PLAYED_MESSAGE_FOR_USERID, userId);
    return newestMessage;
}

void Inbox::setPlayed(const char * filename) 
{
    SQLiteConnection conn(&db);
    conn.execute(QUERY_INCREASE_PLAYCOUNT_BY_LOCALFILE, filename);

    findUnplayedMessagesForEachSlot();
}

void Inbox::onNewMessage(ONNEWMESSAGE_CALLBACK_SIGNATURE callback) 
{
    this->onNewMessageCallback = callback;
}

MessageDownloadTask* Inbox::getNextDownloadTask() 
{
    SQLiteConnection conn(&db);
    auto resultSet = conn.query(QUERY_NEXT_PENDING_MESSAGE_FOR_DOWNLOAD);

    if (!resultSet->read()) {
        pendingDownloadsAvailable = false;
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
        return false;
    }

    if(httpCode != HTTP_CODE_OK) {
        logger.error("failed to download file. Server responded with error: %s\n", http.errorToString(httpCode).c_str());
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
    if (conn.execute(QUERY_UPDATE_LOCALFILE_BY_MESSAGEID, task->getStorageLocation(), task->getMessageId())) {
        
        hasPendingDownloads(true);

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
        return false;
    }
}
