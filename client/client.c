#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include <ncurses.h>
#include <pthread.h>

#include "client_structures.h"

extern int errno;

#define PORT 3001

char message[256];
int msg_index = 0;

int connected[1] = {1};

int sd;

messages_collection messages;

static void *treat_send_messages(void *);

int send_message();
void append_message(char message[256]);
void display_messages();
void refresh_message();

int main (int argc, char *argv[])
{
  struct sockaddr_in server;
  messages.count = 0;

  pthread_t send_message_thread;

  initscr();
  clear();
  noecho();
  raw();

  if ((sd = socket (AF_INET, SOCK_STREAM, 0)) == -1)
  {
    perror ("Error at socket.\n");
    return errno;
  }

  server.sin_family = AF_INET;

  server.sin_addr.s_addr = inet_addr(argv[1]);

  server.sin_port = htons (PORT);

  if (connect (sd, (struct sockaddr *) &server,sizeof (struct sockaddr)) == -1)
  {
    perror ("Error connecting.\n");
    return errno;
  }

  pthread_create(&send_message_thread, NULL, treat_send_messages, NULL);

  char server_msg[256];
  while(connected[0]) 
  {
    printw("connread %d", connected[0]);
    refresh();
    if (read (sd, &server_msg, 256) < 0)
    {
      perror ("Eroare la read() de la server.\n");
      return errno;
    }

    append_message(server_msg);
    display_messages();
  }
  printw("Am terminat de citit de la server");
  if(pthread_join(send_message_thread, NULL)) 
  {
    perror("Error joining thread\n");
    return errno;
  }
  refresh();
  /* inchidem conexiunea, am terminat */
  endwin();
  close(sd);
}

void *treat_send_messages(void *arg)
{
  printw("Conn: %d -> You: ", connected[0]);
  refresh();
  while(connected[0])
  {
    int ch;
    ch = getch();
    if (ch == '\n')
    {
      message[msg_index] = '\0';
      send_message();
      printw("conn: %d", connected[0]);
      refresh();
    } 
    else if (ch == 127) // delete because backspace does not exists
    {
      if(msg_index > 0)
      {
        msg_index--;
        message[msg_index] = '\0';
        display_messages();
      }
    }
    else 
    {
      printw("%c", ch);      
      message[msg_index++] = ch;
      message[msg_index] = '\0';
    }
  }
  return NULL;
}

int send_message()
{
  if (strcmp(message, "/close") == 0)
  {
    connected[0] = 0;
  } 
  
  if (write (sd,&message, sizeof(message)) <= 0)
  {
    perror ("Eroare la write() spre server.\n");
    return errno;
  }
  refresh_message();
}

void append_message(char message[256])
{
  strcpy(messages.list[messages.count++], message);
}

void display_messages()
{
  clear();
  int index;
  for(index = 0; index < messages.count; index ++)
  {
    printw("%s\n", messages.list[index]);
  }
  printw("You: %s", message);
  refresh();
}

void refresh_message()
{
  msg_index = 0;
  message[msg_index] = '\0';
}