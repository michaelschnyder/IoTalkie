#ifndef __SCHEMAMIGRATION_H__
#define __SCHEMAMIGRATION_H__
#include "SQLiteDatabase.h"

class SchemaMigration {

private:
    int64_t version;
    const char* name;

public:

    SchemaMigration(int64_t version, const char* name) {
        this->version = version;
        this->name = name;
    }

    int64_t getVersion() { return this->version; }
    const char* getName() { return this->name ;}
    virtual void up(SQLiteDatabase* db) = 0;
};
#endif // __SCHEMAMIGRATION_H__