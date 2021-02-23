#include "SQLiteConnection.h"

int _delegateDatabaseCallback(void *param, int columns, char **rowContent, char **columnNames)
{
    QueryContext* ctx = (QueryContext *)param;
    ctx->_handleRowResult(param, columns, rowContent, columnNames);    
    return 0;
}

SQLiteConnection::SQLiteConnection(SQLiteDatabase* db) 
{
    this->db = db;
    db->open();    
}

SQLiteConnection::~SQLiteConnection() 
{
    this->db->close();    

    if (this->currentResultSet) {
        this->currentResultSet->clear();
        free(this->currentResultSet);
        this->currentResultSet = NULL;
    }
}

bool SQLiteConnection::execute(const char* sql, ...) 
{
    va_list args;
    va_start(args, sql);
    
    char statement[500];
    vsprintf(statement, sql, args);
    va_end(args);

    return this->query(statement, NULL);
}

String SQLiteConnection::queryString(const char *sql, ...)
{
    va_list args;
    va_start(args, sql);
    
    char statement[500];
    vsprintf(statement, sql, args);
    va_end(args);

    return getSingleRowFirstCellValue(statement);
}

int64_t SQLiteConnection::queryInt(const char *sql, ...)
{
    va_list args;
    va_start(args, sql);
    
    char statement[500];
    vsprintf(statement, sql, args);
    va_end(args);

    String result = getSingleRowFirstCellValue(statement);

    if (result.equals("NULL")) {
        return 0;
    }
    
    return atoll(result.c_str());
}

long long atoll(const char* ptr) {
  long long result = 0;
  while (*ptr && isdigit(*ptr)) {
    result *= 10;
    result += *ptr++ - '0';
  }
  return result;
}

ResultSet* SQLiteConnection::query(const char* sql, ...) 
{
    if (this->currentResultSet) {
        currentResultSet->clear();
    }    
    else {
        currentResultSet = new ResultSet();
    }

    ResultSet* rs = this->currentResultSet;

    row_callback_t cb = [rs](int rowIdx, char** rowData, char** columnNames , int columns) {
        rs->addResult(rowIdx, rowData, columnNames, columns);
    };

    query(sql, cb);
    rs->seal();

    return currentResultSet;
}

String SQLiteConnection::getSingleRowFirstCellValue(const char *sql)
{
    char resultBuff[256];
    char * target = resultBuff;

    row_callback_t cb = [target](int rowIdx, char** rowData, char** columnNames , int columns) {

        if (rowData[0]) {
            char * value = rowData[0];
            const char *p = value;
            int i = 0;
            
            while (*p) {
                target[i] = *p;
                p++; i++;
            }
            target[i] = '\0';
        }
        else {
            strcpy(target, "NULL");
        }
    };

    query(sql, cb);

    return target;
}

bool SQLiteConnection::query(const char* sql, row_callback_t callback) 
{
    log_v("Free Heap before query(): %d\n", esp_get_free_heap_size());

    if (this->db->isLogQueryToConsole()) {
        Serial.printf("SQLQUERY | %s", sql);
        Serial.println();
    }

    QueryContext ctx(this->db, callback);

    long start = micros();
    long duration = 0;    
    char *zErrMsg = 0;
    
    int rc = sqlite3_exec(this->db->getSQLite3(), sql, _delegateDatabaseCallback, &ctx, &zErrMsg);
    duration = micros() - start;

    if (rc != SQLITE_OK) {
        int errCode = sqlite3_extended_errcode(this->db->getSQLite3());

        log_e("Failed to execute sql statement. Error %i: %s", errCode, zErrMsg);

        if (db->isLogResultsToConsole()) {
            Serial.print(F("SQLQUERY | SQL error: "));
            Serial.printf("%i: ", errCode);
            Serial.println(zErrMsg);
        }

        sqlite3_free(zErrMsg);
    }

    if (this->db->isLogStatsToConsole()) {
        Serial.print(F("SQLSTATS | Execution completed. Time taken "));
        Serial.print(duration);
        Serial.println(F(" us"));
    }    

    return rc == SQLITE_OK;
    log_v("Free Heap after query(): %d\n", esp_get_free_heap_size());
}