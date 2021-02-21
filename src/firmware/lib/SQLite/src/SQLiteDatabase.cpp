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
            log_v("print results from query for the first row (later rows won't log)");
            
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

int databaseCallback(void *param, int columns, char **rowContent, char **columnNames)
{
    log_v("handling result from sqlite. delegating to future");
    
    QueryContext* ctx = (QueryContext *)param;
    ctx->_handleRowResult(param, columns, rowContent, columnNames);
    
    return 0;
}

SQLiteDatabase::SQLiteDatabase(const char *file)
{
    this->filename = file;
}

bool SQLiteDatabase::open()
{
    log_v("initalizing sqlite3");
    sqlite3_initialize();

    log_v("Attempting to open '%s'", filename);
    int rc = sqlite3_open(filename, &db);
    if (rc)
    {
        log_e("Can't open database: %s %s", sqlite3_extended_errcode(db), sqlite3_errmsg(db));
        return false;
    }

    return true;
}

bool SQLiteDatabase::close()
{
    if (db)
    {
        log_v("Attempting to close '%s'", filename);
        sqlite3_close(db);
    }

    return sqlite3_shutdown() == 0;
}

bool SQLiteDatabase::execute(const char* sql) 
{
    return this->query(sql, NULL);
}

String SQLiteDatabase::queryStringSingle(const char *sql)
{
    char resultBuff[256];
    char * target = resultBuff;

    row_callback_t cb = [target](int rowIdx, char** rowData, char** columnNames , int columns) {
        char * value = rowData[0];
        const char *p = value;
        int i = 0;
        while (*p) {
            target[i] = *p;
            p++; i++;
        }
        target[i] = '\0';
    };

    query(sql, cb);

    return target;
}

String SQLiteDatabase::queryString(const char *sql)
{
    return queryStringSingle(sql);
}

int64_t SQLiteDatabase::queryInt(const char *sql)
{
    return queryStringSingle(sql).toInt();    
}

bool SQLiteDatabase::query(const char* sql, row_callback_t callback) 
{
    if (db == NULL)
    {
        log_e("No database open");
        return false;
    }
    
    if (logQueryToConsole) {
        Serial.printf("SQLQUERY | Executing: %s", sql);
        Serial.println();
    }

    QueryContext ctx(this, callback);

    long start = micros();
    long duration = 0;    
    char *zErrMsg = 0;
    
    int rc = sqlite3_exec(db, sql, databaseCallback, &ctx, &zErrMsg);
    duration = micros() - start;

    if (rc != SQLITE_OK)
    {
        Serial.print(F("SQLQUERY | SQL error: "));
        Serial.print(sqlite3_extended_errcode(db));
        Serial.print(" ");
        Serial.println(zErrMsg);
        sqlite3_free(zErrMsg);
    }

    if (logStatsToConsole) {
        Serial.print(F("SQLSTATS | Execution completed. Time taken "));
        Serial.print(duration);
        Serial.println(F(" us"));
    }    

    return rc == SQLITE_OK;    
}

bool SQLiteDatabase::isLogResultsToConsole() 
{
    return this->logResultsToConsole;
}
