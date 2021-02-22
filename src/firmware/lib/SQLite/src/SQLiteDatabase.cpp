#include "SQLiteDatabase.h"

SQLiteDatabase::SQLiteDatabase(const char *file)
{
    this->filename = file;
}

bool SQLiteDatabase::open()
{
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
        sqlite3_close(instance);
    }

    instance = nullptr;

    return sqlite3_shutdown() == 0;
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
