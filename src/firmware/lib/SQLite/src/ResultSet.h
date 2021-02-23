#ifndef __RESULTSET_H__
#define __RESULTSET_H__

#include "Arduino.h"

class ResultSet {

    bool isSealed;
    int rowWriteIdx = 0;
    int rowReadIdx = -1;
    int columns;

    char** data;

public:
    char* getString(int index);
    bool read();
    void clear();
    void addResult(int rowIdx, char** rowData, char** columnNames , int columns);
    
    void seal();
};
#endif // __RESULTSET_H__