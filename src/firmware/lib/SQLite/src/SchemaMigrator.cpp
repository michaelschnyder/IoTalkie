#include "SchemaMigrator.h"

void SchemaMigrator::runIfMissing(SchemaMigration* migration) 
{
    log_v("Validating if migration %lli: '%s' has been applied", migration->getVersion(), migration->getName());

    if (currentVersion == UNKNOWN_VERSION) {

        if (!this->hasSchemaVersionInfo()) {
            log_i("Initializing database for schema versioning");
            conn.execute(SQL_SCHEMA_HISTORY_CREATE);        
        }
        
        currentVersion = getCurrentSchemaVersion();
    }

    if (currentVersion < migration->getVersion()) {
        long nextRank = conn.queryInt(SQL_MIGRATIONS_SELECT_MAX_INSTALLED_RANK) + 1;
        
        log_v("About to insert new entry. Rank: %li, version: %lli", nextRank, migration->getVersion());

        // Insert current migration
        conn.execute(SQL_NEW_MIGRATION_INSERT, nextRank, migration->getVersion(), migration->getName());

        log_i("Running migration %lli:'%s'.", migration->getVersion(), migration->getName());

        uint8_t start = millis();
        migration->up(&conn);
        uint8_t duration = millis() - start;
        
        // Update
        currentVersion = migration->getVersion();
        conn.execute(SQL_NEW_MIGRATION_COMPLETE, duration, nextRank);
    }
}

uint64_t SchemaMigrator::getCurrentSchemaVersion() 
{
    uint64_t dbVersion = conn.queryInt(SQL_MIGRATIONS_SELECT_MAX_VERSION);
    log_v("Current version of database is: %lli", dbVersion);

    return dbVersion;
}

bool SchemaMigrator::hasSchemaVersionInfo() 
{
    int tables = conn.queryInt(SQL_SCHEMA_HISTORY_EXISTS);
    return tables != 0;
}