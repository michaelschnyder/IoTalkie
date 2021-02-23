#include "ResultSet.h"

void ResultSet::clear() 
{
    log_v("Clearing ResultSet so that it can be re-used");

    this->isSealed = false;
    this->rowWriteIdx = 0;
    this->rowReadIdx = -1;
    this->columns = 0;

    int dataSize = rowWriteIdx * columns;
    for (size_t i = 0; i < dataSize; i++)
    {
        free(data[i]);
        data[i] = NULL;   
    }

    free(data);
    data = NULL;
}

void ResultSet::addResult(int rowIdx, char** rowData, char** columnNames , int columns) 
{
    if (this->isSealed) {
        log_e("illegal access. ResultSet is sealed already.");
        return;
    }

    this->columns = columns;

    if (rowWriteIdx == 0) {
        data = (char**)malloc(columns * sizeof(char*));
    }
    else {
        // create larger data storage
        return;
    }

    long rowOffset = rowWriteIdx * columns;

    for (size_t column = 0; column < columns; column++)
    {
        char* src;

        if (rowData[column]) {
            src = rowData[column];
        }
        else {
            src = "NULL";
        }

        int len = strlen(src);
        int memSize = len + 1;
        int targetIndex = rowOffset + column;

        log_v("Allocating %i bytes for result of row %i, column %i", memSize, rowWriteIdx, column);
        data[targetIndex] = (char *)malloc(memSize);
        strcpy(data[targetIndex], src);
    }

    rowWriteIdx++;
}

void ResultSet::seal() 
{
    this->isSealed = true;
}

char* ResultSet::getString(int index) 
{
    if (rowReadIdx < 0 || rowReadIdx < rowWriteIdx -1) {
        log_e("No data to read, data ended");
        return "";
    }

    if (index < 0 || index >= columns) {
        log_e("Index out of bounds.");
    }

    long offset = rowReadIdx * columns + index;

    log_v("accessing data at offset %i", offset);

    return data[offset];
}

bool ResultSet::read() 
{
    if (rowReadIdx < rowWriteIdx - 1) {
        rowReadIdx++;
        return true;
    }
    
    return false;
}
