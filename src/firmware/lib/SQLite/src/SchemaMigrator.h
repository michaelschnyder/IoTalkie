#ifndef __SCHEMAMIGRATOR_H__
#define __SCHEMAMIGRATOR_H__

#include "SchemaMigration.h"
#include "SQLiteDatabase.h"

#define UNKNOWN_VERSION -1

class SchemaMigrator {

    const char* SQL_SCHEMA_HISTORY_EXISTS = "SELECT count(*) FROM sqlite_master WHERE type ='table' AND name = '__schemaHistory';";
    const char* SQL_MIGRATIONS_SELECT_MAX_VERSION = "SELECT MAX(version) FROM __schemaHistory WHERE success == 1;";
    const char* SQL_MIGRATIONS_SELECT_MAX_INSTALLED_RANK = "SELECT MAX(installed_rank) FROM __schemaHistory;";
    const char* SQL_NEW_MIGRATION_INSERT = "INSERT INTO __schemaHistory (installed_rank, version, description, execution_time, success) VALUES(%li, '%lli', '%s', -1, 0);";
    const char* SQL_NEW_MIGRATION_COMPLETE = "UPDATE __schemaHistory SET success = 1, execution_time = %i WHERE installed_rank = %li";
    const char* SQL_SCHEMA_HISTORY_CREATE = "CREATE TABLE __schemaHistory (\n" 
                                            "    \"installed_rank\" INT NOT NULL PRIMARY KEY,\n" 
                                            "    \"version\" VARCHAR(50),\n" 
                                            "    \"description\" VARCHAR(200) NOT NULL,\n" 
                                            "    \"execution_time\" INT NOT NULL,\n" 
                                            "    \"success\" BOOLEAN NOT NULL\n" 
                                            ");\n"
                                            "CREATE UNIQUE INDEX \"idx___schemaHistory_version_success\" ON \"__schemaHistory\" (\"version\" DESC, \"success\" DESC);\n"
                                            "CREATE UNIQUE INDEX \"idx___schemaHistory_installed_rank\" ON \"__schemaHistory\" (\"installed_rank\" DESC);\n"
                                            ;

    SQLiteDatabase* db;
    SQLiteConnection conn;
    uint64_t currentVersion = UNKNOWN_VERSION;

    bool hasSchemaVersionInfo();
    uint64_t getCurrentSchemaVersion();

    public:
        SchemaMigrator(SQLiteDatabase* db) : conn(db) {};
        void runIfMissing(SchemaMigration* migration);
};
#endif // __SCHEMAMIGRATOR_H__