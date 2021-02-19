#ifndef __CONTACTS_H__
#define __CONTACTS_H__

#include <Log4Esp.h>
#include <SD.h>
#include <ArduinoJson.h>

typedef struct Contact {
    String name;
    String userId;
};

class Contacts {
    const String filename = "/contacts.json";

    log4Esp::Logger logger = log4Esp::Logger("Contacts");

    Contact* contacts;
    int numberOfContacts;
    
    public:
        bool load();
        Contact* get(int position);

};
#endif // __CONTACTS_H__