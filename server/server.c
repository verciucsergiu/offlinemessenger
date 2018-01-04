
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

#define PORT 3001

extern int errno;

typedef struct thread {
	int id;
	int client;
}thread;

typedef struct clinets_colletion {
    int count;
    int clients[200];
} clinets_colletion;

clinets_colletion clients;

static void *treat_client(void *);
void raspunde(void *);
void send_msg(char *);

void append_client(int,int);


int main() 
{
    struct sockaddr_in server;
    struct sockaddr_in from;

    char *msg;
    int sd, pid;
    pthread_t th[100];
    int i=0;

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

    if(bind(sd, (struct sockaddr *) &server, sizeof(struct sockaddr)) == -1)
    {
        perror("Eroare la bind.\n");
        return errno;
    }

    if (listen(sd, 2) == -1) 
    {
        perror("Eroare la listen.\n");
        return errno;
    }

    while(1)
    {
        int client;
        thread *td;
        int length = sizeof(from);
        printf("Asteptam la portul %d...\n", PORT);
        fflush (stdout);
        if ((client = accept (sd, (struct sockaddr *) &from, &length)) < 0)
        {
            perror ("[server]Eroare la accept().\n");
            continue;
        }

        td=(struct thread*)malloc(sizeof(struct thread));	
        td->id=i;
        td->client=client;

        /* adaugarea clientului intr-o coletie.*/
        append_client(i, client);

        pthread_create(&th[i++], NULL, &treat_client, td);

    }

}

void append_client(int index, int client)
{
    clients.clients[index] = client;
    clients.count = index;
    printf("Clientul cu id: %d a fost adaugat in lista cu success!\n", index);
}

static void *treat_client(void * arg)
{		
		struct thread tdL; 
		tdL= *((struct thread*)arg);
		printf ("[thread]- %d - Asteptam mesajul...\n", tdL.id);
		fflush (stdout);
		pthread_detach(pthread_self());
		raspunde((struct thData*)arg);
		/* am terminat cu acest client, inchidem conexiunea */
		close ((intptr_t)arg);
		return NULL;
  		
}

void raspunde(void *arg)
{
    while(1)
    {
        char msg[256];
        int i=0;
        struct thread tdL; 
        tdL= *((struct thread*)arg);
        if (read (tdL.client, &msg,sizeof(msg)) <= 0)
        {
            printf("[Thread %d]\n",tdL.id);
            perror ("Eroare la read() de la client.\n");
        
        }
        
        // printf ("[Thread %d]Mesajul a fost receptionat...%s\n",tdL.id, msg);
                    
        // printf("[Thread %d]Trimitem mesajul inapoi...%s\n",tdL.id, msg);
                
        send_msg(msg);
    }

}

void send_msg(char *msg) 
{
    /* returnam mesajul clientului */  
    printf("Meajul trimis catre clienti: %s\n", msg);
    int i;
    for (i = 0; i <= clients.count; i++) 
    {
        if (write (clients.clients[i], &msg, sizeof(int)) <= 0)
        {
            printf("[Clinet %d] ", i);
            perror ("[Thread]Eroare la write() catre client.\n");
        }
        else
        {
            printf ("[Client %d]Mesajul a fost trasmis cu succes.\n", i);
        }
    }
}