#ifndef __SCHEMAMIGRATOR_H__
#define __SCHEMAMIGRATOR_H__

#include "SchemaMigration.h"
#include "SQLiteDatabase.h"

#define UNKNOWN_VERSION -1

class SchemaMigrator {
    
    SQLiteDatabase* db;
    uint64_t currentVersion = UNKNOWN_VERSION;

    bool hasSchemaVersionInfo();
    void initializeSchemaInfo();
    uint64_t getCurrentSchemaVersion();


    public:
        SchemaMigrator(SQLiteDatabase* db);
        void runIfMissing(SchemaMigration* migration);
};
#endif // __SCHEMAMIGRATOR_H__