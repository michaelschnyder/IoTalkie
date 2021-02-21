#include "SchemaMigrator.h"

SchemaMigrator::SchemaMigrator(SQLiteDatabase* db) 
{
    this->db = db;
}

void SchemaMigrator::runIfMissing(SchemaMigration* migration) 
{
    char buff[24 + 20];

    if (!this->hasSchemaVersionInfo()) {
        log_i("Initializing database for schema versioning");
        this->initializeSchemaInfo();
    }

    if (currentVersion == UNKNOWN_VERSION) {
        currentVersion = getCurrentSchemaVersion();
    }

    if (currentVersion < migration->getVersion()) {
        log_i("Running migration %ld.", migration->getVersion());

        migration->up(db);
        currentVersion = migration->getVersion();
        
        sprintf(buff, "PRAGMA user_version = %ld;", currentVersion);        
        db->queryInt(buff);
    }
}


uint64_t SchemaMigrator::getCurrentSchemaVersion() 
{
//    long dbVersion = db->queryInt("PRAGMA user_version;");
    long dbVersion = db->queryInt("PRAGMA table_info(TEST);");
    log_v("Current version of database is: %ld", dbVersion);

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
