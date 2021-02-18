#ifndef __CONTACTS_H__
#define __CONTACTS_H__

#include <Log4Esp.h>
#include <SD.h>
#include <ArduinoJson.h>

typedef struct ContactElement {
    String name;
    String userId;
};

class Contacts {
    const String filename = "/contacts.json";

    log4Esp::Logger logger = log4Esp::Logger("Contacts");

    ContactElement* contacts;

    public:
        bool load();
        String getName(int position);
        String getUserId(int position);

};
#endif // __CONTACTS_H__