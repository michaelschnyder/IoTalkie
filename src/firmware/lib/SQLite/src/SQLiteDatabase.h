#ifndef __SQLITEDATABASE_H__
#define __SQLITEDATABASE_H__

#include <Arduino.h>
#include <sqlite3.h>
#include <esp32-hal-log.h>
#include <functional>

#define row_callback_t std::function<void(int, char**, char**, int)> 
#define CELL_BUFF_SIZE = 256;

class SQLiteDatabase {
    sqlite3 *instance = NULL;
    const char* filename;

    bool logQueryToConsole = true;
    bool logResultsToConsole = true;
    bool logStatsToConsole = true;

    void debugPrintQueryResult(bool, int, char**, char**);

public:
    SQLiteDatabase(const char * file);

    bool open();
    bool close();

    bool isLogQueryToConsole();
    bool isLogResultsToConsole();
    bool isLogStatsToConsole();
    sqlite3* getSQLite3();
};
#endif // __SQLITEDATABASE_H__