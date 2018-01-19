#include "message_handler.h"

char *pushMessageToDb(char message[], char username[], char to[])
{
    int res;
    char query[2048] = "INSERT INTO Messages (Text, Username, ReplyTo) VALUES ('";
    strcat(query, message);
    strcat(query, "', '");
    strcat(query, username);
    strcat(query, "', '");
    strcat(query, to);
    strcat(query, "');\0");

    MYSQL *dbConnection = connectToDatabase();
    useDatabase(dbConnection);
    handleDBCommand(dbConnection, query);

    strcpy(query, "SELECT LAST_INSERT_ID();");
    handleDBCommand(dbConnection, query);
    MYSQL_RES *result = mysql_store_result(dbConnection);

    if (result != NULL)
    {
        MYSQL_ROW row;
        row = mysql_fetch_row(result);
        mysql_close(dbConnection);
        return row[0];
    }
}

int lastMessageId()
{
    char query[1024];
    MYSQL *dbConnection = connectToDatabase();
    useDatabase(dbConnection);
    strcpy(query, "SELECT max(Id) FROM Messages;");
    handleDBCommand(dbConnection, query);
    MYSQL_RES *result = mysql_store_result(dbConnection);

    if (result != NULL)
    {
        MYSQL_ROW row;
        row = mysql_fetch_row(result);
        mysql_close(dbConnection);
        return atoi(row[0]);
    }
}

DBMessage getMessageById(char id[])
{
    char query[1024];
    MYSQL *dbConnection = connectToDatabase();
    useDatabase(dbConnection);
    strcpy(query, "SELECT Id, Text, Username, ReplyTo FROM Messages WHERE Id = ");
    strcat(query, id);
    strcat(query, ";\0");
    handleDBCommand(dbConnection, query);
    MYSQL_RES *result = mysql_store_result(dbConnection);
    DBMessage msg;
    if (result != NULL)
    {
        MYSQL_ROW row;
        row = mysql_fetch_row(result);
        mysql_close(dbConnection);

        strcpy(msg.id, row[0]);
        strcpy(msg.text, row[1]);
        strcpy(msg.username, row[2]);
        strcpy(msg.replyTo, row[3]);

        return msg;
    }
}

int updateLastMessageReceived(char username[], char id[])
{
    char query[1024];
    MYSQL *dbConnection = connectToDatabase();
    useDatabase(dbConnection);
    strcpy(query, "UPDATE Users SET LastMessageId = ");
    strcat(query, id);
    strcat(query, " WHERE Username = '");
    strcat(query, username);
    strcat(query, "';\0");

    return handleDBCommand(dbConnection, query);
}