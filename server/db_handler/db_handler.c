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
    printf("Connected to the database with success!\n");
    return con;
}

void createDatabase(MYSQL *connection)
{
    handleDBCommand(connection, "CREATE DATABASE IF NOT EXISTS OfflineMessenger;");
    useDatabase(connection);
    handleDBCommand(connection, "CREATE TABLE IF NOT EXISTS Users (Id MEDIUMINT NOT NULL AUTO_INCREMENT, Username CHAR(30) NOT NULL, Password CHAR(30) NOT NULL, PRIMARY KEY (id), UNIQUE(Username));");
    handleDBCommand(connection, "INSERT INTO Users (Username, Password) SELECT * FROM (SELECT 'serjblue', '1q2w3e4r') AS tmp WHERE NOT EXISTS (SELECT username FROM Users WHERE username = 'serjblue') LIMIT 1;");
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
        fprintf(stderr, "%s\n", mysql_error(connection));
        return 0;
    }
    return 1;
}