
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <pthread.h>

#include "server_structures.h"
#include "db_handler/db_handler.h"
#include "login/login.h"
#include "register/register.h"
#include "message_handler/message_handler.h"
#include "../helper/json.h"

#define PORT 3001

extern int errno;

ClientsCollection clients;
MYSQL *dbConnection;

static void *executeThread(void *);
void treatUser(void *);
void sendUserMessage(char msg[]);
char *getRequestType(char *);
void porcessRequest(char msg[], void *);

void appendClient(int, int);

int main()
{
    struct sockaddr_in server;
    struct sockaddr_in from;

    char *msg;
    int sd;
    pthread_t th[200];
    int i = 0;

    dbConnection = connectToDatabase();
    createDatabase(dbConnection);
    mysql_close(dbConnection);
    if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("Eroare la socket.\n");
        return errno;
    }

    int on = 1;

    setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

    bzero(&server, sizeof(server));
    bzero(&from, sizeof(from));

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons(PORT);

    if (bind(sd, (struct sockaddr *)&server, sizeof(struct sockaddr)) == -1)
    {
        perror("Eroare la bind.\n");
        return errno;
    }

    if (listen(sd, 2) == -1)
    {
        perror("Eroare la listen.\n");
        return errno;
    }

    printf("Server up and running! Users now can connect!\n");
    fflush(stdout);

    while (1)
    {
        int client;
        thread *td;
        int length = sizeof(from);
        if ((client = accept(sd, (struct sockaddr *)&from, &length)) < 0)
        {
            perror("[server]Eroare la accept().\n");
            continue;
        }

        td = (struct thread *)malloc(sizeof(struct thread));
        td->id = i;
        td->client = client;

        /* adaugarea clientului intr-o coletie.*/
        appendClient(i, client);

        pthread_create(&th[i++], NULL, &executeThread, td);
    }
}

void appendClient(int index, int client)
{
    clients.list[index].client = client;
    clients.list[index].connected = 1;
    clients.count = index;
}

void appendUsernameToClientsCollection(int index, char username[])
{
    strcpy(clients.list[index].username, username);
}

static void *executeThread(void *arg)
{
    struct thread tdL;
    tdL = *((struct thread *)arg);
    fflush(stdout);
    pthread_detach(pthread_self());
    treatUser((struct thread *)arg);
    close((intptr_t)arg);
    return NULL;
}

void treatUser(void *arg)
{
    while (1)
    {
        char request[1024];
        memset(request, '\0', sizeof(char) * 1024);
        struct thread currentThread;
        currentThread = *((struct thread *)arg);
        if (read(currentThread.client, &request, sizeof(request)) <= 0)
        {
            printf("[Thread %d] -> Disconnected. Connection lost.\n", currentThread.id);
            disconnect(currentThread.id);
            return;
        }
        else
        {
            porcessRequest(request, &currentThread);
        }
    }
}

void porcessRequest(char request[1024], void *arg)
{
    struct thread currentThread;
    currentThread = *((struct thread *)arg);

    char *duplicatedRequest = strdup(request);
    char *requestType = strtok(duplicatedRequest, " ");

    if (strcmp(requestType, "login") == 0)
    {
        strcpy(request, request + 5);

        LoginModel model = deserializeLoginModel(request);

        if (authenticateUser(model.username, model.password))
        {
            if (write(currentThread.client, "200\0", 5) <= 0)
            {
                perror("[Thread]Eroare la write() catre client.\n");
            }
            strcpy(currentThread.username, model.username);
            appendUsernameToClientsCollection(currentThread.id, model.username);
        }
        else
        {
            if (write(currentThread.client, "404\0", 5) <= 0)
            {
                perror("[Thread]Eroare la write() catre client.\n");
            }
        }
    }
    else if (strcmp(requestType, "send") == 0)
    {
        strcpy(request, request + 4);
        sendUserMessage(request);
    }
    else if (strcmp(requestType, "register") == 0)
    {
        strcpy(request, request + 8);

        LoginModel model = deserializeLoginModel(request);

        if (canUserRegister(model.username))
        {
            if (registerUser(model.username, model.password))
            {
                if (write(currentThread.client, "201\0", 5) <= 0)
                {
                    perror("[Thread]Eroare la write() catre client.\n");
                }
            }
            else
            {
                if (write(currentThread.client, "500\0", 5) <= 0)
                {
                    perror("[Thread]Eroare la write() catre client.\n");
                }
            }
        }
        else
        {
            if (write(currentThread.client, "404\0", 5) <= 0)
            {
                perror("[Thread]Eroare la write() catre client.\n");
            }
        }
    }
    else if (strcmp(requestType, "/close"))
    {
        if (write(currentThread.client, "close\0", 7) <= 0)
        {
            perror("[Thread]Eroare la write() catre client.\n");
        }
    }
}

void sendUserMessage(char msg[1024])
{
    Message message = deserializeMessage(msg);
    char *id = pushMessageToDb(message.text, message.username, message.replyTo);
    strcpy(message.id, id);
    strcpy(msg, serializeMessage(message));
    int i;
    for (i = 0; i <= clients.count; i++)
    {
        if (clients.list[i].connected)
        {
            if (write(clients.list[i].client, msg, strlen(msg) + 1) <= 0)
            {
                printf("Conncetion lost with %d logged in with : %s\n", i, clients.list[i].username);
                disconnect(i);
            }
            else
            {
                updateLastMessageReceived(clients.list[i].username, message.id);
            }
        }
    }
}

void disconnect(int index)
{
    clients.list[index].connected = 0;
}