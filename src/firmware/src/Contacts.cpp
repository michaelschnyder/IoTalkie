#include "Contacts.h"

bool Contacts::load() {

    logger.verbose(F("Attempting to load contacts from '%s'"), filename.c_str());

    if (!SD.exists(filename)) {
        
        logger.warning(F("Contacts file '%s' does not exist."), filename.c_str());
        return false;
    }

    File jsonFile = SD.open(filename, "r");

    if (!jsonFile) {
        logger.error(F("Cannot open contacts file '%s'"), filename.c_str());
        return false;
    }

    // Allocate a buffer to store contents of the file.
    StaticJsonBuffer<512> jsonBuffer;
    JsonArray &root = jsonBuffer.parseArray(jsonFile);
    jsonFile.close();

    if (!root.success()) {
        logger.error(F("failed to load contacts."));
        return false;
    }

    numberOfContacts = root.size();

    logger.verbose(F("Found %i contacts..."), numberOfContacts);
    
    delete[] contacts;
    contacts = new Contact[numberOfContacts];

    int i = 0;

    for (auto& element : root) {

        const char* name = element["name"];
        const char* userId = element["userId"];

        contacts[i].name = String(name);
        contacts[i].userId = String(userId);

        logger.verbose(F("Found contact[%i] '%s' with id %s"), i, name, userId);
        i++;
    }

    logger.trace(F("Contacts loaded successfully: %i"), i);
    return true;
}

Contact* Contacts::get(int position) 
{
    if (position < 0 || position >= numberOfContacts) {
        return NULL;
    }

    return &this->contacts[position];
}
