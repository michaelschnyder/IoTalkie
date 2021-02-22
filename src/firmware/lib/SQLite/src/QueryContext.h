#ifndef __QUERYCONTEXT_H__
#define __QUERYCONTEXT_H__

#include "SQLiteDatabase.h"

class QueryContext {

    SQLiteDatabase* db;
    row_callback_t rowCallback;
    int rows = 0;

    public:
    QueryContext(SQLiteDatabase* db, row_callback_t rowCallback) {
        this->db = db;
        this->rowCallback = rowCallback;
    }

    void _handleRowResult(void *param, int columns, char **rowContent, char **columnNames) {
        log_v("handling result from sqlite. columns: %i", columns);
        int currentRow = rows;
        rows++;
        
        if (db->isLogResultsToConsole()) {
            debugPrintQueryResult(currentRow == 0, columns, rowContent, columnNames);
        } 

        if (this->rowCallback != NULL)
        {
            this->rowCallback(currentRow, rowContent, columnNames, columns);
        }
    }

    void debugPrintQueryResult(bool isFirstRow, int columns, char** row, char** columnNames) 
    {
        if (isFirstRow) {
            Serial.printf("%s", "SQLRESULT| ");
            for (int i = 0; i < columns; i++)
            {
                if (i) { Serial.print((char)'\t'); }
                Serial.printf("%s", columnNames[i]);
            }
            Serial.println();
        }

        Serial.printf("%s", "SQLRESULT| ");
        for (int i = 0; i < columns; i++)
        {
            if (i) { Serial.print((char)'\t'); }
            Serial.printf("%s", row[i] ? row[i] : "NULL");
        }

        Serial.println();
    }
};


#endif // __QUERYCONTEXT_H__