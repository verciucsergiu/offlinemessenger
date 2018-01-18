#include <string.h>
#include <stdio.h>
#include "../db_handler/db_handler.h"

#ifndef REGISTER_H_
#define REGISTER_H_

int canUserRegister(char username[]);
int registerUser(char username[], char password[]);

#endif