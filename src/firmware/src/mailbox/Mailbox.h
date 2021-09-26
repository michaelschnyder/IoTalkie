#ifndef __INBOX_H__
#define __INBOX_H__

#include <Log4Esp.h>
#include <SD.h>
#include <ArduinoJson.h>
#include <ESPRandom.h>

#include <SQLite.h>
#include <SchemaMigrator.h>
#include <SchemaMigration.h>

#include "core/DeviceConfig.h"
#include "core/TimeService.h"
#include "file/FileInfo.h"

#include "MessageRecord.h"
#include "Contacts.h"
#include "http/TaskHTTP.h"

#include "migrations/AllMigrations.h"

#define ONNEWMESSAGE_CALLBACK_SIGNATURE std::function<void(Contact*)> 

#define SQLITE_FILENAME "/sd/mailbox.db"
#define INBOX_FOLDER "/inbox"
#define OUTBOX_FOLDER "/outbox"

#define QUERY_COUNT_PENDING_DOWNLOADS "SELECT COUNT(*) from inbox WHERE localFile IS NULL AND downloadCount < 3"

#define QUERY_COUNT_UNPLAYED_MESSAGE_FOR_USERID "SELECT COUNT(*) from inbox WHERE senderId = '%s' AND playCount = 0 AND localFile is NOT NULL AND isFaulty != 1"
#define QUERY_OLDEST_UNPLAYED_MESSAGE_FOR_USERID "SELECT messageId, localFile from inbox WHERE senderId = '%s' AND playCount = 0 AND localFile is NOT NULL AND isFaulty != 1 ORDER BY timestamp ASC LIMIT 1"
#define QUERY_MOST_RECENT_PLAYED_MESSAGE_FOR_USERID "SELECT messageId, localFile from inbox WHERE senderId = '%s' AND localFile is NOT NULL AND isFaulty != 1 ORDER BY timestamp DESC LIMIT 1"

#define QUERY_INSERT_NEW_INCOMING_MESSAGE "INSERT INTO inbox (messageId, timestamp, senderId, size, remoteUrl) VALUES('%s', %i, '%s', %i, '%s')"
#define QUERY_COUNT_MESSAGES_BY_MESSAGEID "SELECT COUNT(*) FROM inbox WHERE messageId = '%s'"
#define QUERY_INCREASE_PLAYCOUNT_BY_MESSAGEID "UPDATE inbox SET playcount = playcount + 1 WHERE messageId = '%s'"
#define QUERY_NEXT_PENDING_MESSAGE_FOR_DOWNLOAD "SELECT messageId, senderId, remoteUrl from inbox WHERE localFile Is NULL AND downloadCount < 3 ORDER BY timestamp ASC LIMIT 1"
#define QUERY_UPDATE_LOCALFILE_BY_MESSAGEID "UPDATE inbox SET localFile = '%s' WHERE messageId = '%s'"
#define QUERY_SET_IGNORED_BY_MESSAGEID "UPDATE inbox SET isFaulty = 1 WHERE messageId = '%s'"
#define QUERY_INCREASE_DOWNLOADCOUNT_BY_MESSAGEID "UPDATE inbox SET downloadCount = downloadCount + 1 WHERE messageId = '%s'"

#define QUERY_INSERT_NEW_OUTGOING_MESSAGE "INSERT INTO outbox (messageId, timestamp, recipientId, localFile) VALUES('%s', %i, %s, '%s')"
#define QUERY_COUNT_PENDING_UPLOADS "SELECT COUNT(*) from outbox WHERE isSent = 0 AND sentCount < 3"
#define QUERY_NEXT_PENDING_MESSAGE_FOR_UPLOAD "SELECT messageId, recipientId, localFile from outbox WHERE isSent = 0 AND isFaulty = 0 AND sentCount < 3 ORDER BY timestamp ASC LIMIT 1"
#define QUERY_SET_OUTBOX_IGNORED_BY_MESSAGEID "UPDATE outbox SET isFaulty = 1 WHERE messageId = '%s'"
#define QUERY_UPDATE_OUTBOX_ISSENT_BY_MESSAGEID "UPDATE outbox SET isSent = 1 WHERE messageId = '%s'"
#define QUERY_INCREASE_SENTCOUNT_BY_MESSAGEID "UPDATE outbox SET sentCount = sentCount + 1 WHERE messageId = '%s'"
        
class Mailbox {
    log4Esp::Logger logger = log4Esp::Logger("Mailbox");
    
    DeviceConfig* config;
    Contacts* contacts;
    TimeService* timeService;

    SQLiteDatabase db;
    TaskHTTPImpl taskHttp;

    ONNEWMESSAGE_CALLBACK_SIGNATURE onNewMessageCallback;
    
    bool pendingDownloadsAvailable = true;
    bool pendingUploadsAvailable = true;
    bool hasNewMessage[3];

    MessageRecord* getNextDownloadTask();
    MessageRecord* getNextUpload();

    bool setAvailable(MessageRecord* t);
    bool setSent(MessageRecord* t);
    void setFaulty(char* messageId);

    void increaseSentAttempt(char* messageId);
    void increaseDownloadAttempt(char* messageId);

    void findUnplayedMessagesForEachSlot();

public:
    Mailbox(Contacts* contacts, DeviceConfig* config, TimeService* timeService) : db(SQLITE_FILENAME) {
        this->contacts = contacts;
        this->config = config;
        this->timeService = timeService;
    }

    bool load();
    bool handleNotification(JsonObject&);
    bool enqueueMessage(const char* recording, const char* recipientId);
    
    bool hasPendingDownloads(bool);
    bool hasPendingUploads(bool);

    void downloadSingleMessage();
    bool sendSingleMessage();

    bool hasNewMessages(int slotId);
    MessageRecord* getAudioMessageFor(const char* userId);
    
    void setPlayed(MessageRecord* message);
    void setIgnored(MessageRecord* message);

    void onNewMessage(ONNEWMESSAGE_CALLBACK_SIGNATURE callback);
};

#endif // __INBOX_H__