#include "SQLiteDatabase.h"

SQLiteDatabase::SQLiteDatabase(const char *file)
{
    this->filename = file;
}

bool SQLiteDatabase::open()
{
    log_v("Free Heap before database open(): %d", esp_get_free_heap_size());

    if (instance) {
        return true;
    }

    log_v("initalizing sqlite3");
    sqlite3_initialize();

    log_v("Attempting to open '%s'", filename);
    int rc = sqlite3_open(filename, &instance);
    if (rc)
    {
        log_e("Can't open database: %s %s", sqlite3_extended_errcode(instance), sqlite3_errmsg(instance));
        return false;
    }

    return true;
}

bool SQLiteDatabase::close()
{
    if (instance)
    {
        log_v("Attempting to close '%s'", filename);
        
        if (sqlite3_close(instance) != SQLITE_OK) {
            log_w("Could not close database '%s'");
        };
    }

    instance = nullptr;

    bool isShutdown = sqlite3_shutdown() == 0;
    log_v("Free Heap after database close(): %d", esp_get_free_heap_size());

    return isShutdown;
}

bool SQLiteDatabase::isLogQueryToConsole() 
{
    return this->logQueryToConsole;
}

bool SQLiteDatabase::isLogResultsToConsole() 
{
    return this->logResultsToConsole;
}

bool SQLiteDatabase::isLogStatsToConsole() 
{
    return this->logStatsToConsole;
}

sqlite3* SQLiteDatabase::getSQLite3() 
{
    return this->instance;
}
