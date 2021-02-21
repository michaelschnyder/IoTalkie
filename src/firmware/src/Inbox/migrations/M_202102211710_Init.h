#ifndef __M_202102211710_INIT_H__
#define __M_202102211710_INIT_H__

#include <SchemaMigration.h>
#include <SQLiteDatabase.h>

class M_202102211710_Init : public SchemaMigration {
    
public:
    M_202102211710_Init() : SchemaMigration(202102211710, "Init") {}
    void up(SQLiteDatabase* db) {

    }
};
#endif // __M_202102211710_INIT_H__