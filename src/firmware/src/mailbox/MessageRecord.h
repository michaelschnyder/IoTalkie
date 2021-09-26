#ifndef __MESSAGERECORD_H__
#define __MESSAGERECORD_H__

#include <stdlib.h>
#include <string.h>

class MessageRecord {

    char* messageId;
    char* senderId;
    char* recipientId = nullptr;
    char* downloadUrl;
    char* localFile;

    public:
    MessageRecord(char* messageId, char* senderId, char* downloadUrl, char* localFile) {

        this->messageId = (char*)malloc(strlen(messageId) + 1);
        this->senderId = (char*)malloc(strlen(senderId) + 1);
        this->downloadUrl = (char*)malloc(strlen(downloadUrl) + 1);
        this->localFile = (char*)malloc(strlen(localFile) + 1);

        strcpy(this->messageId, messageId);
        strcpy(this->senderId, senderId);
        strcpy(this->downloadUrl, downloadUrl);
        strcpy(this->localFile, localFile);
    }

    MessageRecord(char* messageId, char* recipientId, char* localFile) {

        this->messageId = (char*)malloc(strlen(messageId) + 1);
        this->recipientId = (char*)malloc(strlen(recipientId) + 1);
        this->localFile = (char*)malloc(strlen(localFile) + 1);

        strcpy(this->messageId, messageId);
        strcpy(this->recipientId, recipientId);
        strcpy(this->localFile, localFile);
    }

    ~MessageRecord() {
        free(this->messageId);
        free(this->senderId);
        if (this->recipientId != nullptr) free(this->recipientId);
        free(this->downloadUrl);
        free(this->localFile);
    }

    char* getMessageId() { return this->messageId; }
    char* getSenderId() { return this->senderId; }
    char* getRecipient() { return this->recipientId; }
    char* getDownloadUrl() { return this->downloadUrl; }
    char* getStorageLocation() { return this->localFile; }
};
#endif // __MESSAGERECORD_H__