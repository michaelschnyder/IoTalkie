#include "SchemaMigrator.h"

SchemaMigrator::SchemaMigrator(SQLiteDatabase* db) 
{
    this->db = db;
}

void SchemaMigrator::runIfMissing(SchemaMigration* migration) 
{
    log_v("Validating if migration %lli '%s' has been applied", migration->getVersion(), migration->getName());

    char buff[500];

    if (currentVersion == UNKNOWN_VERSION) {

        if (!this->hasSchemaVersionInfo()) {
            log_i("Initializing database for schema versioning");
            db->execute(SQL_SCHEMA_HISTORY_CREATE);        
        }
        
        currentVersion = getCurrentSchemaVersion();
    }

    if (currentVersion < migration->getVersion()) {
        long nextRank = db->queryInt(SQL_MIGRATIONS_SELECT_MAX_INSTALLED_RANK) + 1;
        
        log_v("About to insert new entry. Rank: %li, version: %lli", nextRank, migration->getVersion());

        // Insert current migration
        sprintf(buff, SQL_NEW_MIGRATION_INSERT, nextRank, migration->getVersion(), migration->getName());
        db->execute(buff);

        log_i("Running migration %lli ('%s').", migration->getVersion(), migration->getName());

        uint8_t start = millis();
        migration->up(db);
        uint8_t duration = millis() - start;
        
        // Update
        currentVersion = migration->getVersion();
        sprintf(buff, SQL_NEW_MIGRATION_COMPLETE, duration, nextRank);
        db->execute(buff);
    }
}

uint64_t SchemaMigrator::getCurrentSchemaVersion() 
{
    uint64_t dbVersion = db->queryInt(SQL_MIGRATIONS_SELECT_MAX_VERSION);
    log_v("Current version of database is: %lli", dbVersion);

    return dbVersion;
}

bool SchemaMigrator::hasSchemaVersionInfo() 
{
    int tables = db->queryInt(SQL_SCHEMA_HISTORY_EXISTS);
    return tables != 0;
}