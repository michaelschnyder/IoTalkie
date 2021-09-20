#ifndef __M_202120092116_ADDFAULTYCOLUMN_H__
#define __M_202120092116_ADDFAULTYCOLUMN_H__

#include <SQLite.h>
#include <SchemaMigration.h>

class M_202120092116_AddFaultyColumn : public SchemaMigration {

public:
    M_202120092116_AddFaultyColumn() : SchemaMigration(202120092116, "AddFaultyColumn") {}
    void up(SQLiteConnection* conn) {
        
        conn->execute("ALTER TABLE messages ADD COLUMN isFaulty INTEGER NOT NULL DEFAULT 0;\n");
    
        conn->execute("CREATE INDEX \"idx_messages_senderId\" ON \"messages\" (\"senderId\" ASC)");
    } 
};
#endif // __M_202120092116_ADDFAULTYCOLUMN_H__