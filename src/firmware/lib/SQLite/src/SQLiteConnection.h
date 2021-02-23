#ifndef __SQLITECONNECTION_H__
#define __SQLITECONNECTION_H__

#include "SQLiteDatabase.h"
#include "QueryContext.h"
#include "ResultSet.h"

class SQLiteConnection {

    SQLiteDatabase* db;
    String getSingleRowFirstCellValue(const char* sql);
    ResultSet* currentResultSet = nullptr;
    bool query(const char* sql, row_callback_t callback);

public:
    SQLiteConnection(SQLiteDatabase*);  
    ~SQLiteConnection();

    bool execute(const char* sql, ...);
    String queryString(const char* sql, ...);
    int64_t queryInt(const char* sql, ...);
    ResultSet* query(const char* sql, ...);

};
#endif // __SQLITECONNECTION_H__