#include "login.h"

int authenticateUser(char username[], char password[])
{
    int res;
    char query[256] = "SELECT count(1) FROM Users WHERE username = '";
    strcat(query, username);
    strcat(query, "' AND Password = '");
    strcat(query, password);
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
        printf("here : %d", res);
    }
    mysql_free_result(result);
    mysql_close(dbConnection);
    return res;
}
