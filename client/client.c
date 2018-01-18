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

#include "../helper/helpers.h"

typedef struct messages_collection
{
  int count;
  Message list[100];
} messages_collection;

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
void appendMessage(Message);
void displayMessage();
void refreshMessages();
void attemptLogin();
int closeApp();
int isCharacterAccepted(int);
void initWindow();
LoginModel getUserLoginModel();

int main(int argc, char *argv[])
{
  struct sockaddr_in server;
  messages.count = 0;

  pthread_t send_message_thread;

  initscr();
  clear();
  noecho();
  raw();

  if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
  {
    perror("Error at socket.\n");
    return closeApp();
  }

  server.sin_family = AF_INET;

  server.sin_addr.s_addr = inet_addr(argv[1]);

  server.sin_port = htons(PORT);

  if (connect(sd, (struct sockaddr *)&server, sizeof(struct sockaddr)) == -1)
  {
    perror("Error connecting.\n");
    return closeApp();
  }

  initWindow();

  pthread_create(&send_message_thread, NULL, treatSendMessages, NULL);

  char server_msg[1024];
  while (connected[0])
  {
    if (read(sd, &server_msg, 1024) < 0)
    {
      perror("Eroare la read() de la server.\n");
      closeApp();
      break;
    }

    if (strcmp(server_msg, "close") == 0)
    {
      break;
    }

    Message msg = deserializeMessage(server_msg);
    appendMessage(msg);
    displayMessage();
  }

  if (pthread_join(send_message_thread, NULL))
  {
    perror("Error joining thread\n");
    return closeApp();
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
    else if (isCharacterAccepted(ch))
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
  if (strlen(message) > 0)
  {
    if (strcmp(message, "/close") == 0)
    {
      connected[0] = 0;
      if (write(sd, "close", 6) <= 0)
      {
        perror("Eroare la write() spre server.\n");
        return errno;
      }
    }
    else
    {
      Message msg;
      strcpy(msg.id, "0");
      strcpy(msg.replyTo, "0");
      strcpy(msg.text, message);
      refreshMessages();
      strcpy(msg.username, connectedUser.username);
      char *messageJson = serializeMessage(msg);
      char *json = malloc(sizeof(char) * 300);
      strcpy(json, "send ");
      strcat(json, messageJson);

      if (write(sd, json, strlen(json) + 1) <= 0)
      {
        perror("Eroare la write() spre server.\n");
        return closeApp();
      }
    }
  }
}

void appendMessage(Message message)
{
  messages.list[messages.count++] = message;
}

void displayMessage()
{
  clear();
  int index;
  for (index = 0; index < messages.count; index++)
  {
    attron(A_BOLD);
    printw("%s", messages.list[index].username);
    indentMessages(15 - strlen(messages.list[index].username));
    printw("@ %s -> %s : ", messages.list[index].id, messages.list[index].replyTo);
    attroff(A_BOLD);
    printw("%s\n", messages.list[index].text);
  }
  printw("%s: %s", connectedUser.username, message);
  refresh();
}

void indentMessages(int spaces)
{
  int i;
  for (i = 0; i < spaces; i++)
  {
    printw(" ");
  }
}

void refreshMessages()
{
  msg_index = 0;
  message[msg_index] = '\0';
}

LoginModel getUserLoginModel(char *type)
{
  int userInserted = 0;
  int username_index = 0, password_index = 0;
  LoginModel model;
  printw("%s\n", type);
  printw("Username: ");
  refresh();
  while (1)
  {
    if (!userInserted)
    {
      int ch;
      ch = getch();
      if (ch == '\n' && username_index > 0)
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
          displayLogin(model, userInserted, type);
        }
      }
      else if (isCharacterAccepted(ch))
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
      if (ch == '\n' && password_index > 0)
      {
        model.password[password_index] = '\0';
        printw("\n");
        refresh();
        return model;
      }
      else if (ch == 127) // delete because backspace does not exists
      {
        if (password_index > 0)
        {
          password_index--;
          model.password[password_index] = '\0';
          displayLogin(model, userInserted, type);
        }
      }
      else if (isCharacterAccepted(ch))
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
  char response[6];
  char *loginJson = serializeLoginModel(model);
  char *json = malloc(sizeof(char) * 300);
  strcpy(json, "login ");
  strcat(json, loginJson);

  if (write(sd, json, strlen(json)) <= 0)
  {
    perror("Eroare la write() spre server.\n");
    return closeApp();
  }

  if (read(sd, &response, 6) < 0)
  {
    perror("Eroare la read() de la server.\n");
    return closeApp();
  }

  if (strcmp(response, "200") == 0)
  {
    strcpy(connectedUser.username, model.username);
    return 1;
  }
  else
  {
    return 0;
  }
}

int requestRegister(LoginModel model)
{
  char response[6];
  char *loginJson = serializeLoginModel(model);
  char *json = malloc(sizeof(char) * 300);
  strcpy(json, "register ");
  strcat(json, loginJson);

  if (write(sd, json, strlen(json)) <= 0)
  {
    perror("Eroare la write() spre server.\n");
    return closeApp();
  }

  if (read(sd, &response, 6) < 0)
  {
    perror("Eroare la read() de la server.\n");
    return closeApp();
  }

  if (strcmp(response, "201") == 0)
  {
    return 1;
  }
  else if (strcmp(response, "500"))
  {
    return -1;
  }
  else
  {
    return 0;
  }
}

void displayLogin(LoginModel model, int userInserted, char *type)
{
  int i = 0;
  clear();
  printw("%s\n", type);
  printw("Username: %s", model.username);
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

void initWindow()
{
  attron(A_BOLD);
  printw("\tOfflineMessenger\n\nChoose:\n\t1.Login\n\t2.Register\n\t3.Close\n");
  attroff(A_BOLD);
  refresh();
  int ch;
  ch = getch();
  if (ch != '1' && ch != '2' && ch != '3')
  {
    clear();
    initWindow();
  }
  else if (ch == '3')
  {
    closeApp();
  }
  else if (ch == '1')
  {
    clear();
    if (requstLogin(getUserLoginModel("Login!")))
    {
      printw("Login successfully!\n");
      printw("Press any key to continue!");
      refresh();
      getch();
      clear();
      return;
    }
    else
    {
      printw("Invalid credentials!\n");
      printw("Press any key to continue!");
      refresh();
      getch();
      clear();
      initWindow();
    }
  }
  else if (ch == '2')
  {
    clear();
    int reg = requestRegister(getUserLoginModel("Register!"));
    if (reg == 1)
    {
      printw("Register successfully!\n");
    }
    else if (reg == 0)
    {
      printw("Register failed! Username is already used!\n");
    }
    else
    {
      printw("Register failed! Unknown issue! Pleas try again!\n");
    }
    printw("Press any key to continue!");
    refresh();

    getch();
    clear();
    initWindow();
  }
}

int closeApp()
{
  refresh();
  endwin();
  close(sd);
  return 1;
}

int isCharacterAccepted(int ch)
{
  if (ch == KEY_UP || ch == KEY_DOWN || ch == KEY_LEFT || ch == KEY_RIGHT || ch == 39)
  {
    return 0;
  }

  if (ch >= 35 && ch <= 57)
  {
    return 1;
  }
  if (ch >= 63 && ch <= 90)
  {
    return 1;
  }
  if (ch >= 97 && ch <= 122)
  {
    return 1;
  }
  if (ch == 32 || ch == 33)
  {
    return 1;
  }

  return 0;
}