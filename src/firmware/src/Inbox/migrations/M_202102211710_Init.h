#ifndef __M_202102211710_INIT_H__
#define __M_202102211710_INIT_H__

#include <SQLite.h>
#include <SchemaMigration.h>

class M_202102211710_Init : public SchemaMigration {

public:
    M_202102211710_Init() : SchemaMigration(202102211710, "Init") {}
    void up(SQLiteConnection* conn) {
        
        conn->execute("CREATE TABLE \"messages\" (\n"
                    "	\"messageId\"	VARCHAR(50) NOT NULL UNIQUE,\n"
                    "	\"timestamp\"	INTEGER     NOT NULL,\n"
                    "	\"senderId\"	VARCHAR(50) NOT NULL,\n"
                    "	\"size\"        INTEGER     NOT NULL,\n"
                    "	\"remoteUrl\"	VARCHAR(255),\n"
                    "	\"localFile\"	VARCHAR(128),\n"
                    "	\"playCount\"	INTEGER     NOT NULL DEFAULT 0\n"
                    ");\n"
                    );

        conn->execute("CREATE UNIQUE INDEX \"idx_messages_messageId\" ON \"messages\" (\"messageId\" ASC)");
    } 
};
#endif // __M_202102211710_INIT_H__