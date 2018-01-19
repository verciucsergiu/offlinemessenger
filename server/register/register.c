#include "register.h"

int canUserRegister(char username[])
{
    int res;
    char query[256] = "SELECT count(1) FROM Users WHERE Username = '";
    strcat(query, username);
    strcat(query, "';\0");

    MYSQL *dbConnection = connectToDatabase();
    useDatabase(dbConnection);
    handleDBCommand(dbConnection, query);
    MYSQL_RES *result = mysql_store_result(dbConnection);

    if (result != NULL)
    {
        MYSQL_ROW row;
        row = mysql_fetch_row(result);
        res = atoi(row[0]);
    }
    mysql_free_result(result);
    mysql_close(dbConnection);
    return !res;
}
int registerUser(char username[], char password[])
{
    char query[256] = "INSERT INTO Users (Username, Password) VALUES ('";
    strcat(query, username);
    strcat(query, "', '");
    strcat(query, password);
    strcat(query, "');\0");

    MYSQL *dbConnection = connectToDatabase();
    useDatabase(dbConnection);
    if (!handleDBCommand(dbConnection, query))
    {
        mysql_close(dbConnection);
        return 0;
    }
    mysql_close(dbConnection);
    return 1;
}