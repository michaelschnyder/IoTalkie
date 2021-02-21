#ifndef __SCHEMAMIGRATION_H__
#define __SCHEMAMIGRATION_H__
#include "SQLiteDatabase.h"

class SchemaMigration {

    int64_t version;
    const char* name;

    public:

    SchemaMigration(int64_t version, const char* name) {
        this->version = version;
        this->name = name;
    }

    int64_t getVersion() { return version; }
    const char* getName() { return name ;}
    virtual void up(SQLiteDatabase* db) = 0;
};
#endif // __SCHEMAMIGRATION_H__