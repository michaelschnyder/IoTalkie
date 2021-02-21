#ifndef __SQLITECONNECTION_H__
#define __SQLITECONNECTION_H__

#include <Arduino.h>
#include <sqlite3.h>
#include <esp32-hal-log.h>
#include <functional>

#define row_callback_t std::function<void(int, char**, char**, int)> 
#define CELL_BUFF_SIZE = 256;

class SQLiteDatabase {
    sqlite3 *db = NULL;
    const char* filename;
    bool logQueryToConsole = true;
    bool logResultsToConsole = true;
    bool logStatsToConsole = true;

    String queryStringSingle(const char* sql);
    bool query(const char* sql, row_callback_t callback);
    void debugPrintQueryResult(bool, int, char**, char**);

public:
    SQLiteDatabase(const char * file);

    bool open();
    bool close();

    bool execute(const char* sql);
    String queryString(const char* sql);
    int64_t queryInt(const char* sql);

    bool isLogResultsToConsole();
};
#endif // __SQLITECONNECTION_H__