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

        contacts[i].slot = i;
        strncpy(contacts[i].name, name, NAME_MAX_LENGTH);
        strncpy(contacts[i].userId, userId, USER_ID_MAX_LENGTH);

        logger.verbose(F("Found contact[%i] '%s' with id %s"), i, name, userId);
        i++;
    }

    logger.trace(F("Contacts loaded successfully: %i"), i);
    return true;
}

Contact* Contacts::get(int slot) 
{
    if (slot < 0 || slot >= numberOfContacts) {
        return NULL;
    }

    return &this->contacts[slot];
}

Contact* Contacts::findByUserId(char* userId) 
{
    logger.verbose(F("Trying to find contact with userId '%s'"), userId);

    for (size_t i = 0; i < this->numberOfContacts; i++)
    {
        if (strcmp(contacts[i].userId, userId) == 0) {
            return &this->contacts[i];
        }
    }

    logger.verbose(F("Unable to find contact with userId '%s', returning NULL"), userId);

    return NULL;
}

int Contacts::size() 
{
    return this->numberOfContacts;
}

void Contacts::update(String remoteUrl) {
    
    auto oldFile = filename  + ".old";
    auto tmpFile = filename + ".tmp";
    
    taskHttp.download(remoteUrl.c_str(), (tmpFile).c_str(), [this, oldFile, tmpFile](bool successful) {
        
        if (successful) {

            logger.trace("Contacts download successful, moving files and try to load...");
            
            // Rename old
            SD.rename(filename, oldFile);
            SD.rename(tmpFile, filename);

            logger.trace("Loading new version of contacts.json");
            if (this->load()) {

                if (this->contactsUpdatedCb != NULL) {
                    this->contactsUpdatedCb(true);
                }

                logger.trace("Contacts refreshed successfully. Removing old file.");
                SD.remove(oldFile);
            }
            else {
                logger.error("Could not update contacts. rolling back");
                SD.remove(filename);
                SD.rename(oldFile, filename);

                if (this->contactsUpdatedCb != NULL) {
                    this->contactsUpdatedCb(false);
                }
            }
        }
        else {
            logger.error("Downloading of new contacts.json was not successful.");
        }
    });
}