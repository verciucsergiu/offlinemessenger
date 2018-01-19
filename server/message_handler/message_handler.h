#include <string.h>
#include "../db_handler/db_handler.h"

#ifndef MSG_HANDLER_H_
#define MSG_HANDLER_H_

typedef struct DBMessage
{
    char id[100];
    char text[1024];
    char username[31];
    char replyTo[100];
} DBMessage;

char *pushMessageToDb(char message[], char username[], char to[]);
int lastMessageId();

DBMessage getMessageById(char id[]);

#endif