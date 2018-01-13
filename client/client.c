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
#include "../helper/helpers.h"
#include "../shared/structs.h"

extern int errno;

#define PORT 3001

char message[256];
int msg_index = 0;

int connected[1] = {1};
User connectedUser;
int sd;

messages_collection messages;

static void *treatSendMessages(void *);

int treatMessage();
void appendMessage(char message[256]);
void displayMessage();
void refreshMessages();
void attemptLogin();

int main(int argc, char *argv[])
{
  struct sockaddr_in server;
  messages.count = 0;
  // connectedUser = malloc(sizeof(connectedUser));

  pthread_t send_message_thread;

  initscr();
  clear();
  noecho();
  raw();

  if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
  {
    perror("Error at socket.\n");
    return errno;
    refresh();
  }

  server.sin_family = AF_INET;

  server.sin_addr.s_addr = inet_addr(argv[1]);

  server.sin_port = htons(PORT);

  if (connect(sd, (struct sockaddr *)&server, sizeof(struct sockaddr)) == -1)
  {
    perror("Error connecting.\n");
    return errno;
  }

  loginUser();

  pthread_create(&send_message_thread, NULL, treatSendMessages, NULL);

  char server_msg[256];
  while (connected[0])
  {
    if (read(sd, &server_msg, 256) < 0)
    {
      perror("Eroare la read() de la server.\n");
      return errno;
    }

    appendMessage(server_msg);
    displayMessage();
  }
  // printw("Am terminat de citit de la server");
  if (pthread_join(send_message_thread, NULL))
  {
    perror("Error joining thread\n");
    return errno;
  }

  refresh();
  endwin();
  close(sd);
}

void *treatSendMessages(void *arg)
{
  printw("%s: ", connectedUser.username);
  refresh();
  while (connected[0])
  {
    int ch;
    ch = getch();
    if (ch == '\n')
    {
      message[msg_index] = '\0';
      treatMessage();
      refresh();
    }
    else if (ch == 127) // delete because backspace does not exists
    {
      if (msg_index > 0)
      {
        msg_index--;
        message[msg_index] = '\0';
        displayMessage();
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

int treatMessage()
{
  if (strcmp(message, "/close") == 0)
  {
    connected[0] = 0;
  }

  if (write(sd, &message, sizeof(message)) <= 0)
  {
    perror("Eroare la write() spre server.\n");
    return errno;
  }
  refreshMessages();
}

void appendMessage(char message[256])
{
  strcpy(messages.list[messages.count++], message);
}

void displayMessage()
{
  clear();
  int index;
  for (index = 0; index < messages.count; index++)
  {
    printw("%s\n", messages.list[index]);
  }
  printw("%s: %s", connectedUser.username, message);
  refresh();
}

void refreshMessages()
{
  msg_index = 0;
  message[msg_index] = '\0';
}

void loginUser()
{
  int userInserted = 0;
  int username_index = 0, password_index = 0;
  LoginModel model;
  printw("You need to login to be able chat!\nUsername: ");
  while (1)
  {
    if (!userInserted)
    {
      int ch;
      ch = getch();
      if (ch == '\n')
      {
        model.username[username_index] = '\0';
        printw("\nPassword: ");
        userInserted = 1;
        refresh();
      }
      else if (ch == 127) // delete because backspace does not exists
      {
        if (username_index > 0)
        {
          username_index--;
          model.username[username_index] = '\0';
          displayLogin(model, userInserted);
        }
      }
      else
      {
        printw("%c", ch);
        model.username[username_index++] = ch;
        model.username[username_index] = '\0';
      }
    }
    else
    {
      int ch;
      ch = getch();
      if (ch == '\n')
      {
        model.password[password_index] = '\0';
        printw("\n");
        refresh();

        if (requstLogin(model))
        {
          return;
        }
        else
        {
          loginUser();
        }
      }
      else if (ch == 127) // delete because backspace does not exists
      {
        if (password_index > 0)
        {
          password_index--;
          model.password[password_index] = '\0';
          displayLogin(model, userInserted);
        }
      }
      else
      {
        printw("*");
        model.password[password_index++] = ch;
        model.password[password_index] = '\0';
      }
    }
  }
}

int requstLogin(LoginModel model)
{

  char response[256];
  char *loginJson = serializeLoginModel(model);
  char *json = malloc(sizeof(char) * 300);
  strcpy(json, "login ");
  strcat(json, loginJson);

  if (write(sd, json, 256) <= 0)
  {
    perror("Eroare la write() spre server.\n");
    return errno;
  }
  printw("\nAttempting to login...");
  refresh();
  if (read(sd, &response, 256) < 0)
  {
    perror("Eroare la read() de la server.\n");
    return errno;
  }

  if (strcmp(response, "200") == 0)
  {
    strcpy(connectedUser.username, model.username);
    return 1;
  }
  else
  {
    clear();
    printw("Invalid credentials!\n");
    refresh();
    return 0;
  }
}

void displayLogin(LoginModel model, int userInserted)
{
  int i = 0;
  clear();
  printw("You need to login to be able chat!\nUsername: %s", model.username);
  if (userInserted)
  {
    printw("\nPassword: ");
    for (i = 0; i < strlen(model.password); i++)
    {
      printw("*");
    }
  }
  refresh();
}