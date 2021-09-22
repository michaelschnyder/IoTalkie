#include "Mailbox.h"

bool Mailbox::load() 
{
    SchemaMigrator schemaMigrator(&db);
    schemaMigrator.runIfMissing(new M_202109211300_Init());
    
    findUnplayedMessagesForEachSlot();
    
    SD.mkdir(OUTBOX_FOLDER);
    SD.mkdir(INBOX_FOLDER);

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

bool Mailbox::enqueueMessage(const char* sourceFile, const char* recipientId) {
    
    logger.trace(F("Enqueueing message '%s' to '%s'"), sourceFile, config->getPostMessageUrl().c_str());

    bool recordInserted = false;
    char outboxFilename[256];

    { // Scoped
        SQLiteConnection conn(&db);

        uint8_t uuid_array[16];
        ESPRandom::uuid4(uuid_array);

        auto messageId = ESPRandom::uuidToString(uuid_array).c_str();
        auto timestamp = timeService->getTimestamp();

        sprintf(outboxFilename, OUTBOX_FOLDER "/" MESSAGE_FILENAME_TEMPLATE, messageId);

        if(conn.execute(QUERY_INSERT_NEW_OUTGOING_MESSAGE, messageId, timestamp, recipientId, outboxFilename)) {
            pendingUploadsAvailable = true;
            recordInserted = true;
        }
    } // End Scope

    if (recordInserted) {
        // Move file to outbox
        SD.rename(sourceFile, outboxFilename);
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

bool Mailbox::hasPendingUploads(bool forceCheck) {
    
    if (forceCheck) {
        SQLiteConnection conn(&db);
        pendingUploadsAvailable = conn.queryInt(QUERY_COUNT_PENDING_UPLOADS) > 0;
    }

    return pendingUploadsAvailable;
}

void Mailbox::downloadSingleMessage() 
{
    SQLiteConnection conn(&db);
    auto next = getNextDownloadTask();

    if (!next) {
        return;
    }

    increaseDownloadAttempt(next->getMessageId());

    conn.~SQLiteConnection();

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

bool Mailbox::sendSingleMessage() {

    auto message = getNextUpload();

    if (message == NULL) {
        return false;
    }

    auto sourceFile = message->getStorageLocation();
    auto recipientId = message->getRecipient();

    logger.trace(F("Sending message '%s' to recipient '%s' using url '%s'"), sourceFile, recipientId, config->getPostMessageUrl().c_str());

    if (!SD.exists(sourceFile)) {
        logger.error("File to be sent does not exist. Missing '%s'. Cancelling message.", sourceFile);
        setFaulty(message->getMessageId());
        return false;
    }

    String url = config->getPostMessageUrl();
    url.replace("{messageId}", message->getMessageId());
    url.replace("{recipientId}", recipientId);

    increaseSentAttempt(message->getMessageId());

    bool isCompleted = false, isSuccessful = false;
    taskHttp.upload(sourceFile, url.c_str(), 
        [this, &isCompleted, &isSuccessful](bool result) { 
            isCompleted = true; isSuccessful = result; 
        });

    while (!isCompleted) {
        yield();
    }

    if (isSuccessful) {
        setSent(message);
    }
    else {
        hasPendingUploads(true);
    }

    return isSuccessful;
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
    setFaulty(message->getMessageId());
    findUnplayedMessagesForEachSlot();
}

void Mailbox::setFaulty(char* messageId) 
{
    SQLiteConnection conn(&db);
    conn.execute(QUERY_SET_IGNORED_BY_MESSAGEID, messageId);
    conn.execute(QUERY_SET_OUTBOX_IGNORED_BY_MESSAGEID, messageId);
}

void Mailbox::increaseDownloadAttempt(char* messageId) {
    SQLiteConnection conn(&db);
    conn.execute(QUERY_INCREASE_DOWNLOADCOUNT_BY_MESSAGEID, messageId);
}

void Mailbox::increaseSentAttempt(char* messageId) {
    SQLiteConnection conn(&db);
    conn.execute(QUERY_INCREASE_SENTCOUNT_BY_MESSAGEID, messageId);    
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
    sprintf(localFile, (INBOX_FOLDER "/" MESSAGE_FILENAME_TEMPLATE), messageId);

    return new MessageRecord(messageId, senderId, remoteUrl, localFile);
}

MessageRecord* Mailbox::getNextUpload() {

    SQLiteConnection conn(&db);
    auto resultSet = conn.query(QUERY_NEXT_PENDING_MESSAGE_FOR_UPLOAD);

    if (!resultSet->read()) {
        pendingUploadsAvailable = false;
        return nullptr;
    }

    char* messageId = resultSet->getString(0);
    char* recipientId = resultSet->getString(1);
    char* localFile = resultSet->getString(2);

    logger.trace("Found pending message %s to be uploaded for %s", messageId, recipientId);

    return new MessageRecord(messageId, recipientId, localFile);
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

bool Mailbox::setSent(MessageRecord* task) 
{
    SQLiteConnection conn(&db);
    if (conn.execute(QUERY_UPDATE_OUTBOX_ISSENT_BY_MESSAGEID, task->getMessageId())) {
        hasPendingDownloads(true);
    }
}
