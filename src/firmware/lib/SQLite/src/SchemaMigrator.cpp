#include "SchemaMigrator.h"

SchemaMigrator::SchemaMigrator(SQLiteDatabase* db) 
{
    this->db = db;
}

void SchemaMigrator::runIfMissing(SchemaMigration* migration) 
{
    log_v("Validating if version %lli '%s' has been applied", migration->getVersion(), migration->getName());

    char buff[500];

    if (currentVersion == UNKNOWN_VERSION) {

        if (!this->hasSchemaVersionInfo()) {
            log_i("Initializing database for schema versioning");
            this->initializeSchemaInfo();
        }

        currentVersion = getCurrentSchemaVersion();
    }

    if (currentVersion < migration->getVersion()) {
        int nextRank = db->queryInt("SELECT MAX(installed_rank) FROM __schemaHistory;") + 1;
        
        log_v("About to insert new entry. Rank: %li, version: %lli", nextRank, migration->getVersion());

        // Insert current migration
        sprintf(buff, "INSERT INTO __schemaHistory (installed_rank, version, description, execution_time, success) VALUES(%li, '%lli', '%s', -1, 0);", 
                                                    nextRank, migration->getVersion(), migration->getName());
        db->execute(buff);

        log_i("Running migration %lli ('%s').", migration->getVersion(), migration->getName());
        uint8_t start = millis();
        migration->up(db);
        uint8_t duration = millis() - start;
        
        // Update
        currentVersion = migration->getVersion();
        sprintf(buff, "UPDATE __schemaHistory SET success = 1, execution_time = %i WHERE installed_rank = %li", duration, nextRank);
        db->execute(buff);
    }
}

uint64_t SchemaMigrator::getCurrentSchemaVersion() 
{
    uint64_t dbVersion = db->queryInt("SELECT MAX(version) FROM __schemaHistory WHERE success == 1;");
    log_v("Current version of database is: %lli", dbVersion);

    return dbVersion;
}

bool SchemaMigrator::hasSchemaVersionInfo() 
{
    int tables = db->queryInt("SELECT count(*) FROM sqlite_master WHERE type ='table' AND name = '__schemaHistory';");
    return tables != 0;
}

void SchemaMigrator::initializeSchemaInfo() 
{
const char *create_table =
    "CREATE TABLE __schemaHistory (\n" 
    "    \"installed_rank\" INT NOT NULL PRIMARY KEY,\n" 
    "    \"version\" VARCHAR(50),\n" 
    "    \"description\" VARCHAR(200) NOT NULL,\n" 
    "    \"execution_time\" INT NOT NULL,\n" 
    "    \"success\" BOOLEAN NOT NULL\n" 
    ");\n";

    db->execute(create_table);
}
