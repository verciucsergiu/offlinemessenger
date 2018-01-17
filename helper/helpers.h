#include <string.h>
#include <stdio.h>
#include "../shared/structs.h"

#ifndef HELPERS_H_
#define HELPERS_H_

char *serializeLoginModel(LoginModel model);
LoginModel deserializeLoginModel(char *json);

char *serializeMessage(Message model);
Message deserializeMessage(char *json);

#endif