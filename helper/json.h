#include <string.h>
#include <stdio.h>
#include "../shared/structs.h"

#ifndef JSON_H_
#define JSON_H_

char *serializeLoginModel(LoginModel model);
LoginModel deserializeLoginModel(char *json);

char *serializeMessage(Message model);
Message deserializeMessage(char *json);

#endif