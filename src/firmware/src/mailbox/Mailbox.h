#ifndef __INBOX_H__
#define __INBOX_H__

#include <Log4Esp.h>
#include <SD.h>
#include <ArduinoJson.h>
#include <SQLite.h>
#include <SchemaMigrator.h>
#include <SchemaMigration.h>

#include "MessageRecord.h"
#include "Contacts.h"
#include "http/TaskHTTP.h"

#include "migrations/AllMigrations.h"

#define ONNEWMESSAGE_CALLBACK_SIGNATURE std::function<void(Contact*)> 

#define SQLITE_FILENAME "/sd/mailbox.db"

#define QUERY_COUNT_PENDING_DOWNLOADS "SELECT COUNT(*) from inbox WHERE localFile Is NULL"

#define QUERY_COUNT_UNPLAYED_MESSAGE_FOR_USERID "SELECT COUNT(*) from inbox WHERE senderId = '%s' AND playCount = 0 AND localFile is NOT NULL AND isFaulty != 1"
#define QUERY_OLDEST_UNPLAYED_MESSAGE_FOR_USERID "SELECT messageId, localFile from inbox WHERE senderId = '%s' AND playCount = 0 AND localFile is NOT NULL AND isFaulty != 1 ORDER BY timestamp ASC LIMIT 1"
#define QUERY_MOST_RECENT_PLAYED_MESSAGE_FOR_USERID "SELECT messageId, localFile from inbox WHERE senderId = '%s' AND localFile is NOT NULL AND isFaulty != 1 ORDER BY timestamp DESC LIMIT 1"

#define QUERY_INSERT_NEW_INCOMING_MESSAGE "INSERT INTO inbox (messageId, timestamp, senderId, size, remoteUrl) VALUES('%s', %i, '%s', %i, '%s')"
#define QUERY_COUNT_MESSAGES_BY_MESSAGEID "SELECT COUNT(*) FROM inbox WHERE messageId = '%s'"
#define QUERY_INCREASE_PLAYCOUNT_BY_MESSAGEID "UPDATE inbox SET playcount = playcount + 1 WHERE messageId = '%s'"
#define QUERY_NEXT_PENDING_MESSAGE_FOR_DOWNLOAD "SELECT messageId, senderId, remoteUrl from inbox WHERE localFile Is NULL ORDER BY timestamp ASC LIMIT 1"
#define QUERY_UPDATE_LOCALFILE_BY_MESSAGEID "UPDATE inbox SET localFile = '%s' WHERE messageId = '%s'"
#define QUERY_SET_IGNORED_BY_MESSAGEID "UPDATE inbox SET isFaulty = 1 WHERE messageId = '%s'"

class Mailbox {
    log4Esp::Logger logger = log4Esp::Logger("Mailbox");
    
    Contacts* contacts;
    SQLiteDatabase db;
    TaskHTTPImpl taskHttp;

    ONNEWMESSAGE_CALLBACK_SIGNATURE onNewMessageCallback;
    
    bool pendingDownloadsAvailable = true;
    bool hasNewMessage[3];

    MessageRecord* getNextDownloadTask();
    bool setAvailable(MessageRecord* t);
    void findUnplayedMessagesForEachSlot();

public:
    Mailbox(Contacts* contacts) : db(SQLITE_FILENAME) {
        this->contacts = contacts;
    }

    bool load();
    bool handleNotification(JsonObject&);
    
    bool hasPendingDownloads(bool);
    void downloadSingleMessage();
    
    bool hasNewMessages(int slotId);
    MessageRecord* getAudioMessageFor(const char* userId);
    
    void setPlayed(MessageRecord* message);
    void setIgnored(MessageRecord* message);

    void onNewMessage(ONNEWMESSAGE_CALLBACK_SIGNATURE callback);
};

#endif // __INBOX_H__