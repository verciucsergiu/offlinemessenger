#include <my_global.h>
#include <mysql.h>

#ifndef DB_HANDLER_H_
#define DB_HANDLER_H_


MYSQL *connectToDatabase();
void createDatabase(MYSQL *connection);


#endif