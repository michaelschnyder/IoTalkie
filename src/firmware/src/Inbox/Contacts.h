#ifndef __CONTACTS_H__
#define __CONTACTS_H__

#include <Log4Esp.h>
#include <SD.h>
#include <ArduinoJson.h>

#define NAME_MAX_LENGTH 20
#define USER_ID_MAX_LENGTH 32

typedef struct Contact {
    int slot;
    char name[NAME_MAX_LENGTH + 1];
    char userId[USER_ID_MAX_LENGTH + 1];
};

class Contacts {
    const String filename = "/contacts.json";

    log4Esp::Logger logger = log4Esp::Logger("Contacts");

    Contact* contacts;
    int numberOfContacts;

public:
    bool load();
    Contact* get(int slot);
    Contact* findByUserId(char* userId);

};
#endif // __CONTACTS_H__