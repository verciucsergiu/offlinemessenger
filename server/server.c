
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
#include "../helper/helpers.h"

#define PORT 3001

extern int errno;

clinets_colletion clients;
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

    printf("Asteptam la portul %d...\n", PORT);
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
    clients.clients[index] = client;
    clients.count = index;
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
        char request[256];
        memset( request, '\0', sizeof(char)*256);
        struct thread currentThread;
        currentThread = *((struct thread *)arg);
        if (read(currentThread.client, &request, sizeof(request)) <= 0)
        {
            printf("[Thread %d] -> Disconnected. Connection lost.", currentThread.id);
            return;
        }
        else
        {
            porcessRequest(request, &currentThread);
        }
    }
}

void porcessRequest(char request[256], void *arg)
{
    char *duplicatedRequest = strdup(request);
    char *requestType = strtok(duplicatedRequest, " ");
    if (strcmp(requestType, "login") == 0)
    {
        strcpy(request, request + 5);
        struct thread currentThread;
        currentThread = *((struct thread *)arg);

        LoginModel model = deserializeLoginModel(request);

        if (authenticateUser(model.username, model.password))
        {

            if (write(currentThread.client, "200\0", 256) <= 0)
            {
                perror("[Thread]Eroare la write() catre client.\n");
            }
        }
        else
        {
            if (write(currentThread.client, "404\0", 256) <= 0)
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
}

void sendUserMessage(char msg[1024])
{
    printf("Mesajul trimis catre clienti: %s\n", msg);
    int i;
    for (i = 0; i <= clients.count; i++)
    {
        if (write(clients.clients[i], msg, strlen(msg)+1) <= 0)
        {
            printf("[Clinet %d] ", i);
            perror("[Thread]Eroare la write() catre client.\n");
        }
        else
        {
            printf("[Client %d]Mesajul a fost trasmis cu succes.\n", i);
        }
    }
}
