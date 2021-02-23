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

class Inbox {
    log4Esp::Logger logger = log4Esp::Logger("Inbox");
    
    const char* filename = "/sd/inbox.db";
    Contacts* contacts;
    SQLiteDatabase db;
    bool checkForPendingDownloads = true;
    bool hasNewMessage[3];

    MessageDownloadTask* getNextDownloadTask();
    bool download(MessageDownloadTask* t);
    bool setAvailable(MessageDownloadTask* t);
    ONNEWMESSAGE_CALLBACK_SIGNATURE onNewMessageCallback;
public:
    Inbox(Contacts* contacts) : db(filename) {
        this->contacts = contacts;
    }

    bool load();
    bool handleNotification(JsonObject&);
    // bool handleUpdates(JsonArray&);

    void loop();
    bool hasNewMessages(int slotId);
    const String getNextFor(const char* userId);

    void onNewMessage(ONNEWMESSAGE_CALLBACK_SIGNATURE callback);
    // int getNumberOfMessagesFrom(String userId);
};

#endif // __INBOX_H__