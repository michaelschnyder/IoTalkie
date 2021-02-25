#ifndef __INBOX_H__
#define __INBOX_H__

#include <Log4Esp.h>
#include <SD.h>
#include <ArduinoJson.h>
#include <SQLite.h>
#include <SchemaMigrator.h>
#include <SchemaMigration.h>
#include <HTTPClient.h>
#include "Contacts.h"

#include "migrations/M_202102211710_Init.h"

class MessageDownloadTask {

    char* messageId;
    char* senderId;
    char* downloadUrl;
    char* localFile;

    public:
    MessageDownloadTask(char* messageId, char* senderId, char* downloadUrl, char* localFile) {

        this->messageId = (char*)malloc(strlen(messageId) + 1);
        this->senderId = (char*)malloc(strlen(senderId) + 1);
        this->downloadUrl = (char*)malloc(strlen(downloadUrl) + 1);
        this->localFile = (char*)malloc(strlen(localFile) + 1);

        strcpy(this->messageId, messageId);
        strcpy(this->senderId, senderId);
        strcpy(this->downloadUrl, downloadUrl);
        strcpy(this->localFile, localFile);
    }

    ~MessageDownloadTask() {
        free(this->messageId);
        free(this->senderId);
        free(this->downloadUrl);
        free(this->localFile);
    }

    char* getMessageId() { return this->messageId; }
    char* getSenderId() { return this->senderId; }
    char* getDownloadUrl() { return this->downloadUrl; }
    char* getStorageLocation() { return this->localFile; }
};

#define ONNEWMESSAGE_CALLBACK_SIGNATURE std::function<void(Contact*)> 

#define QUERY_COUNT_PENDING_DOWNLOADS "SELECT COUNT(*) from messages WHERE localFile Is NULL"
#define QUERY_COUNT_UNPLAYED_MESSAGE_FOR_USERID "SELECT COUNT(*) from messages WHERE senderId = '%s' AND playCount = 0 AND localFile is NOT NULL"
#define QUERY_OLDEST_UNPLAYED_MESSAGE_FOR_USERID "SELECT localFile from messages WHERE senderId = '%s' AND playCount = 0 AND localFile is NOT NULL ORDER BY timestamp ASC LIMIT 1"
#define QUERY_MOST_RECENT_PLAYED_MESSAGE_FOR_USERID "SELECT localFile from messages WHERE senderId = '%s' AND playCount > 0 ORDER BY timestamp DESC LIMIT 1"

class Inbox {
    log4Esp::Logger logger = log4Esp::Logger("Inbox");
    
    const char* filename = "/sd/inbox.db";
    Contacts* contacts;
    SQLiteDatabase db;
    bool pendingDownloadsAvailable = true;
    bool hasNewMessage[3];

    MessageDownloadTask* getNextDownloadTask();
    bool download(MessageDownloadTask* t);
    bool setAvailable(MessageDownloadTask* t);
    void findUnplayedMessagesForEachSlot();
    ONNEWMESSAGE_CALLBACK_SIGNATURE onNewMessageCallback;
public:
    Inbox(Contacts* contacts) : db(filename) {
        this->contacts = contacts;
    }

    bool load();
    bool handleNotification(JsonObject&);
    // bool handleUpdates(JsonArray&);
    
    bool hasPendingDownloads(bool);
    void downloadSingleMessage();
    bool hasNewMessages(int slotId);
    const String getAudioMessageFor(const char* userId);
    void setPlayed(const char * filename);
    void onNewMessage(ONNEWMESSAGE_CALLBACK_SIGNATURE callback);
    // int getNumberOfMessagesFrom(String userId);
};

#endif // __INBOX_H__