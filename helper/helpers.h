#ifndef HELPERS_H_
#define HELPERS_H_


typedef struct LoginModel {
    char username[30];
    char password[30];
} LoginModel;


char * serializeLoginModel(LoginModel model);
LoginModel deserializeLoginModel(char *json);

#endif