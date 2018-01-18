#include "json.h"

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

char *serializeMessage(Message msg)
{
    char *json;
    json = malloc(sizeof(char) * 256);
    appendStartTojson(json);
    appendStringToJson(json, "Id", msg.id);
    appendNewProp(json);
    appendStringToJson(json, "ReplyTo", msg.replyTo);
    appendNewProp(json);
    appendStringToJson(json, "Text", msg.text);
    appendNewProp(json);
    appendStringToJson(json, "Username", msg.username);
    appendEndTojson(json);
    return json;
}

Message deserializeMessage(char *json)
{
    Message msg;
    char *id = findFiledValue(json, "Id");
    strcpy(msg.id, id);
    char *to = findFiledValue(json, "ReplyTo");
    strcpy(msg.replyTo, to);
    char *text = findFiledValue(json, "Text");
    strcpy(msg.text, text);
    char *username = findFiledValue(json, "Username");
    strcpy(msg.username, username);
    return msg;
}

char *findFiledValue(char *json, char *filedName)
{
    char *text = strdup(json);
    char *word;
    word = strtok(text, ":\"}");
    while (word != NULL)
    {
        word = strtok(NULL, ":\"}");

        if (strcmp(word, filedName) == 0)
        {
            return strtok(NULL, ":\"}");
        }
    }
    return NULL;
}

void appendStringToJson(char *json, char *fieldName, char *fieldValue)
{
    strcat(json, "\"");
    strcat(json, fieldName);
    strcat(json, "\":\"");
    strcat(json, fieldValue);
    strcat(json, "\"");
}

void appendNewProp(char *json)
{
    strcat(json, ",");
}

void appendStartTojson(char *json)
{
    strcpy(json, "{");
}

void appendEndTojson(char *json)
{
    strcat(json, "}\0");
}