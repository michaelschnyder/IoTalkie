#include "Mailbox.h"

bool Mailbox::load() 
{
    SchemaMigrator schemaMigrator(&db);
    schemaMigrator.runIfMissing(new M_202109211300_Init());
    
    findUnplayedMessagesForEachSlot();
    
    return true;
}

bool Mailbox::handleNotification(JsonObject& notification) 
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

    if(conn.execute(QUERY_INSERT_NEW_INCOMING_MESSAGE, messageId.c_str(), timestamp, senderId.c_str(), size, remoteUrl.c_str())) {
        pendingDownloadsAvailable = true;
        return true;
    }

    return false;
}

bool Mailbox::hasPendingDownloads(bool forceCheck) 
{
    if (forceCheck) {
        SQLiteConnection conn(&db);
        pendingDownloadsAvailable = conn.queryInt(QUERY_COUNT_PENDING_DOWNLOADS) > 0;
    }

    return pendingDownloadsAvailable;
}

void Mailbox::downloadSingleMessage() 
{
    auto next = getNextDownloadTask();

    if (!next) {
        return;
    }

    bool isCompleted = false, isSuccessful = false;

    taskHttp.download(next->getDownloadUrl(), next->getStorageLocation(),
        [&isCompleted, &isSuccessful](bool result) { 
            isCompleted = true; isSuccessful = result; 
            });

    while (!isCompleted) {
        yield();
    }

    if (!isSuccessful) {
        free(next);
        return;
    }

    setAvailable(next);
    free(next);
}

void Mailbox::findUnplayedMessagesForEachSlot() {
    
    SQLiteConnection conn(&db);
    for (size_t slot = 0; slot < contacts->size(); slot++)
    {
        Contact* c = contacts->get(slot);
        int numberOfUnplayed = conn.queryInt(QUERY_COUNT_UNPLAYED_MESSAGE_FOR_USERID, c->userId);
        logger.trace("Found %i new messages from user '%s' (UserId: '%s') on slot %i", numberOfUnplayed, c->name, c->userId, slot);
        
        this->hasNewMessage[slot] = numberOfUnplayed > 0;
    }
}

bool Mailbox::hasNewMessages(int slotId) 
{
    return this->hasNewMessage[slotId];
}

MessageRecord* Mailbox::getAudioMessageFor(const char* userId) 
{
    SQLiteConnection conn(&db);
    auto nextUnplayed = conn.query(QUERY_OLDEST_UNPLAYED_MESSAGE_FOR_USERID, userId);

    if (nextUnplayed->read()) {
        logger.verbose("Found new unplayed message '%s' from senderId '%s'", nextUnplayed->getString(1), userId);
        return new MessageRecord(nextUnplayed->getString(0), "", "", nextUnplayed->getString(1));
    }

    logger.verbose("Only already played available, finding the most recent one");
    auto newestMessage = conn.query(QUERY_MOST_RECENT_PLAYED_MESSAGE_FOR_USERID, userId);

    if (newestMessage->read()) {
        return new MessageRecord(newestMessage->getString(0), "", "", newestMessage->getString(1));
    }
    
    return NULL;
}

void Mailbox::setPlayed(MessageRecord* message) 
{
    SQLiteConnection conn(&db);
    conn.execute(QUERY_INCREASE_PLAYCOUNT_BY_MESSAGEID, message->getMessageId());

    findUnplayedMessagesForEachSlot();
}

void Mailbox::setIgnored(MessageRecord* message) 
{
    SQLiteConnection conn(&db);
    conn.execute(QUERY_SET_IGNORED_BY_MESSAGEID, message->getMessageId());

    findUnplayedMessagesForEachSlot();
}

void Mailbox::onNewMessage(ONNEWMESSAGE_CALLBACK_SIGNATURE callback) 
{
    this->onNewMessageCallback = callback;
}

MessageRecord* Mailbox::getNextDownloadTask() 
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

    return new MessageRecord(messageId, senderId, remoteUrl, localFile);
}

bool Mailbox::setAvailable(MessageRecord* task) 
{
    logger.trace("Mark message '%s' as available for consumption", task->getMessageId());

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