#include "helpers.h"
#include <string.h>
#include <stdio.h>

char *findFiledValue(char *, char *);

char *serializeLoginModel(LoginModel model)
{
    char *json;
    json = malloc(sizeof(char) * 256);
    appendStartTojson(json);
    appendStringToJson(json, "Username", model.username);
    appendNewProp(json);
    appendStringToJson(json, "Password", model.password);
    appendEndTojson(json);
    return json;
}

LoginModel deserializeLoginModel(char *json)
{
    LoginModel model;
    char *username = findFiledValue(json, "Username");
    strcpy(model.username, username);
    char *password = findFiledValue(json, "Password");
    strcpy(model.password, password);

    return model;
}

char *findFiledValue(char *json, char *filedName)
{
    char *text = strdup(json);
    char *word;
    int i = 0, j;
    word = strtok(text, " :\"\t\n}");
    while (word != NULL)
    {
        word = strtok(NULL, " :\"\t\n}");
        if (strcmp(word, filedName) == 0)
        {
            return strtok(NULL, " :\"\t\n}");
        }
    }
    return NULL;
}

void appendStringToJson(char *json, char *fieldName, char *fieldValue)
{
    strcat(json, "\t\"");
    strcat(json, fieldName);
    strcat(json, "\" : \"");
    strcat(json, fieldValue);
    strcat(json, "\"");
}

void appendNumberToJson(char json, char *fieldName, char *fieldValue)
{
    strcat(json, "\t\"");
    strcat(json, fieldName);
    strcat(json, "\" : ");
    strcat(json, fieldValue);
}

void appendNewProp(char *json)
{
    strcat(json, ",\n");
}

void appendStartTojson(char *json)
{
    strcpy(json, "{\n");
}

void appendEndTojson(char *json)
{
    strcat(json, "\n}\0");
}
