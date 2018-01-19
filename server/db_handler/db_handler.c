#include "db_handler.h"

MYSQL *connectToDatabase()
{
    MYSQL *con = mysql_init(NULL);
    if (mysql_real_connect(con, "localhost", "root", "root", NULL, 0, NULL, 0) == NULL)
    {
        fprintf(stderr, "%s\n", mysql_error(con));
        printf("Unable to connect to the database!\nExit is needed!\n");
        mysql_close(con);
        exit(1);
    }
    return con;
}

void createDatabase(MYSQL *connection)
{
    handleDBCommand(connection, "CREATE DATABASE IF NOT EXISTS OfflineMessenger;");
    useDatabase(connection);
    handleDBCommand(connection, "CREATE TABLE IF NOT EXISTS Users (Id MEDIUMINT NOT NULL AUTO_INCREMENT, Username CHAR(30) NOT NULL, Password CHAR(30) NOT NULL, PRIMARY KEY (id), UNIQUE(Username));");
    handleDBCommand(connection, "CREATE TABLE IF NOT EXISTS Messages (Id MEDIUMINT NOT NULL AUTO_INCREMENT, Text TEXT NOT NULL, Username CHAR(30) NOT NULL, ReplyTo MEDIUMINT NOT NULL DEFAULT 0, PRIMARY KEY (id));");
    handleDBCommand(connection, "INSERT INTO Users (Username, Password) SELECT * FROM (SELECT 'serjblue', '1q2w3e4r') AS tmp WHERE NOT EXISTS (SELECT username FROM Users WHERE username = 'serjblue') LIMIT 1;");
    handleDBCommand(connection, "INSERT INTO Messages (Id, Text, Username, ReplyTo) SELECT * FROM (SELECT 1, 'Hello there! This is the first message!', 'sys_admin', 0) AS tmp WHERE NOT EXISTS (SELECT id FROM Messages WHERE id = 1) LIMIT 1;");
    printf("Database created with success!\n");
}

int useDatabase(MYSQL *connection)
{
    return handleDBCommand(connection, "USE OfflineMessenger;");
}

int handleDBCommand(MYSQL *connection, char *command)
{
    if (mysql_real_query(connection, command, strlen(command)))
    {
        fprintf(stderr, "%s query : %s\n", mysql_error(connection), command);
        return 0;
    }
    return 1;
}