#ifndef __M_202102211710_INIT_H__
#define __M_202102211710_INIT_H__

#include <SQLite.h>
#include <SchemaMigration.h>

class M_202109211300_Init : public SchemaMigration {

public:
    M_202109211300_Init() : SchemaMigration(202109211300, "Init") {}
    void up(SQLiteConnection* conn) {
        
        conn->execute("CREATE TABLE \"inbox\" (\n"
                    "	\"messageId\"	    VARCHAR(50) NOT NULL UNIQUE,\n"
                    "	\"timestamp\"	    INTEGER     NOT NULL,\n"
                    "	\"senderId\"	    VARCHAR(50) NOT NULL,\n"
                    "	\"size\"            INTEGER     NOT NULL,\n"
                    "	\"remoteUrl\"	    VARCHAR(255),\n"
                    "	\"downloadCount\"	INTEGER     NOT NULL DEFAULT 0,\n"
                    "	\"localFile\"	    VARCHAR(128),\n"
                    "	\"playCount\"	    INTEGER     NOT NULL DEFAULT 0,\n"
                    "	\"isFaulty\"        INTEGER NOT NULL DEFAULT 0"
                    ");\n"
                    );

        conn->execute("CREATE        INDEX \"idx_inbox_senderId\"   ON \"inbox\" (\"senderId\"  ASC)");
        conn->execute("CREATE UNIQUE INDEX \"idx_inbox_messageId\"  ON \"inbox\" (\"messageId\" ASC)");

        conn->execute("CREATE TABLE \"outbox\" (\n"
                    "	\"messageId\"	VARCHAR(50) NOT NULL UNIQUE,\n"
                    "	\"timestamp\"	INTEGER     NOT NULL,\n"
                    "	\"recipientId\"	VARCHAR(50) NOT NULL,\n"
                    "	\"sentCount\"	INTEGER     NOT NULL DEFAULT 0,\n"
                    "	\"localFile\"	VARCHAR(128),\n"
                    "	\"isSent\"      INTEGER NOT NULL DEFAULT 0,"
                    "	\"isFaulty\"    INTEGER NOT NULL DEFAULT 0"
                    ");\n"
                    );

        conn->execute("CREATE UNIQUE INDEX \"idx_outbox_messageId\"  ON \"outbox\" (\"messageId\" ASC)");
    } 
};
#endif // __M_202102211710_INIT_H__