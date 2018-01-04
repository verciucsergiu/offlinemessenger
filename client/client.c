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

#include "client_structures.h"

extern int errno;

#define PORT 3001

char message[256];
int msg_index = 0;

int connected = 1;

messages_collection messages;

int send_message(int descriptor);
void append_message(char message[256]);
void display_messages();
void refresh_message();

int main (int argc, char *argv[])
{
  int sd;
  struct sockaddr_in server;
  messages.count = 0;

  initscr();
  clear();
  noecho();
  raw();

  if ((sd = socket (AF_INET, SOCK_STREAM, 0)) == -1)
  {
    perror ("Eroare la socket.\n");
    return errno;
  }

  server.sin_family = AF_INET;

  server.sin_addr.s_addr = inet_addr(argv[1]);

  server.sin_port = htons (PORT);

  if (connect (sd, (struct sockaddr *) &server,sizeof (struct sockaddr)) == -1)
  {
    perror ("Eroare la connect.\n");
    return errno;
  }
  
  if(fork()) 
  {
    while(connected)
    {
      int ch;
      ch = getch();
      if (ch == '\n')
      {
        message[msg_index] = '\0';
        send_message(sd);
      } 
      else if (ch == 127) // delete because backspace does not exists :(
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
  }
  else
  {
    char server_msg[256];
    while(connected) 
    {
      if (read (sd, &server_msg, 256) < 0)
      {
        perror ("Eroare la read() de la server.\n");
        return errno;
      }
      /* afisam mesajul primit */
      append_message(server_msg);
      display_messages();
      refresh();
    }
  }
  /* inchidem conexiunea, am terminat */
  endwin();
  close (sd);
}


int send_message(int descriptor)
{
  if (strcmp(message, "/close") == 0)
  {
    connected = 0;
  } 
  else if (write (descriptor,&message, sizeof(message)) <= 0)
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

void display_input(char message[256])
{
  printw("=> you were typing: %s", message);
}

void display_messages()
{
  clear();
  int index;
  for(index = 0; index < messages.count; index ++)
  {
    printw("%s\n", messages.list[index]);
  }
  refresh();
}

void refresh_message()
{
  msg_index = 0;
  message[msg_index] = '\0';
}